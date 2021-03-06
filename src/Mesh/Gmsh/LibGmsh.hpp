// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_LibGmsh_hpp
#define CF_LibGmsh_hpp

////////////////////////////////////////////////////////////////////////////////

#include "Common/CLibrary.hpp"

////////////////////////////////////////////////////////////////////////////////

/// Define the macro Gmsh_API
/// @note build system defines COOLFLUID_GMSH_EXPORTS when compiling Gmsh files
#ifdef COOLFLUID_GMSH_EXPORTS
#   define Gmsh_API      CF_EXPORT_API
#   define Gmsh_TEMPLATE
#else
#   define Gmsh_API      CF_IMPORT_API
#   define Gmsh_TEMPLATE CF_TEMPLATE_EXTERN
#endif

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {
  
/// @brief Library for I/O of the gmsh format
namespace Gmsh {

////////////////////////////////////////////////////////////////////////////////

/// Class defines the Gmshtral mesh format operations
/// @author Willem Deconinck
class Gmsh_API LibGmsh :
    public Common::CLibrary
{
public:

  typedef boost::shared_ptr<LibGmsh> Ptr;
  typedef boost::shared_ptr<LibGmsh const> ConstPtr;

  /// Constructor
  LibGmsh ( const std::string& name) : Common::CLibrary(name) {   }

  /// @return string of the library namespace
  static std::string library_namespace() { return "CF.Mesh.Gmsh"; }

  /// Static function that returns the library name.
  /// Must be implemented for CLibrary registration
  /// @return name of the library
  static std::string library_name() { return "Gmsh"; }

  /// Static function that returns the description of the library.
  /// Must be implemented for CLibrary registration
  /// @return description of the library

  static std::string library_description()
  {
    return "This library implements the Gmsh mesh format operations.";
  }

  /// Gets the Class name
  static std::string type_name() { return "LibGmsh"; }

protected:

  /// initiate library
  virtual void initiate_impl();

  /// terminate library
  virtual void terminate_impl();

}; // end LibGmsh

////////////////////////////////////////////////////////////////////////////////

} // Gmsh
} // Mesh
} // CF

////////////////////////////////////////////////////////////////////////////////

#endif // CF_LibGmsh_hpp
