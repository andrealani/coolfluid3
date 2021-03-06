// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/URI.hpp"
 

#include "Common/OptionArray.hpp"

#include "Solver/Actions/CLoop.hpp"

#include "Mesh/CRegion.hpp"

/////////////////////////////////////////////////////////////////////////////////////

using namespace CF::Common;
using namespace CF::Mesh;

namespace CF {
namespace Solver {
namespace Actions {

/////////////////////////////////////////////////////////////////////////////////////

CLoop::CLoop ( const std::string& name ) :
  Solver::Action(name)
{
  mark_basic();
}

/////////////////////////////////////////////////////////////////////////////////////

CLoopOperation& CLoop::create_loop_operation(const std::string action_provider)
{
  // The execuation of operations must be in chronological order,
  // hence they get an alphabetical name
  std::string name = action_provider;
  CLoopOperation::Ptr sub_operation =
    (build_component_abstract_type<CLoopOperation>(action_provider,name));
  add_component(sub_operation);
  return *sub_operation;
}

/////////////////////////////////////////////////////////////////////////////////////

const CLoopOperation& CLoop::action(const std::string& name) const
{
  return *get_child_ptr(name)->as_ptr<CLoopOperation>();
}

/////////////////////////////////////////////////////////////////////////////////////

CLoopOperation& CLoop::action(const std::string& name)
{
  return *get_child_ptr(name)->as_ptr<CLoopOperation>();
}

/////////////////////////////////////////////////////////////////////////////////////

} // Actions
} // Solver
} // CF

/////////////////////////////////////////////////////////////////////////////////////
