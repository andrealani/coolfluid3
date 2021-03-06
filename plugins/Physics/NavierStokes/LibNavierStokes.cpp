// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/RegistLibrary.hpp"

#include "NavierStokes/LibNavierStokes.hpp"

namespace CF {
namespace Physics {
namespace NavierStokes {

  using namespace Common;

CF::Common::RegistLibrary<LibNavierStokes> LibNavierStokes;

////////////////////////////////////////////////////////////////////////////////

void LibNavierStokes::initiate_impl()
{
}

void LibNavierStokes::terminate_impl()
{
}

////////////////////////////////////////////////////////////////////////////////

} // NavierStokes
} // Physics
} // CF
