// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Common/CBuilder.hpp"

#include "SFDM/SF/LibSF.hpp"
#include "SFDM/SF/QuadFluxP3.hpp"
#include "SFDM/SF/LineFluxP3.hpp"

namespace CF {
namespace SFDM {
namespace SF {

////////////////////////////////////////////////////////////////////////////////

Common::ComponentBuilder < QuadFluxP3, Mesh::ShapeFunction, LibSF > QuadFluxP3_Builder;

////////////////////////////////////////////////////////////////////////////////

QuadFluxP3::QuadFluxP3(const std::string& name) : ShapeFunction(name)
{
  m_dimensionality = dimensionality;
  m_nb_nodes = nb_nodes;
  m_order = order;
  m_shape = shape;
  m_nb_lines_per_orientation = nb_lines_per_orientation;

  m_points.resize(boost::extents[nb_orientations][nb_lines_per_orientation][nb_nodes_per_line]);
  m_points[KSI][0][0] =  0;
  m_points[KSI][0][1] =  1;
  m_points[KSI][0][2] =  2;
  m_points[KSI][0][3] =  3;
  m_points[KSI][1][0] =  4;
  m_points[KSI][1][1] =  5;
  m_points[KSI][1][2] =  6;
  m_points[KSI][1][3] =  7;
  m_points[KSI][2][0] =  8;
  m_points[KSI][2][1] =  9;
  m_points[KSI][2][2] = 10;
  m_points[KSI][2][3] = 11;
  m_points[ETA][0][0] =  0;
  m_points[ETA][0][1] = 12;
  m_points[ETA][0][2] = 13;
  m_points[ETA][0][3] =  8;
  m_points[ETA][1][0] = 14;
  m_points[ETA][1][1] = 15;
  m_points[ETA][1][2] = 16;
  m_points[ETA][1][3] = 17;
  m_points[ETA][2][0] =  3;
  m_points[ETA][2][1] = 18;
  m_points[ETA][2][2] = 19;
  m_points[ETA][2][3] = 11;

  m_face_points.resize(boost::extents[nb_orientations][nb_lines_per_orientation][2]);
  m_face_points[KSI][0][LEFT ] =  0;
  m_face_points[KSI][0][RIGHT] =  3;
  m_face_points[KSI][1][LEFT ] =  4;
  m_face_points[KSI][1][RIGHT] =  7;
  m_face_points[KSI][2][LEFT ] =  8;
  m_face_points[KSI][2][RIGHT] = 11;
  m_face_points[ETA][0][LEFT ] =  0;
  m_face_points[ETA][0][RIGHT] =  8;
  m_face_points[ETA][1][LEFT ] = 14;
  m_face_points[ETA][1][RIGHT] = 17;
  m_face_points[ETA][2][LEFT ] =  3;
  m_face_points[ETA][2][RIGHT] = 11;

  m_face_number.resize(boost::extents[nb_orientations][2]);
  m_face_number[KSI][LEFT ]=3;
  m_face_number[KSI][RIGHT]=1;
  m_face_number[ETA][LEFT ]=0;
  m_face_number[ETA][RIGHT]=2;
}

////////////////////////////////////////////////////////////////////////////////

const ShapeFunction& QuadFluxP3::line() const
{
  return line_type();
}

////////////////////////////////////////////////////////////////////////////////

const LineFluxP3& QuadFluxP3::line_type()
{
  static const LineFluxP3 sf;
  return sf;
}

////////////////////////////////////////////////////////////////////////////////

void QuadFluxP3::compute_value(const MappedCoordsT& mapped_coord, ValueT& result)
{
  throw Common::NotImplemented(FromHere(),"This should never be called, as fluxes are only being computed using LineFluxP3 instead.");
}

////////////////////////////////////////////////////////////////////////////////

void QuadFluxP3::compute_gradient(const MappedCoordsT& mapped_coord, GradientT& result)
{
  throw Common::NotImplemented(FromHere(),"This should never be called, as fluxes are only being computed using LineFluxP3 instead.");
}

////////////////////////////////////////////////////////////////////////////////

RealMatrix QuadFluxP3::s_mapped_sf_nodes =  ( RealMatrix(20,2) <<
  -1.,          -1.,
  -1./sqrt(3.), -1.,
   1./sqrt(3.), -1.,
   1.,          -1.,
  -1.,           0.,
  -1./sqrt(3.),  0.,
   1./sqrt(3.),  0.,
   1.,           0.,
  -1.,           1.,
  -1./sqrt(3.),  1.,
   1./sqrt(3.),  1.,
   1.,           1.,
  -1.,          -1./sqrt(3.),
  -1.,           1./sqrt(3.),
   0.,          -1.,
   0.,          -1./sqrt(3.),
   0.,           1./sqrt(3.),
   0.,           1.,
   1.,          -1./sqrt(3.),
   1.,           1./sqrt(3.)
).finished();

////////////////////////////////////////////////////////////////////////////////

} // SF
} // SFDM
} // CF
