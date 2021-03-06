// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.


#include "Common/FindComponents.hpp"
#include "Common/CBuilder.hpp"
#include "Common/OptionT.hpp"
#include "Common/CLink.hpp"

#include "Mesh/CSpace.hpp"
#include "Mesh/CElements.hpp"
#include "Mesh/ElementType.hpp"
#include "Mesh/CEntities.hpp"
#include "Mesh/CConnectivity.hpp"
#include "Mesh/FieldGroup.hpp"

namespace CF {
namespace Mesh {

using namespace Common;

Common::ComponentBuilder < CSpace, Component, LibMesh > CSpace_Builder;

////////////////////////////////////////////////////////////////////////////////

CSpace::CSpace ( const std::string& name ) :
  Component ( name ),
  m_is_proxy(false)
{
  mark_basic();

  m_properties["brief"] = std::string("Spaces are other views of CEntities, for instance a higher-order representation");
  m_properties["description"] = std::string("");

  m_options.add_option(OptionT<std::string>::create("shape_function", std::string("")))
      ->description("Shape Function defined in this space")
      ->pretty_name("Shape Function")
      ->attach_trigger(boost::bind(&CSpace::configure_shape_function, this))
      ->mark_basic();

  m_connectivity = create_static_component_ptr<CConnectivity>("connectivity");

  m_bound_fields = create_static_component_ptr<CLink>("bound_fields");
}

////////////////////////////////////////////////////////////////////////////////

CSpace::~CSpace()
{
}

////////////////////////////////////////////////////////////////////////////////

void CSpace::configure_shape_function()
{
  const std::string sf_name = option("shape_function").value<std::string>();
  if (is_not_null(m_shape_function))
  {
    remove_component(m_shape_function->name());
  }
  m_shape_function = build_component_abstract_type<ShapeFunction>( sf_name, sf_name );
  m_shape_function->rename(m_shape_function->derived_type_name());
  add_component( m_shape_function );
}

////////////////////////////////////////////////////////////////////////////////

bool CSpace::is_bound_to_fields() const
{
  return m_bound_fields->is_linked();
}

////////////////////////////////////////////////////////////////////////////////

FieldGroup& CSpace::bound_fields() const
{
  cf_assert(is_bound_to_fields());
  return m_bound_fields->follow()->as_type<FieldGroup>();
}

////////////////////////////////////////////////////////////////////////////////

CConnectivity::ConstRow CSpace::indexes_for_element(const Uint elem_idx) const
{
  if (m_is_proxy)
  {
    for (Uint i=0; i<m_connectivity_proxy.shape()[1]; ++i)
      m_connectivity_proxy[0][i] = m_elem_start_idx+i + elem_idx*m_connectivity_proxy.shape()[1];
    return m_connectivity_proxy[0];
  }
  else
  {
    return connectivity()[elem_idx];
  }
}

////////////////////////////////////////////////////////////////////////////////

void CSpace::make_proxy(const Uint elem_start_idx)
{
  m_is_proxy = true;
  m_elem_start_idx = elem_start_idx;
  m_connectivity_proxy.resize(boost::extents[1][nb_states()]);
}

////////////////////////////////////////////////////////////////////////////////


} // Mesh
} // CF
