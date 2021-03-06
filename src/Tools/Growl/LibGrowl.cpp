// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/RegistLibrary.hpp"

#include "Tools/Growl/LibGrowl.hpp"

namespace CF {
namespace Tools {
namespace Growl {

CF::Common::RegistLibrary<LibGrowl> libGrowl;

////////////////////////////////////////////////////////////////////////////////

void LibGrowl::initiate_impl()
{
}

void LibGrowl::terminate_impl()
{
}

////////////////////////////////////////////////////////////////////////////////

} // Growl
} // Tools
} // CF
