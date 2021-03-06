// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <boost/assign/list_of.hpp>

#include "Common/Signal.hpp"
#include "Common/CBuilder.hpp"
#include "Common/OptionT.hpp"

#include "Common/XML/SignalOptions.hpp"

#include "Mesh/CMeshReader.hpp"
#include "Mesh/CDomain.hpp"
#include "Mesh/WriteMesh.hpp"

#include "Solver/CModelSteady.hpp"
#include "Solver/CSolver.hpp"
#include "RDM/Tags.hpp"

#include "RDM/RDSolver.hpp"
#include "RDM/IterativeSolver.hpp"
#include "RDM/TimeStepping.hpp"
#include "RDM/FwdEuler.hpp"
#include "RDM/SetupSingleSolution.hpp"
#include "RDM/Reset.hpp"

// supported physical models

#include "Physics/Scalar/Scalar2D.hpp"
#include "Physics/Scalar/ScalarSys2D.hpp"
#include "Physics/Scalar/Scalar3D.hpp"
#include "Physics/NavierStokes/NavierStokes2D.hpp"


#include "SteadyExplicit.hpp"

namespace CF {
namespace RDM {

using namespace CF::Common;
using namespace CF::Common::XML;
using namespace CF::Mesh;
using namespace CF::Physics;
using namespace CF::Solver;

Common::ComponentBuilder < SteadyExplicit, CF::Solver::CWizard, LibRDM > SteadyExplicit_Builder;

////////////////////////////////////////////////////////////////////////////////

SteadyExplicit::SteadyExplicit ( const std::string& name  ) :
  CF::Solver::CWizard ( name )
{
  // signals

  regist_signal( "create_model" )
    ->connect( boost::bind( &SteadyExplicit::signal_create_model, this, _1 ) )
    ->description("Creates a model for solving steady problms with RD using explicit iterations")
    ->pretty_name("Create Model");

  signal("create_component")->hidden(true);
  signal("rename_component")->hidden(true);
  signal("delete_component")->hidden(true);
  signal("move_component")->hidden(true);

  signal("create_model")->signature( boost::bind( &SteadyExplicit::signature_create_model, this, _1));
}


SteadyExplicit::~SteadyExplicit() {}


CModel& SteadyExplicit::create_model( const std::string& model_name, const std::string& physics_builder )
{
  // (1) create the model

  CModel& model = Common::Core::instance().root().create_component<CModelSteady>( model_name );

  // (2) create the domain

  CDomain& domain = model.create_domain( "Domain" );

  // (3) create the Physical Model

  PhysModel& pm = model.create_physics( physics_builder );

  pm.mark_basic();

  // (4) setup solver

  CF::RDM::RDSolver& solver = model.create_solver( "CF.RDM.RDSolver" ).as_type< CF::RDM::RDSolver >();

  solver.mark_basic();

  solver.time_stepping().configure_option_recursively( "maxiter",   1u);

  // (4a) setup iterative solver reset action

  Reset::Ptr reset  = allocate_component<Reset>("Reset");
  reset->configure_option( RDM::Tags::solver(), solver.uri() );
  solver.iterative_solver().pre_actions().append( reset );

  std::vector<std::string> reset_tags = boost::assign::list_of( RDM::Tags::residual() )
                                                              ( RDM::Tags::wave_speed() );
  reset->configure_option("FieldTags", reset_tags);

  // (4c) setup iterative solver explicit time stepping  - forward euler

  solver.iterative_solver().update()
      .append( allocate_component<FwdEuler>("Step") );

  // (4d) setup solver fields

  SetupSingleSolution::Ptr setup = allocate_component<SetupSingleSolution>("SetupFields");
  solver.prepare_mesh().append(setup);

  // (5) configure domain, physical model and solver in all subcomponents

  solver.configure_option_recursively( RDM::Tags::domain(),         domain.uri() );
  solver.configure_option_recursively( RDM::Tags::physical_model(), pm.uri() );
  solver.configure_option_recursively( RDM::Tags::solver(),         solver.uri() );

  return model;
}


void SteadyExplicit::signal_create_model ( Common::SignalArgs& node )
{
  SignalOptions options( node );

  std::string model_name  = options.value<std::string>("model_name");
  std::string phys  = options.value<std::string>("physical_model");

  create_model( model_name, phys );
}



void SteadyExplicit::signature_create_model( SignalArgs& node )
{
  SignalOptions options( node );

  options.add_option< OptionT<std::string> >("model_name", std::string() )
      ->description("Name for created model" )
      ->pretty_name("Model Name");

  std::vector<boost::any> models = boost::assign::list_of
      ( Scalar::Scalar2D::type_name() )
      ( Scalar::Scalar3D::type_name() )
      ( Scalar::ScalarSys2D::type_name() )
      ( NavierStokes::NavierStokes2D::type_name() ) ;

  options.add_option< OptionT<std::string> >("physical_model", std::string() )
      ->description("Name of the Physical Model")
      ->pretty_name("Physical Model Type")
      ->restricted_list() = models;
}

////////////////////////////////////////////////////////////////////////////////

} // RDM
} // CF
