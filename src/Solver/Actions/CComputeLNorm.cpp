// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <cmath>

#include "Common/MPI/PE.hpp"

#include "Common/CBuilder.hpp"
#include "Common/OptionT.hpp"
#include "Common/OptionComponent.hpp"
#include "Common/Foreach.hpp"

#include "Mesh/CField.hpp"
#include "Mesh/CTable.hpp"

#include "Solver/Actions/CComputeLNorm.hpp"


using namespace CF::Common;
using namespace CF::Mesh;

namespace CF {
namespace Solver {
namespace Actions {

////////////////////////////////////////////////////////////////////////////////////////////

void compute_L2( CTable<Real>::ArrayT& array, Real& norm )
{
  const int size = 1; // sum 1 value in each processor

  Real loc_norm = 0.; // norm on local processor
  Real glb_norm = 0.; // norm summed over all processors

  boost_foreach(CTable<Real>::ConstRow row, array )
      loc_norm += row[0]*row[0];

  mpi::PE::instance().all_reduce( mpi::plus(), &loc_norm, size, &glb_norm );

  norm = std::sqrt(glb_norm);
}

void compute_L1( CTable<Real>::ArrayT& array, Real& norm )
{
  const int size = 1; // sum 1 value in each processor

  Real loc_norm = 0.; // norm on local processor
  Real glb_norm = 0.; // norm summed over all processors

  boost_foreach(CTable<Real>::ConstRow row, array )
      loc_norm += std::abs( row[0] );

  mpi::PE::instance().all_reduce( mpi::plus(), &loc_norm, size, &glb_norm );
}

void compute_Linf( CTable<Real>::ArrayT& array, Real& norm )
{
  const int size = 1; // sum 1 value in each processor

  Real loc_norm = 0.; // norm on local processor
  Real glb_norm = 0.; // norm summed over all processors

  boost_foreach(CTable<Real>::ConstRow row, array )
      loc_norm = std::max( std::abs(row[0]), loc_norm );

  mpi::PE::instance().all_reduce( mpi::max(), &loc_norm, size, &glb_norm );

  norm = glb_norm;
}

void compute_Lp( CTable<Real>::ArrayT& array, Real& norm, Uint order )
{
  const int size = 1; // sum 1 value in each processor

  Real loc_norm = 0.; // norm on local processor
  Real glb_norm = 0.; // norm summed over all processors

  boost_foreach(CTable<Real>::ConstRow row, array )
    loc_norm += std::pow( std::abs(row[0]), (int)order ) ;

  mpi::PE::instance().all_reduce( mpi::plus(), &loc_norm, size, &glb_norm );

  norm = std::pow(glb_norm, 1./order );
}

////////////////////////////////////////////////////////////////////////////////////////////

Common::ComponentBuilder < CComputeLNorm, CAction, LibActions > CComputeLNorm_Builder;

////////////////////////////////////////////////////////////////////////////////////////////

CComputeLNorm::CComputeLNorm ( const std::string& name ) : CAction(name)
{
  mark_basic();

  // properties

  m_properties.add_property("Norm", Real(0.) );

  // options

  m_options.add_option< OptionT<bool> >("Scale", true)
      ->description("Scales (divides) the norm by the number of entries (ignored if order zero)");

  m_options.add_option< OptionT<Uint> >("Order", 2u)
      ->description("Order of the p-norm, zero if L-inf");

  m_options.add_option(OptionComponent<CField>::create("Field", &m_field))
      ->description("Field for which to compute the norm");
}


void CComputeLNorm::execute()
{
  if ( m_field.expired() ) 	throw SetupError(FromHere(), "Field was not set");

  CTable<Real>& table = m_field.lock()->data();
  CTable<Real>::ArrayT& array =  table.array();

  const Uint nbrows = table.size();

  if ( !nbrows ) throw SetupError(FromHere(), "Field has empty table");

  Real norm = 0.;

  const Uint order = m_options.option("Order").value<Uint>();

  // sum of all processors

  switch(order) {

  case 2:  compute_L2( array, norm );    break;

  case 1:  compute_L1( array, norm );    break;

  case 0:  compute_Linf( array, norm );  break; // consider order 0 as Linf

  default: compute_Lp( array, norm, order );    break;

  }


  if( m_options.option("Scale").value<bool>() && order )
    norm /= nbrows;

  configure_property("Norm", norm);
}

////////////////////////////////////////////////////////////////////////////////

} // Actions
} // Solver
} // CF
