// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_RDM_ElementLoop_hpp
#define CF_RDM_ElementLoop_hpp

#include <boost/mpl/for_each.hpp>

#include "Common/FindComponents.hpp"

#include "Mesh/CRegion.hpp"

#include "RDM/LibRDM.hpp"
#include "RDM/Tags.hpp"

namespace CF {
namespace RDM {

////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract RDM looping component
/// @author Tiago Quintino
class RDM_API ElementLoop : public Common::Component {

public: // typedefs

  /// provider
  typedef boost::shared_ptr< ElementLoop > Ptr;
  typedef boost::shared_ptr< ElementLoop const > ConstPtr;

public: // functions

  /// Contructor
  /// @param name of the component
  ElementLoop ( const std::string& name ) : Common::Component(name) {}

  /// Virtual destructor
  virtual ~ElementLoop() {}

  /// Get the class name
  static std::string type_name () { return "ElementLoop"; }

  /// execute the action
  virtual void execute () = 0;

  /// selects the region where to loop on
  void select_region( Mesh::CRegion::Ptr region ) { current_region = region; }

protected: // data

  /// region to loop on
  Mesh::CRegion::Ptr current_region;

}; // ElementLoop

////////////////////////////////////////////////////////////////////////////////////////////

} // RDM
} // CF

#endif // CF_RDM_ElementLoop_hpp
