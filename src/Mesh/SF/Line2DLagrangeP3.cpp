// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <boost/assign/list_of.hpp>

#include "Common/CBuilder.hpp"

#include "LibSF.hpp"
#include "Line2DLagrangeP3.hpp"
#include "Point2DLagrangeP0.hpp"

namespace CF {
namespace Mesh {
namespace SF {

////////////////////////////////////////////////////////////////////////////////

Common::ComponentBuilder < Line2DLagrangeP3, 
                           ElementType, 
                           LibSF > aLine2DLagrangeP3_Builder;

////////////////////////////////////////////////////////////////////////////////

Line2DLagrangeP3::Line2DLagrangeP3(const std::string& name) : Line<DIM_2D,SFLineLagrangeP3>(name)
{
}

////////////////////////////////////////////////////////////////////////////////

Real Line2DLagrangeP3::compute_volume(const NodesT& coord) const
{
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

Real Line2DLagrangeP3::compute_area(const NodesT& coord) const
{
  return area(coord);
}

////////////////////////////////////////////////////////////////////////////////

void Line2DLagrangeP3::compute_normal(const NodesT& coord, RealVector& normal) const
{
  throw Common::NotImplemented( FromHere(), "" );
}

////////////////////////////////////////////////////////////////////////////////

bool Line2DLagrangeP3::is_coord_in_element(const RealVector& coord, const NodesT& nodes) const
{
  throw Common::NotImplemented( FromHere(), "" );
}

////////////////////////////////////////////////////////////////////////////////

void Line2DLagrangeP3::compute_centroid(const NodesT& coord , RealVector& centroid) const
{
  throw Common::NotImplemented( FromHere(), "" );
}

////////////////////////////////////////////////////////////////////////////////

const CF::Mesh::ElementType::FaceConnectivity& Line2DLagrangeP3::face_connectivity() const
{
  static FaceConnectivity connectivity;
  if(connectivity.face_first_nodes.empty())
  {
    connectivity.face_first_nodes = boost::assign::list_of(0);
    connectivity.face_node_counts.assign(1, 4); //1 row, 4 columns - indexes 0,1,2,3 of the nodes of the line
    connectivity.face_nodes = boost::assign::list_of(0)(1)(2)(3);
  }
  return connectivity;
}

////////////////////////////////////////////////////////////////////////////////

const CF::Mesh::ElementType& Line2DLagrangeP3::face_type(const CF::Uint face) const
{
  const static Point2DLagrangeP0 facetype;
  return facetype;
}

////////////////////////////////////////////////////////////////////////////////

void Line2DLagrangeP3::jacobian(const MappedCoordsT& mappedCoord, const NodeMatrixT& nodes, JacobianT& result)
{
  throw Common::NotImplemented( FromHere(), "" );
}

////////////////////////////////////////////////////////////////////////////////

void Line2DLagrangeP3::normal(const MappedCoordsT& mappedCoord, const NodeMatrixT& nodes, CoordsT& result)
{
  throw Common::NotImplemented( FromHere(), "" );
}

////////////////////////////////////////////////////////////////////////////////

Real Line2DLagrangeP3::volume(const NodeMatrixT& nodes)
{
  throw Common::NotImplemented( FromHere(), "" );

  return 0.;
}

////////////////////////////////////////////////////////////////////////////////

Real Line2DLagrangeP3::area(const NodeMatrixT& nodes)
{
  throw Common::NotImplemented( FromHere(), "" );

  return (nodes.row(1)-nodes.row(0)).norm();
}

////////////////////////////////////////////////////////////////////////////////

} // SF
} // Mesh
} // CF
