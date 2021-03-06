// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/regex.hpp>

#include "Common/Signal.hpp"
#include "Common/CBuilder.hpp"
#include "Common/OptionT.hpp"
#include "Common/OptionURI.hpp"
#include "Common/OptionArray.hpp"
#include "Common/Foreach.hpp"
#include "Common/CLink.hpp"
#include "Common/FindComponents.hpp"
#include "Common/MPI/PECommPattern.hpp"

#include "Mesh/LibMesh.hpp"
#include "Mesh/CField.hpp"
#include "Mesh/CRegion.hpp"
#include "Mesh/CNodes.hpp"
#include "Mesh/CMesh.hpp"
#include "Mesh/CTable.hpp"
#include "Mesh/CList.hpp"
#include "Mesh/CFieldView.hpp"
#include "Mesh/CCells.hpp"

namespace CF {
namespace Mesh {

using namespace Common;
using namespace boost::assign;

Common::ComponentBuilder < CField, Component, LibMesh >  CField_Builder;

////////////////////////////////////////////////////////////////////////////////

CField::Basis::Convert::Convert()
{
  all_fwd = boost::assign::map_list_of
      ( CField::Basis::POINT_BASED, "PointBased" )
      ( CField::Basis::ELEMENT_BASED, "ElementBased" )
      ( CField::Basis::CELL_BASED, "CellBased" )
      ( CField::Basis::FACE_BASED, "FaceBased" );

  all_rev = boost::assign::map_list_of
      ("PointBased",    CField::Basis::POINT_BASED )
      ("ElementBased",  CField::Basis::ELEMENT_BASED )
      ("CellBased",     CField::Basis::CELL_BASED )
      ("FaceBased",     CField::Basis::FACE_BASED );
}

CField::Basis::Convert& CField::Basis::Convert::instance()
{
  static CField::Basis::Convert instance;
  return instance;
}


CField::CField ( const std::string& name  ) :
  Component ( name ),
  m_basis(Basis::POINT_BASED),
  m_space_name("space[0]"),
  m_iter_stamp(0),
  m_time_stamp(0.)
{
  mark_basic();

  regist_signal( "create_data_storage" )
    ->connect( boost::bind( &CField::signal_create_data_storage, this, _1 ) )
    ->description("Allocate the data")
    ->pretty_name("Create Storage");

  Option::Ptr uri_option;
  uri_option = m_options.add_option<OptionURI>("Topology", URI("cpath:"));
  uri_option->description("The field tree this field will be registered in");
  uri_option->attach_trigger ( boost::bind ( &CField::config_tree,   this ) );
  uri_option->mark_basic();

  Option::Ptr option;
  option = m_options.add_option< OptionT<std::string> >("FieldType", std::string("PointBased"));
  option->description("The type of the field");
  option->restricted_list() += std::string("ElementBased");
  option->restricted_list() += std::string("CellBased");
  option->restricted_list() += std::string("FaceBased");
  option->attach_trigger ( boost::bind ( &CField::config_field_type,   this ) );
  option->mark_basic();

  option = m_options.add_option< OptionT<std::string> >("Space", m_space_name);
  option->description("The space of the field is based on");
  option->link_to(&m_space_name);
  option->mark_basic();

  std::vector<std::string> var_names;
  var_names.push_back(name);
  option = m_options.add_option<OptionArrayT<std::string> >("VarNames", var_names);
  option->description("Names of the variables");
  option->attach_trigger ( boost::bind ( &CField::config_var_names, this ) );
  option->mark_basic();
  config_var_names();

  std::vector<std::string> var_types;
  var_types.push_back("scalar");
  option = m_options.add_option<OptionArrayT<std::string> >("VarTypes", var_types);
  option->description("Types of the variables");
  option->restricted_list() += std::string("scalar") ,
                                std::string("vector2D"),
                                std::string("vector3D"),
                                std::string("tensor2D"),
                                std::string("tensor3D");
  option->attach_trigger ( boost::bind ( &CField::config_var_types,   this ) );
  option->mark_basic();
  config_var_types();

  m_options.add_option(OptionT<Uint>::create("iteration", m_iter_stamp) )
      ->description("Iteration stamp of the field")
      ->pretty_name("Iteration")
      ->link_to(&m_iter_stamp);

  m_options.add_option(OptionT<Real>::create("time", m_time_stamp) )
      ->description("Time stamp of the field")
      ->pretty_name("Time")
      ->link_to(&m_time_stamp);

  m_topology = create_static_component_ptr<CLink>("topology");
  m_data = create_static_component_ptr<CTable<Real> >("data");
}

////////////////////////////////////////////////////////////////////////////////

void CField::config_var_types()
{
  std::vector<std::string> var_types; option("VarTypes").put_value(var_types);

  boost::regex e_scalar  ("(s(cal(ar)?)?)|1"     ,boost::regex::perl|boost::regex::icase);
  boost::regex e_vector2d("(v(ec(tor)?)?.?2d?)|2",boost::regex::perl|boost::regex::icase);
  boost::regex e_vector3d("(v(ec(tor)?)?.?3d?)|3",boost::regex::perl|boost::regex::icase);
  boost::regex e_tensor2d("(t(ens(or)?)?.?2d?)|4",boost::regex::perl|boost::regex::icase);
  boost::regex e_tensor3d("(t(ens(or)?)?.?3d?)|9",boost::regex::perl|boost::regex::icase);

  m_var_types.resize(var_types.size());
  Uint iVar = 0;
  boost_foreach (std::string& var_type, var_types)
  {
    if (boost::regex_match(var_type,e_scalar))
    {
      var_type="scalar";
      m_var_types[iVar++]=SCALAR;
    }
    else if (boost::regex_match(var_type,e_vector2d))
    {
      var_type="vector_2D";
      m_var_types[iVar++]=VECTOR_2D;
    }
    else if (boost::regex_match(var_type,e_vector3d))
    {
      var_type="vector_3D";
      m_var_types[iVar++]=VECTOR_3D;
    }
    else if (boost::regex_match(var_type,e_tensor2d))
    {
      var_type="tensor_2D";
      m_var_types[iVar++]=TENSOR_2D;
    }
    else if (boost::regex_match(var_type,e_tensor3d))
    {
      var_type="tensor_3D";
      m_var_types[iVar++]=TENSOR_3D;
    }
  }
  // give property a similar look, not all possible regex combinations
  //option("VarTypes").change_value(var_types); // this triggers infinite recursion
}

////////////////////////////////////////////////////////////////////////////////

void CField::config_var_names()
{
  option("VarNames").put_value(m_var_names);
}

////////////////////////////////////////////////////////////////////////////////

void CField::config_field_type()
{
  std::string field_type;
  option("FieldType").put_value(field_type);
  set_basis( Basis::Convert::instance().to_enum(field_type) );
}

////////////////////////////////////////////////////////////////////////////////

std::string CField::var_name(Uint i) const
{
  cf_assert(i<m_var_types.size());
  return m_var_names.size() ? m_var_names[i] : "var";

  //  std::vector<std::string> names;
  //  switch (m_var_types[i])
  //  {
  //    case SCALAR:
  //      names += name;
  //      break;
  //    case VECTOR_2D:
  //      names += name+"x";
  //      names += name+"y";
  //      break;
  //    case VECTOR_3D:
  //      names += name+"x";
  //      names += name+"y";
  //      names += name+"z";
  //      break;
  //    case TENSOR_2D:
  //      names += name+"xx";
  //      names += name+"xy";
  //      names += name+"yx";
  //      names += name+"yy";
  //      break;
  //    case TENSOR_3D:
  //      names += name+"xx";
  //      names += name+"xy";
  //      names += name+"xz";
  //      names += name+"yx";
  //      names += name+"yy";
  //      names += name+"yz";
  //      names += name+"zx";
  //      names += name+"zy";
  //      names += name+"zz";
  //      break;
  //    default:
  //      break;
  //  }
  //  return names;
}

////////////////////////////////////////////////////////////////////////////////

void CField::config_tree()
{
  URI topology_uri;
  option("Topology").put_value(topology_uri);
  CRegion::Ptr topology = Core::instance().root().access_component(topology_uri).as_ptr<CRegion>();
  if ( is_null(topology) )
    throw CastingFailed (FromHere(), "Topology must be of a CRegion or derived type");
  m_topology->link_to(topology);
}

////////////////////////////////////////////////////////////////////////////////

void CField::set_topology(CRegion& region)
{
  m_topology->link_to(region.self());
  configure_option( "Topology", region.uri() );
}

////////////////////////////////////////////////////////////////////////////////

CField::~CField()
{
}

////////////////////////////////////////////////////////////////////////////////


void CField::create_data_storage()
{

  cf_assert( m_var_types.size()!=0 );
  cf_assert( is_not_null(m_topology->follow()) );


  // Check if there are coordinates in this field, and add to map
  m_coords = find_parent_component<CMesh>(topology()).nodes().coordinates().as_ptr<CTable<Real> >();

  Uint row_size(0);
  boost_foreach(const VarType var_size, m_var_types)
    row_size += Uint(var_size);

  m_data->set_row_size(row_size);

  switch (m_basis)
  {
    case Basis::POINT_BASED:
    {
      m_used_nodes = CElements::used_nodes(topology()).as_ptr<CList<Uint> >();
      m_data->resize(m_used_nodes->size());
      break;
    }
    case Basis::ELEMENT_BASED:
    {
      Uint data_size = 0;
      boost_foreach(CEntities& field_elements, find_components_recursively<CEntities>(topology()))
      {
        if (field_elements.exists_space(m_space_name) == false)
          throw ValueNotFound(FromHere(),"space \""+m_space_name+"\" does not exist in "+field_elements.uri().path());

        m_elements_start_idx[field_elements.as_ptr<CEntities>()] = data_size;
        CFieldView field_view("tmp_field_view");
        data_size = field_view.initialize(*this,field_elements.as_ptr<CEntities>());
      }
      m_data->resize(data_size);
      break;
    }
    case Basis::CELL_BASED:
    {
      Uint data_size = 0;
      boost_foreach(CEntities& field_elements, find_components_recursively<CCells>(topology()))
      {
        //CFinfo << name() << ": creating cellbased field storage in " << field_elements.uri().path() << CFendl;
        if (field_elements.exists_space(m_space_name) == false)
          throw ValueNotFound(FromHere(),"space \""+m_space_name+"\" does not exist in "+field_elements.uri().path());

        m_elements_start_idx[field_elements.as_ptr<CEntities>()] = data_size;
        CFieldView field_view("tmp_field_view");
        data_size = field_view.initialize(*this,field_elements.as_ptr<CEntities>());
      }
      m_data->resize(data_size);
      break;
    }
    case Basis::FACE_BASED:
    {
      Uint data_size = 0;
      boost_foreach(CEntities& field_elements, find_components_recursively_with_tag<CEntities>(topology(),Mesh::Tags::face_entity()))
      {
        if (field_elements.exists_space(m_space_name) == false)
          throw ValueNotFound(FromHere(),"space \""+m_space_name+"\" does not exist in "+field_elements.uri().path());

        m_elements_start_idx[field_elements.as_ptr<CEntities>()] = data_size;
        CFieldView field_view("tmp_field_view");
        data_size = field_view.initialize(*this,field_elements.as_ptr<CEntities>());
      }
      m_data->resize(data_size);
      break;
    }

    default:
      throw NotSupported(FromHere() , "Basis can only be ELEMENT_BASED or NODE_BASED");
      break;
  }
}

////////////////////////////////////////////////////////////////////////////////

bool CField::has_variable(const std::string& vname) const
{
  return std::find(m_var_names.begin(), m_var_names.end(), vname) != m_var_names.end();
}

////////////////////////////////////////////////////////////////////////////////

Uint CField::var_number ( const std::string& vname ) const
{
  const std::vector<std::string>::const_iterator var_loc_it = std::find(m_var_names.begin(), m_var_names.end(), vname);
  if(var_loc_it == m_var_names.end())
    throw Common::ValueNotFound(FromHere(), "Variable " + vname + " was not found in field " + name());
  return var_loc_it - m_var_names.begin();
}
//////////////////////////////////////////////////////////////////////////////

Uint CField::var_index ( const std::string& vname ) const
{
  const Uint var_nb = var_number(vname);
  return var_index(var_nb);
}

////////////////////////////////////////////////////////////////////////////////

Uint CField::var_index ( const Uint var_nb ) const
{
  Uint var_start = 0;
  for(Uint i = 0; i != var_nb; ++i)
    var_start += m_var_types[i];
  return var_start;
}

//////////////////////////////////////////////////////////////////////////////

CField::VarType CField::var_type ( const std::string& vname ) const
{
  return var_type(var_number(vname));
}

////////////////////////////////////////////////////////////////////////////////

const CRegion& CField::topology() const
{
  return *m_topology->follow()->as_ptr<CRegion>();
}

////////////////////////////////////////////////////////////////////////////////

CRegion& CField::topology()
{
  return *m_topology->follow()->as_ptr<CRegion>();
}

////////////////////////////////////////////////////////////////////////////////

const CList<Uint>& CField::used_nodes() const
{
  return *m_used_nodes;
}

//////////////////////////////////////////////////////////////////////////////

CTable<Real>::ConstRow CField::coords(const Uint idx) const
{
  return (*m_coords)[used_nodes()[idx]];
}

////////////////////////////////////////////////////////////////////////////////

boost::iterator_range< Common::ComponentIterator<CEntities const> > CField::field_elements() const
{
  std::vector<CEntities::ConstPtr> elements_vec;

  std::map<CEntities::ConstPtr,Uint>::const_iterator map_it = m_elements_start_idx.begin();
  std::map<CEntities::ConstPtr,Uint>::const_iterator map_end = m_elements_start_idx.end();
  for( ; map_it!=map_end; ++map_it)
  {
    elements_vec.push_back(map_it->first);
  }
  ComponentIterator<CEntities const> begin_iter(elements_vec,0);
  ComponentIterator<CEntities const> end_iter(elements_vec,elements_vec.size());
  return boost::make_iterator_range(begin_iter,end_iter);
}

////////////////////////////////////////////////////////////////////////////////

PECommPattern& CField::parallelize_with(PECommPattern& comm_pattern)
{
  cf_assert_desc("Only point-based fields supported now", m_basis == Basis::POINT_BASED);
  m_comm_pattern = comm_pattern.as_ptr<Common::PECommPattern>();
  comm_pattern.insert(name(),data().array(),true);
  return comm_pattern;
}

////////////////////////////////////////////////////////////////////////////////

PECommPattern& CField::parallelize()
{
  if ( is_not_null( m_comm_pattern ) ) // return if already parallel
    return *m_comm_pattern;

  // Extract gid from the nodes.glb_idx()  for only the nodes in the region the fields will use.
  const CList<Uint>& nodes = used_nodes();
  std::vector<Uint> gid;
  std::vector<Uint> rank;
  gid.reserve(nodes.size());
  rank.reserve(nodes.size());

  CMesh& mesh = find_parent_component<CMesh>(*this);
  boost_foreach (const Uint node, nodes.array())
  {
    cf_assert_desc(to_str(node)+">="+to_str(mesh.nodes().glb_idx().size()), node < mesh.nodes().glb_idx().size());
    cf_assert_desc(to_str(node)+">="+to_str(mesh.nodes().rank().size()), node < mesh.nodes().rank().size());
    gid.push_back(mesh.nodes().glb_idx()[node]);
    rank.push_back(mesh.nodes().rank()[node]);
  }

  // create the comm pattern and setup the pattern
  m_comm_pattern = mesh.create_component_ptr<PECommPattern>("comm_pattern_node_based");
  m_comm_pattern->insert("gid",gid,1,false);
  m_comm_pattern->setup(m_comm_pattern->get_child("gid").as_ptr<PEObjectWrapper>(),rank);

  return parallelize_with(*m_comm_pattern);
}

////////////////////////////////////////////////////////////////////////////////

void CField::synchronize()
{
  if ( is_not_null( m_comm_pattern ) )
    m_comm_pattern->synchronize( name() );
}

////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF
