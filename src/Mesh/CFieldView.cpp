// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/Log.hpp"

#include "Common/CBuilder.hpp"
#include "Common/FindComponents.hpp"
#include "Common/Foreach.hpp"
#include "Mesh/CFieldView.hpp"
#include "Mesh/CField.hpp"
#include "Mesh/CNodes.hpp"
#include "Mesh/CList.hpp"
#include "Mesh/CTable.hpp"
#include "Mesh/CEntities.hpp"
#include "Mesh/ElementType.hpp"
#include "Mesh/CSpace.hpp"
#include "Mesh/CFaceCellConnectivity.hpp"
#include "Mesh/CCells.hpp"
#include "Mesh/CMesh.hpp"
#include "Mesh/CMeshElements.hpp"

namespace CF {
namespace Mesh {

  using namespace CF::Common;

Common::ComponentBuilder < CFieldView, CFieldView, LibMesh >  CFieldView_Builder;
Common::ComponentBuilder < CScalarFieldView, CFieldView, LibMesh >  CScalarFieldView_Builder;
Common::ComponentBuilder < CMultiStateFieldView, CFieldView, LibMesh >  CMultiStateFieldView_Builder;

////////////////////////////////////////////////////////////////////////////////

CFieldView::CFieldView ( const std::string& name ) :
  Common::Component (name),
    m_start_idx(0),
    m_end_idx(0),
    m_stride(0),
    m_size(0)
{
}

////////////////////////////////////////////////////////////////////////////////

Uint CFieldView::initialize(CField& field, CEntities::Ptr elements)
{
  cf_assert(is_not_null(elements));
  set_field(field);
  set_elements(elements);
  return m_end_idx;
}

////////////////////////////////////////////////////////////////////////////////

CTable<Real>::Row CFieldView::operator[](const Uint idx)
{
  cf_assert_desc("Elements were not set in " + uri().path(), m_elements.expired() == false );
  cf_assert( idx < m_size );
  return m_field_data.lock()->array()[m_start_idx+idx];
}

////////////////////////////////////////////////////////////////////////////////

CTable<Real>::ConstRow CFieldView::operator[](const Uint idx) const
{
  cf_assert_desc("Elements were not set in " + uri().path(), m_elements.expired() == false );
  cf_assert( idx < m_size );
  return m_field_data.lock()->array()[m_start_idx+idx];
}

////////////////////////////////////////////////////////////////////////////////

bool CFieldView::set_elements(const CEntities& elements)
{
  cf_assert_desc("Field must be set before elements in "+uri().path(), m_field.expired() == false );
  const CField& field = *m_field.lock();
  m_elements = elements.as_const()->as_ptr<CEntities>();
  m_mesh_elements = find_parent_component<CMesh>(elements).elements().as_ptr_checked<CMeshElements>();
  if (field.exists_for_entities(elements))
  {
    m_space = elements.space(field.space_name()).as_ptr<CSpace>();
    m_stride = m_space.lock()->nb_states(); // this is the number of states per element (high order methods)
    m_start_idx = field.elements_start_idx(elements);
    m_end_idx = m_start_idx + m_stride * elements.size();
    m_size = m_end_idx - m_start_idx;
    m_coords_table = elements.nodes().coordinates().as_ptr<CTable<Real> >();
    return true;
  }
  else
  {
    m_size = 0;
    return false;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////

bool CFieldView::set_elements(CEntities::Ptr elements)
{
  cf_assert( is_not_null(m_field.lock()) );
  return set_elements(*elements);
}

////////////////////////////////////////////////////////////////////////////////

void CFieldView::set_field(CField& field)
{
  m_field = field.as_ptr<CField>();
  m_field_data = field.data().as_ptr<CTable<Real> >();
  cf_assert( is_not_null(m_field_data.lock()) );
}

////////////////////////////////////////////////////////////////////////////////

void CFieldView::set_field(CField::Ptr field)
{
  set_field(*field);
}

////////////////////////////////////////////////////////////////////////////////

void CFieldView::set_field(const CField& field)
{
  set_field(boost::const_pointer_cast<CField>(field.as_ptr<CField>()));
}

////////////////////////////////////////////////////////////////////////////////

Uint CFieldView::mesh_elements_idx(const Uint idx) const
{
  cf_assert_desc("Elements were not set in " + uri().path(), m_elements.expired() == false );
  return m_mesh_elements.lock()->unified_idx(*m_elements.lock(),idx);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CMultiStateFieldView::CMultiStateFieldView ( const std::string& name )
  : CFieldView (name)
{
}

////////////////////////////////////////////////////////////////////////////////

CMultiStateFieldView::View CMultiStateFieldView::operator[](const Uint idx)
{
  cf_assert_desc("Elements were not set in " + uri().path(), m_elements.expired() == false );
  cf_assert( idx < m_size );
  Uint data_idx = m_start_idx+m_stride*idx;
  Range range = Range().start(data_idx).finish(data_idx + m_stride);
  cf_assert( is_not_null(m_field_data.lock()) );
  return m_field_data.lock()->array()[ boost::indices[range][Range()] ];
}

////////////////////////////////////////////////////////////////////////////////

CMultiStateFieldView::ConstView CMultiStateFieldView::operator[](const Uint idx) const
{
  cf_assert_desc("Elements were not set in " + uri().path(), m_elements.expired() == false );
  cf_assert( idx < m_size );
  Uint data_idx = m_start_idx+m_stride*idx;
  Range range = Range().start(data_idx).finish(data_idx + m_stride);
  cf_assert( is_not_null(m_field_data.lock()) );
  return m_field_data.lock()->array()[ boost::indices[range][Range()] ];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CScalarFieldView::CScalarFieldView ( const std::string& name )
  : CFieldView (name)
{
}

////////////////////////////////////////////////////////////////////////////////

Real& CScalarFieldView::operator[](const Uint idx)
{
  cf_assert_desc("Elements were not set in " + uri().path(), m_elements.expired() == false );
  cf_assert( idx < m_size );
  Uint data_idx = m_start_idx+idx;
  cf_assert( is_not_null(m_field_data.lock()) );
  return m_field_data.lock()->array()[data_idx][0];
}

////////////////////////////////////////////////////////////////////////////////

const Real& CScalarFieldView::operator[](const Uint idx) const
{
  cf_assert_desc("Elements were not set", m_elements.expired() == false );
  cf_assert( idx < m_size );
  Uint data_idx = m_start_idx+idx;
  cf_assert( is_not_null(m_field_data.lock()) );
  return m_field_data.lock()->array()[data_idx][0];
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CConnectedFieldView::initialize(CField::Ptr field, boost::shared_ptr<CEntities> faces)
{
  set_field(field);
  set_elements(faces);
}

////////////////////////////////////////////////////////////////////////////////

void CConnectedFieldView::set_field(CField::Ptr field) { return set_field(*field); }

////////////////////////////////////////////////////////////////////////////////

void CConnectedFieldView::set_field(CField& field) { m_field = field.as_ptr<CField>(); }

////////////////////////////////////////////////////////////////////////////////

bool CConnectedFieldView::set_elements(boost::shared_ptr<CEntities> elements)
{
  cf_assert(is_not_null(elements));
  return set_elements(*elements);
  cf_assert( m_elements.expired() == false );
}

////////////////////////////////////////////////////////////////////////////////

bool CConnectedFieldView::set_elements(CEntities& elements)
{
  if (is_null(m_field.lock()))
    throw Common::SetupError(FromHere(),"set_field(field) must be called first for CConnectedFieldView "+uri().path());

  m_elements = elements.as_ptr_checked<CEntities>();
  m_face2cells = find_component_ptr<CFaceCellConnectivity>(elements);
  cf_assert_desc(elements.uri().path()+" needs to have a FaceCellConnectivity." , m_face2cells.expired() == false);
  m_views.resize(m_face2cells.lock()->lookup().components().size());
  boost_foreach(Component::Ptr cells, m_face2cells.lock()->used())
  {
    const Uint i = m_face2cells.lock()->lookup().location_comp_idx(*cells);
    cf_assert(is_not_null(cells));
    m_views[i] = Common::allocate_component<CFieldView>("view");
    m_views[i]->initialize(*m_field.lock(), cells->as_ptr<CElements>());
  }
  cf_assert(m_elements.expired() == false)
  return true;
}

////////////////////////////////////////////////////////////////////////////////

std::vector<CTable<Real>::Row> CConnectedFieldView::operator[](const Uint elem_idx)
{
  cf_assert_desc("Elements were not set", m_elements.expired() == false );
  cf_assert_desc(uri().path(),m_views.size());
  std::vector<CTable<Real>::Row> vec;
  boost_foreach(const Uint cell, m_face2cells.lock()->connectivity()[elem_idx])
  {
    boost::tie(cells_comp_idx,cell_idx) = m_face2cells.lock()->lookup().location_idx(cell);
    cf_assert(cells_comp_idx < m_views.size());
    cf_assert( is_not_null(m_views[cells_comp_idx]) );
    cf_assert(cell_idx < m_views[cells_comp_idx]->size());
    vec.push_back((*m_views[cells_comp_idx])[cell_idx]);
  }
  return vec;
}

////////////////////////////////////////////////////////////////////////////////

CTable<Real>::Row CConnectedFieldView::operator()(const Uint elem_idx, const Uint connected_idx)
{
  cf_assert_desc("Elements were not set", m_elements.expired() == false );
  cf_assert_desc(uri().path(),m_views.size());
  boost::tie(cells_comp_idx,cell_idx) = m_face2cells.lock()->lookup().location_idx(m_face2cells.lock()->connectivity()[elem_idx][connected_idx]);
  cf_assert(cells_comp_idx < m_views.size());
  cf_assert( is_not_null(m_views[cells_comp_idx]) );
  cf_assert(cell_idx < m_views[cells_comp_idx]->size());
  return (*m_views[cells_comp_idx])[cell_idx];
}

////////////////////////////////////////////////////////////////////////////////

Mesh::CField& CConnectedFieldView::field() { return *m_field.lock(); }

////////////////////////////////////////////////////////////////////////////////

CTable<Uint>::ConstRow CConnectedFieldView::connected_idx(const Uint idx) const
{
  return m_face2cells.lock()->connectivity()[idx];
}

////////////////////////////////////////////////////////////////////////////////

} // Mesh
} // CF
