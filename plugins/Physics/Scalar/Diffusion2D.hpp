// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_Physics_Scalar_Diffusion2D_hpp
#define CF_Physics_Scalar_Diffusion2D_hpp

#include "Common/StringConversion.hpp"
#include "Math/Defs.hpp"

#include "Physics/Variables.hpp"

#include "Scalar2D.hpp"

namespace CF {
namespace Physics {
namespace Scalar {

///////////////////////////////////////////////////////////////////////////////////////

class Scalar_API Diffusion2D : public VariablesT<Diffusion2D> {

public: //typedefs

  typedef Scalar2D     MODEL;

  enum { U = 0 };

  typedef boost::shared_ptr<Diffusion2D> Ptr;
  typedef boost::shared_ptr<Diffusion2D const> ConstPtr;

public: // functions

  /// constructor
  /// @param name of the component
  Diffusion2D ( const std::string& name );

  /// virtual destructor
  virtual ~Diffusion2D();

  /// Get the class name
  static std::string type_name () { return "Diffusion2D"; }

  /// compute physical properties
  template < typename CV, typename SV, typename GM >
  static void compute_properties ( const CV& coord,
                                   const SV& sol,
                                   const GM& grad_vars,
                                   MODEL::Properties& p )
  {
    p.coords    = coord;       // cache the coordiantes locally
    p.vars      = sol;         // cache the variables locally
    p.grad_vars = grad_vars;   // cache the gradient of variables locally

    p.v[XX] = 1.0; // constant vx
    p.v[YY] = 1.0; // constant vy

    p.u = sol[U];

    p.mu = 1.0;     // no diffusion
  }

  /// compute the physical flux
  template < typename FM >
  static void flux( const MODEL::Properties& p,
                    FM& flux)
  {
#if 0 // compilation error
    flux(0,XX)   = p.mu * p.grad_vars.col(XX);
    flux(0,YY)   = p.mu * p.grad_vars.col(YY);
#endif
  }

  /// compute the eigen values of the flux jacobians
  template < typename GV, typename EV >
  static void flux_jacobian_eigen_values(const MODEL::Properties& p,
                                         const GV& direction,
                                         EV& Dv)
  {
//    Dv[0]   = p.v[XX] * direction[XX] + p.v[YY] * direction[YY];
  }

  /// compute the eigen values of the flux jacobians
  template < typename GV, typename EV, typename OP >
  static void flux_jacobian_eigen_values(const MODEL::Properties& p,
                                         const GV& direction,
                                         EV& Dv,
                                         OP& op )

  {
//    Dv[0]   = op( p.v[XX] * direction[XX] + p.v[YY] * direction[YY] );
  }

  /// decompose the eigen structure of the flux jacobians projected on the gradients
  template < typename GV, typename EM, typename EV >
  static void flux_jacobian_eigen_structure(const MODEL::Properties& p,
                                            const GV& direction,
                                            EM& Rv,
                                            EM& Lv,
                                            EV& Dv)
  {
//    Rv(0,0) = 1.;
//    Lv(0,0) = 1.;
//    Dv[0]   = p.v[XX] * direction[XX] + p.v[YY] * direction[YY];
  }

  /// compute the PDE residual
  template < typename JM, typename RV >
  static void residual(const MODEL::Properties& p,
                       JM         flux_jacob[],
                       RV&        res)
  {
//    JM& A = flux_jacob[XX];
//    JM& B = flux_jacob[YY];
//
//    A(0,0) = p.v[XX];
//    B(0,0) = p.v[YY];
//
//    res = A * p.grad_vars.col(XX) + B * p.grad_vars.col(YY);
  }

}; // Diffusion2D

////////////////////////////////////////////////////////////////////////////////////

} // Scalar
} // Physics
} // CF

#endif // CF_Physics_Scalar_Diffusion2D_hpp
