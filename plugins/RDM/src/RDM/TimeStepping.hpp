// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_RDM_TimeStepping_hpp
#define CF_RDM_TimeStepping_hpp

#include "Solver/ActionDirector.hpp"

#include "RDM/LibRDM.hpp"

namespace CF {

  namespace Solver { class CTime; }

namespace RDM {


/////////////////////////////////////////////////////////////////////////////////////

class RDM_API TimeStepping : public CF::Solver::ActionDirector {

public: // typedefs

  typedef boost::shared_ptr<TimeStepping> Ptr;
  typedef boost::shared_ptr<TimeStepping const> ConstPtr;

public: // functions
  /// Contructor
  /// @param name of the component
  TimeStepping ( const std::string& name );

  /// Virtual destructor
  virtual ~TimeStepping() {}

  /// Get the class name
  static std::string type_name () { return "TimeStepping"; }

  /// execute the action
  virtual void execute ();

  Common::CActionDirector& pre_actions()  { return *m_pre_actions; }
  Common::CActionDirector& post_actions() { return *m_post_actions; }

  CF::Solver::CTime&       time()         { return *m_time; }

  /// @name SIGNALS
  //@{

  //@} END SIGNALS

private: // functions

  /// @returns true if any of the stop criteria is achieved
  bool stop_condition();
  /// raises event when timestep is done
  void raise_timestep_done();

private: // data

  boost::shared_ptr< Solver::CTime > m_time;   ///< component tracking time

  Common::CActionDirector::Ptr m_pre_actions;  ///< set of actions before non-linear solve

  Common::CActionDirector::Ptr m_post_actions; ///< set of actions after non-linear solve

};

/////////////////////////////////////////////////////////////////////////////////////


} // RDM
} // CF

#endif // CF_RDM_TimeStepping_hpp
