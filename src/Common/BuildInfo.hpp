// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Common_BuildInfo_hpp
#define CF_Common_BuildInfo_hpp

#include "Common/CF.hpp"
#include "Common/CommonAPI.hpp"

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Common {

////////////////////////////////////////////////////////////////////////////////

struct Common_API BuildInfo : public boost::noncopyable
{

  /// Return the subversion version string of this build
  std::string version_header() const;
  /// Return the subversion version string of this build
  std::string svn_version() const;
  /// Return the CF version string
  std::string release_version() const;
  /// Return the CF Kernel version string
  std::string kernel_version() const;
  /// Return the CF build type
  std::string build_type() const;
  /// Return the CMake version
  std::string build_tool() const;
  /// Return the build processor
  std::string build_processor() const;
  /// OS short name. Examples: "Linux" or "Windows"
  /// @return string with short OS name
  std::string os_name() const;
  /// OS short name. Examples: "Linux-2.6.23" or "Windows 5.1"
  /// @return string with long OS name
  std::string os_long_name() const;
  /// OS version. Examples: "2.6.23" or "5.1"
  /// @return string with OS version
  std::string os_version() const;
  /// OS bits. Examples: "32" or "64"
  /// @post should be equal to 8 * size_of(void*) but it given by the build system
  /// @return string with OS addressing size
  std::string os_bits() const;

}; // BuildInfo

////////////////////////////////////////////////////////////////////////////////

} // Common
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_Common_Core_hpp
