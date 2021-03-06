// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_RDM_SchemeBase_hpp
#define CF_RDM_SchemeBase_hpp

#include <functional>

#include <boost/assign.hpp>

#include <Eigen/Dense>

#include "Common/Core.hpp"
#include "Common/OptionT.hpp"
#include "Common/OptionComponent.hpp"
#include "Common/BasicExceptions.hpp"

#include "Math/MatrixTypes.hpp"

#include "Mesh/ElementData.hpp"
#include "Mesh/CField.hpp"
#include "Mesh/CFieldView.hpp"
#include "Mesh/CNodes.hpp"
#include "Mesh/ElementType.hpp"

#include "Physics/PhysModel.hpp"

#include "Solver/Actions/CLoopOperation.hpp"

#include "RDM/LibRDM.hpp"
#include "RDM/CellLoop.hpp"
#include "RDM/Tags.hpp"

namespace CF {
namespace RDM {

////////////////////////////////////////////////////////////////////////////////////////////

/// Base class for RD schemes
/// Templatized with the shape function, the quadrature rule and
/// the physical variables
/// @author Tiago Quintino
template < typename SF, typename QD, typename PHYS >
class RDM_API SchemeBase : public Solver::Actions::CLoopOperation {

public: // typedefs

  /// pointers
  typedef boost::shared_ptr< SchemeBase > Ptr;
  typedef boost::shared_ptr< SchemeBase const> ConstPtr;

public: // functions

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW  ///< storing fixed-sized Eigen structures

  /// Contructor
  /// @param name of the component
  SchemeBase ( const std::string& name );

  /// Virtual destructor
  virtual ~SchemeBase() {}

  /// Get the class name
  static std::string type_name () { return "SchemeBase<" + SF::type_name() + ">"; }

  /// interpolates the shape functions and gradient values
  /// @post zeros the local residual matrix
  void interpolate ( const Mesh::CTable<Uint>::ConstRow& nodes_idx );

  void sol_gradients_at_qdpoint(const Uint q);

protected: // helper functions

  void change_elements()
  {
    connectivity_table =
        elements().as_ptr<Mesh::CElements>()->node_connectivity().as_ptr< Mesh::CTable<Uint> >();
    coordinates =
        elements().nodes().coordinates().as_ptr< Mesh::CTable<Real> >();

    cf_assert( is_not_null(connectivity_table) );
    cf_assert( is_not_null(coordinates) );

    solution   = csolution.lock()->data_ptr();
    residual   = cresidual.lock()->data_ptr();
    wave_speed = cwave_speed.lock()->data_ptr();
  }

protected: // typedefs

  typedef typename SF::NodeMatrixT                                               NodeMT;

  typedef Eigen::Matrix<Real, QD::nb_points, 1u>                                 WeightVT;

  typedef Eigen::Matrix<Real, QD::nb_points, PHYS::MODEL::_neqs>                 ResidualMT;

  typedef Eigen::Matrix<Real, PHYS::MODEL::_neqs, PHYS::MODEL::_neqs >           EigenValueMT;

  typedef Eigen::Matrix<Real, PHYS::MODEL::_neqs, PHYS::MODEL::_neqs>            PhysicsMT;
  typedef Eigen::Matrix<Real, PHYS::MODEL::_neqs, 1u>                            PhysicsVT;

  typedef Eigen::Matrix<Real, SF::nb_nodes,   PHYS::MODEL::_neqs>                SolutionMT;
  typedef Eigen::Matrix<Real, 1u, PHYS::MODEL::_neqs >                           SolutionVT;

  typedef Eigen::Matrix<Real, SF::nb_nodes, SF::nb_nodes>                        MassMT;
  typedef Eigen::Matrix<Real, QD::nb_points, SF::nb_nodes>                       SFMatrixT;
  typedef Eigen::Matrix<Real, 1u, SF::nb_nodes >                                 SFVectorT;

  typedef Eigen::Matrix<Real, PHYS::MODEL::_ndim, 1u>                            DimVT;

  typedef Eigen::Matrix<Real, PHYS::MODEL::_ndim, PHYS::MODEL::_ndim>            JMT;

  typedef Eigen::Matrix<Real, QD::nb_points, PHYS::MODEL::_ndim>                 QCoordMT;
  typedef Eigen::Matrix<Real, QD::nb_points, PHYS::MODEL::_neqs>                 QSolutionMT;

  typedef Eigen::Matrix<Real, PHYS::MODEL::_neqs, PHYS::MODEL::_ndim>            QSolutionVT;

protected: // data

  boost::weak_ptr< Mesh::CField > csolution;   ///< solution field
  boost::weak_ptr< Mesh::CField > cresidual;   ///< residual field
  boost::weak_ptr< Mesh::CField > cwave_speed; ///< wave_speed field

  /// pointer to connectivity table, may reset when iterating over element types
  Mesh::CTable<Uint>::Ptr connectivity_table;
  /// pointer to nodes coordinates, may reset when iterating over element types
  Mesh::CTable<Real>::Ptr coordinates;
  /// pointer to solution table, may reset when iterating over element types
  Mesh::CTable<Real>::Ptr solution;
  /// pointer to solution table, may reset when iterating over element types
  Mesh::CTable<Real>::Ptr residual;
  /// pointer to solution table, may reset when iterating over element types
  Mesh::CTable<Real>::Ptr wave_speed;

  /// helper object to compute the quadrature information
  const QD& m_quadrature;

  /// coordinates of quadrature points in physical space
  QCoordMT X_q;
  /// stores dX/dksi and dx/deta at each quadrature point, one matrix per dimension
  QCoordMT dX[PHYS::MODEL::_ndim];

  /// solution at quadrature points in physical space
  QSolutionMT U_q;
  /// derivatives of solution to X at each quadrature point, one matrix per dimension
  QSolutionMT dUdX[PHYS::MODEL::_ndim];

  /// derivatives of solution to X at ONE quadrature point, each column stores derivatives
  /// with respect to given coordinate
  QSolutionVT dUdXq;

  /// contribution to nodal residuals
  SolutionMT Phi_n;
  /// node values
  NodeMT     X_n;
  /// Values of the solution located in the dof of the element
  SolutionMT U_n;
  /// Values of the operator L(u) computed in quadrature points.
  PhysicsVT  LU;
  /// flux jacobians
  PhysicsMT  dFdU[PHYS::MODEL::_ndim];
  /// interporlation matrix - values of shapefunction at each quadrature point
  SFMatrixT  Ni;
  /// derivative matrix - values of shapefunction derivative in Ksi at each quadrature point
  SFMatrixT  dNdKSI[PHYS::MODEL::_ndim];
  /// derivatives of shape functions on physical space at all quadrature points,
  /// one matrix per dimenison
  SFMatrixT  dNdX[PHYS::MODEL::_ndim];
  /// jacobian of transformation at each quadrature point
  WeightVT jacob;
  /// Integration factor (jacobian multiplied by quadrature weight)
  WeightVT wj;
  /// temporary local gradient of 1 shape function
  DimVT dN;

  typename PHYS::MODEL::Properties phys_props; ///< physical properties

protected:

  /// temporary local gradient of 1 shape function in reference and physical space
  DimVT dNphys;
  DimVT dNref;
  /// Jacobi matrix at each quadrature point
  JMT JM;
  /// Inverse of the Jacobi matrix at each quadrature point
  JMT JMinv;

};

////////////////////////////////////////////////////////////////////////////////////////////

template<typename SF, typename QD, typename PHYS>
SchemeBase<SF,QD,PHYS>::SchemeBase ( const std::string& name ) :
  CLoopOperation(name),
  m_quadrature( QD::instance() )
{
  regist_typeinfo(this); // template class so must force type registration @ construction

  // options

  m_options.add_option(
        Common::OptionComponent<Mesh::CField>::create( RDM::Tags::solution(), &csolution));
  m_options.add_option(
        Common::OptionComponent<Mesh::CField>::create( RDM::Tags::wave_speed(), &cwave_speed));
  m_options.add_option(
        Common::OptionComponent<Mesh::CField>::create( RDM::Tags::residual(), &cresidual));


  m_options["Elements"]
      .attach_trigger ( boost::bind ( &SchemeBase<SF,QD,PHYS>::change_elements, this ) );

  // initializations

  for(Uint d = 0; d < PHYS::MODEL::_ndim; ++d)
    dFdU[d].setZero();

  // Gradient of the shape functions in reference space
  typename SF::MappedGradientT GradSF;
  // Values of shape functions in reference space
  typename SF::ShapeFunctionsT ValueSF;

  // initialize the interpolation matrix

  for(Uint q = 0; q < QD::nb_points; ++q)
  {
    // compute values and gradients of all functions in this quadrature point

    SF::shape_function_gradient( m_quadrature.coords.col(q), GradSF  );
    SF::shape_function_value   ( m_quadrature.coords.col(q), ValueSF );

    // copy the values to interpolation matrix

    Ni.row(q) = ValueSF.transpose();

    // copy the gradients

    for(Uint d = 0; d < PHYS::MODEL::_ndim; ++d)
      dNdKSI[d].row(q) = GradSF.row(d);
  }

  // debug

  //  std::cout << "QD points [" << QD::nb_points << "]"  << std::endl;
  //  std::cout << "Ki_qn   is " << Ki_n[0].rows()<< "x" << Ki_n[0].cols() << std::endl;
  //  std::cout << "LU      is " << LU.rows() << "x" << LU.cols()  << std::endl;
  //  std::cout << "Phi_n   is " << Phi_n.rows()   << "x" << Phi_n.cols()    << std::endl;
  //  std::cout << "U_n     is " << U_n.rows()     << "x" << U_n.cols()      << std::endl;
  //  std::cout << "DvPlus  is " << DvPlus[0].rows()  << "x" << DvPlus[0].cols()   << std::endl;

}


template<typename SF,typename QD, typename PHYS>
void SchemeBase<SF, QD,PHYS>::interpolate( const Mesh::CTable<Uint>::ConstRow& nodes_idx )
{
  /// @todo must be tested for 3D

  // copy the coordinates from the large array to a small

  Mesh::fill(X_n, *coordinates, nodes_idx );

  // copy the solution from the large array to a small

  for(Uint n = 0; n < SF::nb_nodes; ++n)
    for (Uint v=0; v < PHYS::MODEL::_neqs; ++v)
      U_n(n,v) = (*solution)[ nodes_idx[n] ][v];

  // coordinates of quadrature points in physical space

  X_q  = Ni * X_n;

  // solution at all quadrature points in physical space

  U_q = Ni * U_n;

  // Jacobian of transformation phys -> ref:
  //    |   dx/dksi    dx/deta    |
  //    |   dy/dksi    dy/deta    |

  // dX[XX].col(KSI) has the values of dx/dksi at all quadrature points
  // dX[XX].col(ETA) has the values of dx/deta at all quadrature points

  for(Uint dimx = 0; dimx < PHYS::MODEL::_ndim; ++dimx)
    for(Uint dimksi = 0; dimksi < PHYS::MODEL::_ndim; ++dimksi)
    {
      dX[dimx].col(dimksi) = dNdKSI[dimksi] * X_n.col(dimx);
    }

  // Fill Jacobi matrix (matrix of transformation phys. space -> ref. space) at qd. point q

  for(Uint q = 0; q < QD::nb_points; ++q)
  {

    for(Uint dimx = 0; dimx < PHYS::MODEL::_ndim; ++dimx)
      for(Uint dimksi = 0; dimksi < PHYS::MODEL::_ndim; ++dimksi)
        JM(dimksi,dimx) = dX[dimx](q,dimksi);

    // Once the jacobi matrix at one quadrature point is assembled, let's re-use it
    // to compute the gradients of of all shape functions in phys. space
    jacob[q] = JM.determinant();
    JMinv = JM.inverse();

    for(Uint n = 0; n < SF::nb_nodes; ++n)
    {

      for(Uint dimksi = 0; dimksi < PHYS::MODEL::_ndim; ++ dimksi)
        dNref[dimksi] = dNdKSI[dimksi](q,n);

      dNphys = JMinv * dNref;

      for(Uint dimx = 0; dimx < PHYS::MODEL::_ndim; ++ dimx)
        dNdX[dimx](q,n) = dNphys[dimx];

    }

  } // loop over quadrature points

  // compute transformed integration weights (sum is element area)

  for(Uint q = 0; q < QD::nb_points; ++q)
    wj[q] = jacob[q] * m_quadrature.weights[q];

  // solution derivatives in physical space at quadrature point

  for(Uint dim = 0; dim < PHYS::MODEL::_ndim; ++dim)
    dUdX[dim] = dNdX[dim] * U_n;

  // zero element residuals

  Phi_n.setZero();
}


template<typename SF,typename QD, typename PHYS>
void SchemeBase<SF, QD,PHYS>::sol_gradients_at_qdpoint(const Uint q)
{

  for(Uint dim = 0; dim < PHYS::MODEL::_ndim; ++dim)
    dUdXq.col(dim) = dUdX[dim].row(q).transpose();

}

////////////////////////////////////////////////////////////////////////////////////////////

} // RDM
} // CF

#endif // CF_RDM_SchemeBase_hpp
