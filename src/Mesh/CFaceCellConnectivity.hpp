// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Mesh_CFaceCellConnectivity_hpp
#define CF_Mesh_CFaceCellConnectivity_hpp

//#include "Mesh/CElements.hpp"
#include "Mesh/CUnifiedData.hpp"
#include "Mesh/CTable.hpp"
#include "Mesh/CMeshElements.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {
  class CLink;
}
namespace Mesh {
  
  class CNodes;
  class CRegion;
  class CCells;

////////////////////////////////////////////////////////////////////////////////

/// Stores connectivity data between faces and their adjacent elements
/// and provides a convenient API to access the data
/// @author Andrea Lani
/// @author Willem Deconinck
class Mesh_API CFaceCellConnectivity : public Common::Component
{
public:

  typedef boost::shared_ptr<CFaceCellConnectivity> Ptr;
  typedef boost::shared_ptr<CFaceCellConnectivity const> ConstPtr;

  /// Contructor
  /// @param name of the component
  CFaceCellConnectivity ( const std::string& name );

  /// Virtual destructor
  virtual ~CFaceCellConnectivity() {}

  /// Get the class name
  static std::string type_name () { return "CFaceCellConnectivity"; }

  /// setup the node to element connectivity
  /// This function calls 
  /// - set_nodes(nodes)
  /// - set_elements(elements_range)
  /// - build_connectivity
  /// They could be called seperately if wanted
  /// @post all access functions can be used after setup
  /// @param [in] nodes the nodes component to find connected elements of
  /// @param [in] elements_range the elements range to see if they are connected to the nodes.
  ///                            Can be made using "find_components_recursively<CElements>()" function
  void setup(CRegion& region);
  
  /// Build the connectivity table
  /// Build the connectivity table as a CDynTable<Uint>
  /// @pre set_nodes() and set_elements() must have been called
  
  void build_connectivity();
  
  /// const access to the node to element connectivity table in unified indices
  CTable<Uint>& connectivity() { return *m_connectivity; }

  const CTable<Uint>& connectivity() const { return *m_connectivity; }

  /// access to see if the face is a bdry face
  CList<Uint>& is_bdry_face() { cf_assert( is_not_null(m_is_bdry_face) ); return *m_is_bdry_face; }

  const CList<Uint>& is_bdry_face() const { cf_assert( is_not_null(m_is_bdry_face) ); return *m_is_bdry_face; }
  
  CList<Uint>& face_number() { cf_assert( is_not_null(m_face_nb_in_first_elem) ); return *m_face_nb_in_first_elem; }

  const CList<Uint>& face_number() const { cf_assert( is_not_null(m_face_nb_in_first_elem) ); return *m_face_nb_in_first_elem; }
  
  Uint size() const { return connectivity().size(); }
  
  std::vector<Uint> nodes(const Uint face) const;

  CMeshElements& lookup() { cf_assert_desc("Must build connectivity first", is_not_null(m_mesh_elements)); return *m_mesh_elements; }

  const CMeshElements& lookup() const { cf_assert_desc("Must build connectivity first", is_not_null(m_mesh_elements)); return *m_mesh_elements; }

  std::vector<Component::Ptr> used();
  
  void add_used (const Component& used_comp);

private: // data

  /// nb_faces
  Uint m_nb_faces;

  /// unified view of the elements
  boost::shared_ptr<Common::CGroup> m_used_components;

  /// Actual connectivity table
  CTable<Uint>::Ptr m_connectivity;
  
  CList<Uint>::Ptr m_face_nb_in_first_elem;
  
  // @todo make a CList<bool> (some bug prevents using CList<bool>::Buffer with CList<bool> )
  CList<Uint>::Ptr m_is_bdry_face;
  
  CMeshElements::Ptr m_mesh_elements;

}; // CFaceCellConnectivity

////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Mesh_ConnectivityData_hpp
