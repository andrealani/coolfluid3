// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <set>

#include "Common/Foreach.hpp"
#include "Common/Log.hpp"
#include "Common/Signal.hpp"
#include "Common/OptionT.hpp"
#include "Common/OptionURI.hpp"
#include "Common/MPI/PE.hpp"
#include "Common/MPI/datatype.hpp"
#include "Common/MPI/Buffer.hpp"
#include "Common/MPI/debug.hpp"
#include "Common/StringConversion.hpp"

#include "Common/XML/Protocol.hpp"
#include "Common/XML/SignalOptions.hpp"

#include "Mesh/CMesh.hpp"
#include "Mesh/CList.hpp"
#include "Mesh/CMeshPartitioner.hpp"
#include "Mesh/CDynTable.hpp"
#include "Mesh/CNodes.hpp"
#include "Mesh/CRegion.hpp"
#include "Mesh/Manipulations.hpp"
#include "Mesh/CMeshElements.hpp"

namespace CF {
namespace Mesh {


  using namespace Common;
  using namespace Common::XML;
  using namespace Common::mpi;

//////////////////////////////////////////////////////////////////////////////

CMeshPartitioner::CMeshPartitioner ( const std::string& name ) :
    CMeshTransformer(name),
    m_base(0),
    m_nb_parts(mpi::PE::instance().size())
{
  m_options.add_option<OptionT <Uint> >("nb_parts", m_nb_parts)
      ->description("Total number of partitions (e.g. number of processors)")
      ->pretty_name("Number of Partitions")
      ->link_to(&m_nb_parts)
      ->mark_basic();

  m_global_to_local = create_static_component_ptr<CMap<Uint,Uint> >("global_to_local");
  m_lookup = create_static_component_ptr<CUnifiedData >("lookup");

  regist_signal( "load_balance" )
    ->description("Partitions and migrates elements between processors")
    ->pretty_name("Load Balance")
    ->connect ( boost::bind ( &CMeshPartitioner::load_balance,this, _1 ) )
    ->signature ( boost::bind ( &CMeshPartitioner::load_balance_signature, this, _1));
}

//////////////////////////////////////////////////////////////////////////////

void CMeshPartitioner::execute()
{
  CMesh& mesh = *m_mesh.lock();
  initialize(mesh);
  partition_graph();
  //show_changes();
  migrate();
}

//////////////////////////////////////////////////////////////////////////////

void CMeshPartitioner::load_balance( SignalArgs& node  )
{
  SignalOptions options( node );

  URI path = options.value<URI>("mesh");

  if( path.scheme() != URI::Scheme::CPATH )
    throw ProtocolError( FromHere(), "Wrong protocol to access the Domain component, expecting a \'cpath\' but got \'" + path.string() +"\'");

  // get the domain
  CMesh::Ptr mesh = access_component_ptr( path.path() )->as_ptr<CMesh>();
  if ( is_null(mesh) )
    throw CastingFailed( FromHere(), "Component in path \'" + path.string() + "\' is not a valid CMesh." );

  m_mesh = mesh;

  execute();

}

//////////////////////////////////////////////////////////////////////

void CMeshPartitioner::load_balance_signature ( Common::SignalArgs& node )
{
  SignalOptions options( node );

  options.add_option<OptionURI>("mesh", URI())
      ->description("Mesh to load balance");
}

//////////////////////////////////////////////////////////////////////

void CMeshPartitioner::initialize(CMesh& mesh)
{
  m_mesh = mesh.as_ptr<CMesh>();

  CNodes& nodes = mesh.nodes();
  Uint tot_nb_owned_nodes(0);
  for (Uint i=0; i<nodes.size(); ++i)
  {
    if (nodes.is_ghost(i) == false)
      ++tot_nb_owned_nodes;
  }

  Uint tot_nb_owned_elems(0);
  boost_foreach( CElements& elements, find_components_recursively<CElements>(mesh) )
  {
    tot_nb_owned_elems += elements.size();
  }

  Uint tot_nb_owned_obj = tot_nb_owned_nodes + tot_nb_owned_elems;

  std::vector<Uint> nb_nodes_per_proc(mpi::PE::instance().size());
  std::vector<Uint> nb_elems_per_proc(mpi::PE::instance().size());
  std::vector<Uint> nb_obj_per_proc(mpi::PE::instance().size());
  mpi::PE::instance().all_gather(tot_nb_owned_nodes, nb_nodes_per_proc);
  mpi::PE::instance().all_gather(tot_nb_owned_elems, nb_elems_per_proc);
  mpi::PE::instance().all_gather(tot_nb_owned_obj, nb_obj_per_proc);
  m_start_id_per_part.resize(mpi::PE::instance().size());
  m_start_node_per_part.resize(mpi::PE::instance().size());
  m_start_elem_per_part.resize(mpi::PE::instance().size());
  m_end_id_per_part.resize(mpi::PE::instance().size());
  m_end_node_per_part.resize(mpi::PE::instance().size());
  m_end_elem_per_part.resize(mpi::PE::instance().size());

  Uint start_id(0);
  for (Uint p=0; p<mpi::PE::instance().size(); ++p)
  {
    m_start_id_per_part[p]   = start_id;
    m_end_id_per_part[p]     = start_id + nb_obj_per_proc[p];
    m_start_node_per_part[p] = start_id;
    m_end_node_per_part[p]   = start_id + nb_nodes_per_proc[p];
    m_start_elem_per_part[p] = start_id + nb_nodes_per_proc[p];
    m_end_elem_per_part[p]   = start_id + nb_nodes_per_proc[p] + nb_elems_per_proc[p];

    start_id += nb_obj_per_proc[p];
  }

  m_nodes_to_export.resize(m_nb_parts);
  m_elements_to_export.resize(find_components_recursively<CElements>(mesh.topology()).size());
  for (Uint c=0; c<m_elements_to_export.size(); ++c)
    m_elements_to_export[c].resize(m_nb_parts);

  build_global_to_local_index(mesh);
  build_graph();
}

//////////////////////////////////////////////////////////////////////////////

void CMeshPartitioner::build_global_to_local_index(CMesh& mesh)
{


  CNodes& nodes = mesh.nodes();

  m_lookup->add(nodes);
  boost_foreach ( CEntities& elements, mesh.topology().elements_range() )
    m_lookup->add(elements);

  m_nb_owned_obj = 0;
  CList<Uint>& node_glb_idx = nodes.glb_idx();
  for (Uint i=0; i<nodes.size(); ++i)
  {
    if (!nodes.is_ghost(i))
    {
      //std::cout << mpi::PE::instance().rank() << " --   owning node " << node_glb_idx[i] << std::endl;
      ++m_nb_owned_obj;
    }
  }

  boost_foreach ( CEntities& elements, mesh.topology().elements_range() )
  {
    m_nb_owned_obj += elements.size();
  }

  Uint tot_nb_obj = m_lookup->size();
  m_global_to_local->reserve(tot_nb_obj);
  Uint loc_idx=0;
  //CFinfo << "adding nodes to map " << CFendl;
  boost_foreach (Uint glb_idx, node_glb_idx.array())
  {
    //CFinfo << "  adding node with glb " << glb_idx << CFendl;
    if (nodes.is_ghost(loc_idx) == false)
    {
      cf_assert(glb_idx >= m_start_id_per_part[mpi::PE::instance().rank()]);
      cf_assert(glb_idx >= m_start_node_per_part[mpi::PE::instance().rank()]);
      cf_assert_desc(to_str(glb_idx)+">="+to_str(m_end_id_per_part[mpi::PE::instance().rank()]),glb_idx < m_end_id_per_part[mpi::PE::instance().rank()]);
      cf_assert_desc(to_str(glb_idx)+">="+to_str(m_end_node_per_part[mpi::PE::instance().rank()]),glb_idx < m_end_node_per_part[mpi::PE::instance().rank()]);
    }
    else
    {
      cf_assert(glb_idx > m_start_id_per_part[mpi::PE::instance().rank()] ||
                glb_idx <= m_end_id_per_part[mpi::PE::instance().rank()]);

      cf_assert(glb_idx > m_start_node_per_part[mpi::PE::instance().rank()] ||
                glb_idx <= m_end_node_per_part[mpi::PE::instance().rank()]);
    }

    m_global_to_local->insert_blindly(glb_idx,loc_idx++);
  }

  //CFinfo << "adding elements " << CFendl;
  boost_foreach ( CElements& elements, find_components_recursively<CElements>(mesh))
  {
    boost_foreach (Uint glb_idx, elements.glb_idx().array())
    {
      cf_assert_desc(to_str(glb_idx)+"<"+to_str(m_start_elem_per_part[mpi::PE::instance().rank()]),glb_idx >= m_start_elem_per_part[mpi::PE::instance().rank()]);
      cf_assert_desc(to_str(glb_idx)+">="+to_str(m_end_elem_per_part[mpi::PE::instance().rank()]),glb_idx < m_end_elem_per_part[mpi::PE::instance().rank()]);
      cf_assert_desc(to_str(glb_idx)+">="+to_str(m_end_id_per_part[mpi::PE::instance().rank()]),glb_idx < m_end_id_per_part[mpi::PE::instance().rank()]);
      m_global_to_local->insert_blindly(glb_idx,loc_idx++);
      //CFinfo << "  adding element with glb " << glb_idx << CFendl;
    }
  }

  m_global_to_local->sort_keys();
}

//////////////////////////////////////////////////////////////////////////////

void CMeshPartitioner::show_changes()
{
  Uint nb_changes(0);
  boost_foreach(std::vector<Uint>& export_nodes_to_part, m_nodes_to_export)
    nb_changes += export_nodes_to_part.size();

  boost_foreach(std::vector<std::vector<Uint> >& export_elems_from_region, m_elements_to_export)
    boost_foreach(std::vector<Uint>& export_elems_from_region_to_part, export_elems_from_region)
      nb_changes += export_elems_from_region_to_part.size();

  if (nb_changes > 0)
  {
    PEProcessSortedExecute(-1,
      for (Uint to_part=0; to_part<m_nodes_to_export.size(); ++to_part)
      {
        std::cout << "[" << mpi::PE::instance().rank() << "] export nodes to part " << to_part << ":  ";
        for (Uint n=0; n<m_nodes_to_export[to_part].size(); ++n)
          std::cout << m_nodes_to_export[to_part][n] << " ";
        std::cout << "\n";
      }
      for (Uint comp=0; comp<m_elements_to_export.size(); ++comp)
      {
        std::string elements = m_lookup->components()[comp+1]->uri().path();
        for (Uint to_part=0; to_part<m_elements_to_export[comp].size(); ++to_part)
        {
          std::cout << "[" << mpi::PE::instance().rank() << "] export " << elements << " to part " << to_part << ":  ";
          for (Uint e=0; e<m_elements_to_export[comp][to_part].size(); ++e)
            std::cout << m_elements_to_export[comp][to_part][e] << " ";
          std::cout << "\n";
        }
      }
    )
  }
  else
  {
    CFinfo << "No changes in partitions" << CFendl;
  }
}

//////////////////////////////////////////////////////////////////////////////

boost::tuple<Uint,Uint> CMeshPartitioner::location_idx(const Uint glb_obj) const
{
  CMap<Uint,Uint>::const_iterator itr = m_global_to_local->find(glb_obj);
  if (itr != m_global_to_local->end() )
  {
    return m_lookup->location_idx(itr->second);
  }
  return boost::make_tuple(0,0);
}

//////////////////////////////////////////////////////////////////////////////

boost::tuple<Component::Ptr,Uint> CMeshPartitioner::location(const Uint glb_obj) const
{
  return m_lookup->location( (*m_global_to_local)[glb_obj] );
}

//////////////////////////////////////////////////////////////////////////////





template <typename T>
void flex_all_gather(const std::vector<T>& send, std::vector<std::vector<T> >& recv)
{
  std::vector<int> strides;
  mpi::PE::instance().all_gather((int)send.size(),strides);
  std::vector<int> displs(strides.size());
  if (strides.size())
  {
    int sum_strides = strides[0];
    displs[0] = 0;
    for (Uint i=1; i<strides.size(); ++i)
    {
      displs[i] = displs[i-1] + strides[i-1];
      sum_strides += strides[i];
    }
    std::vector<Uint> recv_linear(sum_strides);
    MPI_CHECK_RESULT(MPI_Allgatherv, ((void*)&send[0], (int)send.size(), get_mpi_datatype<T>(), &recv_linear[0], &strides[0], &displs[0], get_mpi_datatype<T>(), mpi::PE::instance().communicator()));
    recv.resize(strides.size());
    for (Uint i=0; i<strides.size(); ++i)
    {
      recv[i].resize(strides[i]);
      for (Uint j=0; j<strides[i]; ++j)
      {
        recv[i][j]=recv_linear[displs[i]+j];
      }
    }
  }
  else
  {
    recv.resize(0);
  }
}

template <typename T>
void flex_all_to_all(const std::vector<std::vector<T> >& send, std::vector<std::vector<T> >& recv)
{
  std::vector<int> send_strides(send.size());
  std::vector<int> send_displs(send.size());
  for (Uint i=0; i<send.size(); ++i)
    send_strides[i] = send[i].size();

  send_displs[0] = 0;
  for (Uint i=1; i<send.size(); ++i)
    send_displs[i] = send_displs[i-1] + send_strides[i-1];

  std::vector<T> send_linear(send_displs.back()+send_strides.back());
  for (Uint i=0; i<send.size(); ++i)
    for (Uint j=0; j<send[i].size(); ++j)
      send_linear[send_displs[i]+j] = send[i][j];

  std::vector<int> recv_strides(mpi::PE::instance().size());
  std::vector<int> recv_displs(mpi::PE::instance().size());
  mpi::PE::instance().all_to_all(send_strides,recv_strides);
  recv_displs[0] = 0;
  for (Uint i=1; i<mpi::PE::instance().size(); ++i)
    recv_displs[i] = recv_displs[i-1] + recv_strides[i-1];

  std::vector<Uint> recv_linear(recv_displs.back()+recv_strides.back());
  MPI_CHECK_RESULT(MPI_Alltoallv, (&send_linear[0], &send_strides[0], &send_displs[0], mpi::get_mpi_datatype<Uint>(), &recv_linear[0], &recv_strides[0], &recv_displs[0], get_mpi_datatype<Uint>(), mpi::PE::instance().communicator()));

  recv.resize(recv_strides.size());
  for (Uint i=0; i<recv_strides.size(); ++i)
  {
    recv[i].resize(recv_strides[i]);
    for (Uint j=0; j<recv_strides[i]; ++j)
    {
      recv[i][j]=recv_linear[recv_displs[i]+j];
    }
  }
}

void flex_all_to_all(const std::vector<mpi::Buffer>& send, mpi::Buffer& recv)
{
  std::vector<int> send_strides(send.size());
  std::vector<int> send_displs(send.size());
  for (Uint i=0; i<send.size(); ++i)
    send_strides[i] = send[i].packed_size();

  if (send.size()) send_displs[0] = 0;
  for (Uint i=1; i<send.size(); ++i)
    send_displs[i] = send_displs[i-1] + send_strides[i-1];

  mpi::Buffer send_linear;

  send_linear.resize(send_displs.back()+send_strides.back());
  for (Uint i=0; i<send.size(); ++i)
    send_linear.pack(send[i].buffer(),send[i].packed_size());

  std::vector<int> recv_strides(mpi::PE::instance().size());
  std::vector<int> recv_displs(mpi::PE::instance().size());
  mpi::PE::instance().all_to_all(send_strides,recv_strides);
  if (recv_displs.size()) recv_displs[0] = 0;
  for (Uint i=1; i<mpi::PE::instance().size(); ++i)
    recv_displs[i] = recv_displs[i-1] + recv_strides[i-1];
  recv.reset();
  recv.resize(recv_displs.back()+recv_strides.back());
  MPI_CHECK_RESULT(MPI_Alltoallv, ((void*)send_linear.buffer(), &send_strides[0], &send_displs[0], MPI_PACKED, (void*)recv.buffer(), &recv_strides[0], &recv_displs[0], MPI_PACKED, mpi::PE::instance().communicator()));
  recv.packed_size()=recv_displs.back()+recv_strides.back();
}

////////////////////////////////////////////////////////////////////////////////

void flex_all_to_all(const mpi::Buffer& send, std::vector<int>& send_strides, mpi::Buffer& recv, std::vector<int>& recv_strides)
{
  std::vector<int> send_displs(send_strides.size());
  if (send_strides.size()) send_displs[0] = 0;
  for (Uint i=1; i<send_strides.size(); ++i)
    send_displs[i] = send_displs[i-1] + send_strides[i-1];

  recv_strides.resize(mpi::PE::instance().size());
  std::vector<int> recv_displs(mpi::PE::instance().size());
  mpi::PE::instance().all_to_all(send_strides,recv_strides);
  if (recv_displs.size()) recv_displs[0] = 0;
  for (Uint i=1; i<mpi::PE::instance().size(); ++i)
    recv_displs[i] = recv_displs[i-1] + recv_strides[i-1];
  recv.reset();
  recv.resize(recv_displs.back()+recv_strides.back());
  MPI_CHECK_RESULT(MPI_Alltoallv, ((void*)send.buffer(), &send_strides[0], &send_displs[0], MPI_PACKED, (void*)recv.buffer(), &recv_strides[0], &recv_displs[0], MPI_PACKED, mpi::PE::instance().communicator()));
  recv.packed_size()=recv_displs.back()+recv_strides.back();
}

//////////////////////////////////////////////////////////////////////////////

void CMeshPartitioner::migrate()
{
  if (mpi::PE::instance().is_active() == false)
    return;

  Uint nb_changes(0);
  boost_foreach(std::vector<Uint>& export_nodes_to_part, m_nodes_to_export)
    nb_changes += export_nodes_to_part.size();

  boost_foreach(std::vector<std::vector<Uint> >& export_elems_from_region, m_elements_to_export)
    boost_foreach(std::vector<Uint>& export_elems_from_region_to_part, export_elems_from_region)
      nb_changes += export_elems_from_region_to_part.size();

  if (nb_changes == 0)
    return;


  CMesh& mesh = *m_mesh.lock();
  CNodes& nodes = mesh.nodes();

  // ----------------------------------------------------------------------------
  // ----------------------------------------------------------------------------
  //                            MIGRATION ALGORITHM
  // ----------------------------------------------------------------------------
  // ----------------------------------------------------------------------------

  PackUnpackNodes node_manipulation(nodes);

  // -----------------------------------------------------------------------------
  // REMOVE GHOST NODES AND GHOST ELEMENTS


  for (Uint n=0; n<nodes.size(); ++n)
  {
    if (nodes.is_ghost(n))
      node_manipulation.remove(n);
  }

  // DONT FLUSH YET!!! node_manipulation.flush()

  boost_foreach(CElements& elements, find_components_recursively<CElements>(mesh.topology()) )
  {
    PackUnpackElements element_manipulation(elements);

    if (elements.rank().size() != elements.size())
      throw ValueNotFound(FromHere(),elements.uri().string()+" --> mismatch in element sizes (rank.size() = "+to_str(elements.rank().size())+" , elements.size() = "+to_str(elements.size())+")");
    for (Uint e=0; e<elements.size(); ++e)
    {
      if (elements.rank()[e] != mpi::PE::instance().rank())
        element_manipulation.remove(e);
    }
    /// @todo mechanism not to flush element_manipulation until during real migration
  }

  // -----------------------------------------------------------------------------
  // SET NODE CONNECTIVITY TO GLOBAL NUMBERS BEFORE PARTITIONING

  const CList<Uint>& global_node_indices = mesh.nodes().glb_idx();
  boost_foreach (CEntities& elements, mesh.topology().elements_range())
  {
    boost_foreach ( CTable<Uint>::Row nodes, elements.as_type<CElements>().node_connectivity().array() )
    {
      boost_foreach ( Uint& node, nodes )
      {
        node = global_node_indices[node];
      }
    }
  }

  // -----------------------------------------------------------------------------
  // SEND ELEMENTS AND NODES FROM PARTITIONING ALGORITHM

  std::vector<Component::Ptr> mesh_element_comps = mesh.elements().components();

  mpi::Buffer send_to_proc;  std::vector<int> send_strides(mpi::PE::instance().size());
  mpi::Buffer recv_from_all; std::vector<int> recv_strides(mpi::PE::instance().size());

  // Move elements
  for(Uint i=0; i<mesh_element_comps.size(); ++i)
  {
    CElements& elements = mesh_element_comps[i]->as_type<CElements>();

    send_to_proc.reset();
    recv_from_all.reset();

    PackUnpackElements migrate_element(elements);
    std::vector<Uint> nb_elems_to_send(mpi::PE::instance().size());

    for (Uint r=0; r<mpi::PE::instance().size(); ++r)
    {
      Uint displs = send_to_proc.packed_size();
      for (Uint e=0; e<exported_elements()[i][r].size(); ++e)
        send_to_proc << migrate_element(exported_elements()[i][r][e],PackUnpackElements::MIGRATE);
      send_strides[r] = send_to_proc.packed_size() - displs;
    }

    flex_all_to_all(send_to_proc,send_strides,recv_from_all,recv_strides);

    while(recv_from_all.more_to_unpack())
      recv_from_all >> migrate_element;
    migrate_element.flush();
  }


  // Move nodes
   send_to_proc.reset();
   recv_from_all.reset();

   std::set<Uint> packed_nodes;
   for (Uint r=0; r<mpi::PE::instance().size(); ++r)
   {
     Uint displs = send_to_proc.packed_size();
     for (Uint n=0; n<exported_nodes()[r].size(); ++n)
     {
       send_to_proc << node_manipulation(exported_nodes()[r][n],PackUnpackNodes::MIGRATE);
     }
     send_strides[r] = send_to_proc.packed_size() - displs;
   }

  // STILL DONT FLUSH!!! node_manipulation.flush();


   flex_all_to_all(send_to_proc,send_strides,recv_from_all,recv_strides);


   while (recv_from_all.more_to_unpack())
    recv_from_all >> node_manipulation;

   // FINALLY FLUSH NODES
   node_manipulation.flush();


   // -----------------------------------------------------------------------------
   // MARK EVERYTHING AS OWNED

   for (Uint n=0; n<nodes.size(); ++n)
     nodes.rank()[n] = mpi::PE::instance().rank();

   boost_foreach(CEntities& elements, mesh.topology().elements_range())
   {
     for (Uint e=0; e<elements.size(); ++e)
       elements.rank()[e] = mpi::PE::instance().rank();
   }


  // -----------------------------------------------------------------------------
  // ELEMENTS AND NODES HAVE BEEN MOVED
  // -----------------------------------------------------------------------------


  // -----------------------------------------------------------------------------
  // COLLECT GHOST-NODES TO LOOK FOR ON OTHER PROCESSORS

  std::set<Uint> owned_nodes;
  for (Uint n=0; n<nodes.size(); ++n)
    owned_nodes.insert(nodes.glb_idx()[n]);

  std::set<Uint> ghost_nodes;
  boost_foreach(const CElements& elements, find_components_recursively<CElements>(mesh.topology()))
  {
    boost_foreach(CConnectivity::ConstRow connected_nodes, elements.node_connectivity().array())
    {
      boost_foreach(const Uint node, connected_nodes)
      {
        if (owned_nodes.find(node) == owned_nodes.end())
          ghost_nodes.insert(node);
      }
    }
  }

  std::vector<Uint> request_nodes;  request_nodes.reserve(ghost_nodes.size());
  boost_foreach(const Uint node, ghost_nodes)
    request_nodes.push_back(node);


  // -----------------------------------------------------------------------------
  // SEARCH FOR REQUESTED NODES
  // in  : requested nodes                std::vector<Uint>
  // out : buffer with packed nodes       mpi::Buffer(nodes)

  // COMMUNICATE NODES TO LOOK FOR

  std::vector<std::vector<Uint> > recv_request_nodes;
  flex_all_gather(request_nodes,recv_request_nodes);


  PackUnpackNodes copy_node(nodes);
  std::vector<mpi::Buffer> nodes_to_send(PE::instance().size());
  for (Uint proc=0; proc<PE::instance().size(); ++proc)
  {
    if (proc != PE::instance().rank())
    {

      for (Uint n=0; n<recv_request_nodes[proc].size(); ++n)
      {
        Uint find_glb_idx = recv_request_nodes[proc][n];

        // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
        /// @todo THIS ALGORITHM HAS TO BE IMPROVED (BRUTE FORCE)
        Uint loc_idx=0;
        bool found=false;
        boost_foreach(const Uint glb_idx, nodes.glb_idx().array())
        {

          cf_assert(loc_idx < nodes.size());
          if (glb_idx == find_glb_idx)
          {
            //std::cout << PERank << "copying node " << glb_idx << " from loc " << loc_idx << std::flush;
            nodes_to_send[proc] << copy_node(loc_idx,PackUnpackNodes::COPY);

            break;
          }
          ++loc_idx;
        }
        // +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
      }
    }
  }

  // COMMUNICATE FOUND NODES BACK TO RANK THAT REQUESTED IT

  mpi::Buffer received_nodes_buffer;
  flex_all_to_all(nodes_to_send,received_nodes_buffer);

  // out: buffer containing requested nodes
  // -----------------------------------------------------------------------------

  // ADD GHOST NODES

  PackUnpackNodes add_node(nodes);
  while (received_nodes_buffer.more_to_unpack())
    received_nodes_buffer >> add_node;
  add_node.flush();

  // -----------------------------------------------------------------------------
  // REQUESTED GHOST-NODES HAVE NOW BEEN ADDED
  // -----------------------------------------------------------------------------


  // -----------------------------------------------------------------------------
  // FIX NODE CONNECTIVITY
  std::map<Uint,Uint> glb_to_loc;
  std::map<Uint,Uint>::iterator it;
  bool inserted;
  for (Uint n=0; n<nodes.size(); ++n)
  {
    boost::tie(it,inserted) = glb_to_loc.insert(std::make_pair(nodes.glb_idx()[n],n));
    if (! inserted)
      throw ValueExists(FromHere(), std::string(nodes.is_ghost(n)? "ghost " : "" ) + "node["+to_str(n)+"] with glb_idx "+to_str(nodes.glb_idx()[n])+" already exists as "+to_str(glb_to_loc[n]));
  }
  boost_foreach (CEntities& elements, mesh.topology().elements_range())
  {
    boost_foreach ( CTable<Uint>::Row nodes, elements.as_type<CElements>().node_connectivity().array() )
    {
      boost_foreach ( Uint& node, nodes )
      {
        node = glb_to_loc[node];
      }
    }
  }

  // -----------------------------------------------------------------------------
  // MESH IS NOW COMPLETELY LOAD BALANCED WITHOUT OVERLAP
  // -----------------------------------------------------------------------------

  mesh.update_statistics();
  mesh.elements().reset();
  mesh.elements().update();
}

//////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF
