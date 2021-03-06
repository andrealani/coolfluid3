// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "Mesh/Tecplot/Shared.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {
namespace Tecplot {

//////////////////////////////////////////////////////////////////////////////

const Uint Shared::m_nodes_in_tp_elem[nb_tp_types] = { 0,  2,  3,  4,  4,  8,  6,  5,  3,  6,  9,
                                                              10, 27, 18, 14,  1,  8, 20, 15, 13,  9,
                                                              10, 12, 15, 15, 21,  4,  5,  6, 20, 35,
                                                              56, 34, 52,  0,  0, 16 };

const Uint Shared::m_tp_elem_dim[nb_tp_types] = { DIM_0D, DIM_1D, DIM_2D, DIM_2D, DIM_3D, DIM_3D, DIM_3D, DIM_3D, DIM_1D, DIM_2D,
                                                      DIM_2D, DIM_3D, DIM_3D, DIM_3D, DIM_3D, DIM_1D, DIM_2D, DIM_2D, DIM_3D, DIM_3D,
                                                      DIM_2D, DIM_2D, DIM_2D, DIM_2D, DIM_2D, DIM_2D, DIM_1D, DIM_1D, DIM_1D, DIM_3D,
                                                      DIM_3D, DIM_3D, DIM_3D, DIM_3D, DIM_2D, DIM_3D, DIM_2D };

const Uint Shared::m_tp_elem_order[nb_tp_types] = {  0, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2,
                                                     2, 2, 2, 2, 1, 2, 2, 2, 2, 3,
                                                     3, 4, 4, 5, 5, 3, 4, 5, 3, 4,
                                                     5, 4, 5, 0, 0, 3 };

const std::string Shared::tp_elem_geo_name[nb_tp_types] = { "Empty", "Line" , "Triag"  , "Quad" , "Tetra"  ,
                                                            "Hexa" , "Prism", "Pyramid", "Line" , "Triag"  ,
                                                            "Quad" , "Tetra", "Hexa"   , "Prism", "Pyramid",
                                                            "Point", "Quad" , "Hexa"   , "Prism", "Pyramid",
                                                            "Triag", "Triag", "Triag"  , "Triag", "Triag"  ,
                                                            "Triag", "Line" , "Line"   , "Line" , "Tetra"  ,
                                                            "Tetra", "Tetra", "Tetra"  , "Tetra", "Polyg"  ,
                                                            "Polyh", "Quad"
                                                          };

const std::string Shared::dim_name[4] = { "0D", "1D", "2D", "3D" };


const std::string Shared::order_name[10] = { "P0", "P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8", "P9" }; 



                                                            
//////////////////////////////////////////////////////////////////////////////

Shared::Shared() :
    m_nodes_cf_to_tp(nb_tp_types),
    m_nodes_tp_to_cf(nb_tp_types)
{
  m_supported_types.reserve(20);
  m_supported_types.push_back("CF.Mesh.SF.Line1DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Line2DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Line2DLagrangeP2");
  m_supported_types.push_back("CF.Mesh.SF.Line2DLagrangeP3");
  m_supported_types.push_back("CF.Mesh.SF.Line3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Quad2DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Quad2DLagrangeP2");
  m_supported_types.push_back("CF.Mesh.SF.Quad2DLagrangeP3");
  m_supported_types.push_back("CF.Mesh.SF.Quad3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Triag2DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Triag2DLagrangeP2");
  m_supported_types.push_back("CF.Mesh.SF.Triag2DLagrangeP3");
  m_supported_types.push_back("CF.Mesh.SF.Triag3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Hexa3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Tetra3DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Point1DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Point2DLagrangeP1");
  m_supported_types.push_back("CF.Mesh.SF.Point3DLagrangeP1");

  m_element_cf_to_tp[GeoShape::LINE ]=P1LINE;
  m_element_cf_to_tp[GeoShape::TRIAG]=P1TRIAG;
  m_element_cf_to_tp[GeoShape::QUAD ]=P1QUAD;
  m_element_cf_to_tp[GeoShape::HEXA ]=P1HEXA;
  m_element_cf_to_tp[GeoShape::TETRA]=P1TETRA;
  m_element_cf_to_tp[GeoShape::POINT]=P1POINT;

  // --------------------------------------------------- NODES

  // P1 line
  m_nodes_cf_to_tp[P1LINE].resize(2);
  m_nodes_cf_to_tp[P1LINE][0]=0;
  m_nodes_cf_to_tp[P1LINE][1]=1;

  m_nodes_tp_to_cf[P1LINE].resize(2);
  m_nodes_tp_to_cf[P1LINE][0]=0;
  m_nodes_tp_to_cf[P1LINE][1]=1;

  // P1 quad
  m_nodes_cf_to_tp[P1QUAD].resize(4);
  m_nodes_cf_to_tp[P1QUAD][0]=0;
  m_nodes_cf_to_tp[P1QUAD][1]=1;
  m_nodes_cf_to_tp[P1QUAD][2]=2;
  m_nodes_cf_to_tp[P1QUAD][3]=3;

  m_nodes_tp_to_cf[P1QUAD].resize(4);
  m_nodes_tp_to_cf[P1QUAD][0]=0;
  m_nodes_tp_to_cf[P1QUAD][1]=1;
  m_nodes_tp_to_cf[P1QUAD][2]=2;
  m_nodes_tp_to_cf[P1QUAD][3]=3;

  // P1 triag
  m_nodes_cf_to_tp[P1TRIAG].resize(3);
  m_nodes_cf_to_tp[P1TRIAG][0]=0;
  m_nodes_cf_to_tp[P1TRIAG][1]=1;
  m_nodes_cf_to_tp[P1TRIAG][2]=2;

  m_nodes_tp_to_cf[P1TRIAG].resize(3);
  m_nodes_tp_to_cf[P1TRIAG][0]=0;
  m_nodes_tp_to_cf[P1TRIAG][1]=1;
  m_nodes_tp_to_cf[P1TRIAG][2]=2;


  // P1 tetra
  m_nodes_cf_to_tp[P1TETRA].resize(4);
  m_nodes_cf_to_tp[P1TETRA][0]=0;
  m_nodes_cf_to_tp[P1TETRA][1]=1;
  m_nodes_cf_to_tp[P1TETRA][2]=2;
  m_nodes_cf_to_tp[P1TETRA][3]=3;

  m_nodes_tp_to_cf[P1TETRA].resize(4);
  m_nodes_tp_to_cf[P1TETRA][0]=0;
  m_nodes_tp_to_cf[P1TETRA][1]=1;
  m_nodes_tp_to_cf[P1TETRA][2]=2;
  m_nodes_tp_to_cf[P1TETRA][3]=3;


  // P1 hexa
  m_nodes_cf_to_tp[P1HEXA].resize(8);
  m_nodes_cf_to_tp[P1HEXA][0]=4;
  m_nodes_cf_to_tp[P1HEXA][1]=5;
  m_nodes_cf_to_tp[P1HEXA][2]=1;
  m_nodes_cf_to_tp[P1HEXA][3]=0;
  m_nodes_cf_to_tp[P1HEXA][4]=6;
  m_nodes_cf_to_tp[P1HEXA][5]=7;
  m_nodes_cf_to_tp[P1HEXA][6]=3;
  m_nodes_cf_to_tp[P1HEXA][7]=2;

  m_nodes_tp_to_cf[P1HEXA].resize(8);
  m_nodes_tp_to_cf[P1HEXA][0]=3;
  m_nodes_tp_to_cf[P1HEXA][1]=2;
  m_nodes_tp_to_cf[P1HEXA][2]=7;
  m_nodes_tp_to_cf[P1HEXA][3]=6;
  m_nodes_tp_to_cf[P1HEXA][4]=0;
  m_nodes_tp_to_cf[P1HEXA][5]=1;
  m_nodes_tp_to_cf[P1HEXA][6]=4;
  m_nodes_tp_to_cf[P1HEXA][7]=5;

  // P2 line
  m_nodes_cf_to_tp[P2LINE].resize(3);
  m_nodes_cf_to_tp[P2LINE][0]=0;
  m_nodes_cf_to_tp[P2LINE][1]=1;
  m_nodes_cf_to_tp[P2LINE][2]=2;

  m_nodes_tp_to_cf[P2LINE].resize(3);
  m_nodes_tp_to_cf[P2LINE][0]=0;
  m_nodes_tp_to_cf[P2LINE][1]=1;
  m_nodes_tp_to_cf[P2LINE][2]=2;

  // P2 quad
  m_nodes_cf_to_tp[P2QUAD].resize(9);
  m_nodes_cf_to_tp[P2QUAD][0]=0;
  m_nodes_cf_to_tp[P2QUAD][1]=1;
  m_nodes_cf_to_tp[P2QUAD][2]=2;
  m_nodes_cf_to_tp[P2QUAD][3]=3;
  m_nodes_cf_to_tp[P2QUAD][4]=4;
  m_nodes_cf_to_tp[P2QUAD][5]=5;
  m_nodes_cf_to_tp[P2QUAD][6]=6;
  m_nodes_cf_to_tp[P2QUAD][7]=7;
  m_nodes_cf_to_tp[P2QUAD][8]=8;


  m_nodes_tp_to_cf[P2QUAD].resize(9);
  m_nodes_tp_to_cf[P2QUAD][0]=0;
  m_nodes_tp_to_cf[P2QUAD][1]=1;
  m_nodes_tp_to_cf[P2QUAD][2]=2;
  m_nodes_tp_to_cf[P2QUAD][3]=3;
  m_nodes_tp_to_cf[P2QUAD][4]=4;
  m_nodes_tp_to_cf[P2QUAD][5]=5;
  m_nodes_tp_to_cf[P2QUAD][6]=6;
  m_nodes_tp_to_cf[P2QUAD][7]=7;
  m_nodes_tp_to_cf[P2QUAD][8]=8;


  // P2 triag
  m_nodes_cf_to_tp[P2TRIAG].resize(6);
  m_nodes_cf_to_tp[P2TRIAG][0]=0;
  m_nodes_cf_to_tp[P2TRIAG][1]=1;
  m_nodes_cf_to_tp[P2TRIAG][2]=2;
  m_nodes_cf_to_tp[P2TRIAG][3]=3;
  m_nodes_cf_to_tp[P2TRIAG][4]=4;
  m_nodes_cf_to_tp[P2TRIAG][5]=5;

  m_nodes_tp_to_cf[P2TRIAG].resize(6);
  m_nodes_tp_to_cf[P2TRIAG][0]=0;
  m_nodes_tp_to_cf[P2TRIAG][1]=1;
  m_nodes_tp_to_cf[P2TRIAG][2]=2;
  m_nodes_tp_to_cf[P2TRIAG][3]=3;
  m_nodes_tp_to_cf[P2TRIAG][4]=4;
  m_nodes_tp_to_cf[P2TRIAG][5]=5;


  // P2 tetra
  m_nodes_cf_to_tp[P2TETRA].resize(10);
  m_nodes_cf_to_tp[P2TETRA][0]=0;
  m_nodes_cf_to_tp[P2TETRA][1]=1;
  m_nodes_cf_to_tp[P2TETRA][2]=2;
  m_nodes_cf_to_tp[P2TETRA][3]=3;
  m_nodes_cf_to_tp[P2TETRA][4]=4;
  m_nodes_cf_to_tp[P2TETRA][5]=5;
  m_nodes_cf_to_tp[P2TETRA][6]=6;
  m_nodes_cf_to_tp[P2TETRA][7]=7;
  m_nodes_cf_to_tp[P2TETRA][8]=8;
  m_nodes_cf_to_tp[P2TETRA][9]=9;

  m_nodes_tp_to_cf[P2TETRA].resize(10);
  m_nodes_tp_to_cf[P2TETRA][0]=0;
  m_nodes_tp_to_cf[P2TETRA][1]=1;
  m_nodes_tp_to_cf[P2TETRA][2]=2;
  m_nodes_tp_to_cf[P2TETRA][3]=3;
  m_nodes_tp_to_cf[P2TETRA][4]=4;
  m_nodes_tp_to_cf[P2TETRA][5]=5;
  m_nodes_tp_to_cf[P2TETRA][6]=6;
  m_nodes_tp_to_cf[P2TETRA][7]=7;
  m_nodes_tp_to_cf[P2TETRA][8]=8;
  m_nodes_tp_to_cf[P2TETRA][9]=9;


  // P2 hexa
  m_nodes_cf_to_tp[P2HEXA].resize(27);
  m_nodes_cf_to_tp[P2HEXA][0]=0;
  m_nodes_cf_to_tp[P2HEXA][1]=1;
  m_nodes_cf_to_tp[P2HEXA][2]=2;
  m_nodes_cf_to_tp[P2HEXA][3]=3;
  m_nodes_cf_to_tp[P2HEXA][4]=4;
  m_nodes_cf_to_tp[P2HEXA][5]=5;
  m_nodes_cf_to_tp[P2HEXA][6]=6;
  m_nodes_cf_to_tp[P2HEXA][7]=7;
  m_nodes_cf_to_tp[P2HEXA][8]=8;
  m_nodes_cf_to_tp[P2HEXA][9]=9;
  m_nodes_cf_to_tp[P2HEXA][10]=10;
  m_nodes_cf_to_tp[P2HEXA][11]=11;
  m_nodes_cf_to_tp[P2HEXA][12]=12;
  m_nodes_cf_to_tp[P2HEXA][13]=13;
  m_nodes_cf_to_tp[P2HEXA][14]=14;
  m_nodes_cf_to_tp[P2HEXA][15]=15;
  m_nodes_cf_to_tp[P2HEXA][16]=16;
  m_nodes_cf_to_tp[P2HEXA][17]=17;
  m_nodes_cf_to_tp[P2HEXA][18]=18;
  m_nodes_cf_to_tp[P2HEXA][19]=19;
  m_nodes_cf_to_tp[P2HEXA][20]=20;
  m_nodes_cf_to_tp[P2HEXA][21]=21;
  m_nodes_cf_to_tp[P2HEXA][22]=22;
  m_nodes_cf_to_tp[P2HEXA][23]=23;
  m_nodes_cf_to_tp[P2HEXA][24]=24;
  m_nodes_cf_to_tp[P2HEXA][25]=25;
  m_nodes_cf_to_tp[P2HEXA][26]=26;

  m_nodes_tp_to_cf[P2HEXA].resize(27);
  m_nodes_tp_to_cf[P2HEXA][0]=0;
  m_nodes_tp_to_cf[P2HEXA][1]=1;
  m_nodes_tp_to_cf[P2HEXA][2]=2;
  m_nodes_tp_to_cf[P2HEXA][3]=3;
  m_nodes_tp_to_cf[P2HEXA][4]=4;
  m_nodes_tp_to_cf[P2HEXA][5]=5;
  m_nodes_tp_to_cf[P2HEXA][6]=6;
  m_nodes_tp_to_cf[P2HEXA][7]=7;
  m_nodes_tp_to_cf[P2HEXA][8]=8;
  m_nodes_tp_to_cf[P2HEXA][9]=9;
  m_nodes_tp_to_cf[P2HEXA][10]=10;
  m_nodes_tp_to_cf[P2HEXA][11]=11;
  m_nodes_tp_to_cf[P2HEXA][12]=12;
  m_nodes_tp_to_cf[P2HEXA][13]=13;
  m_nodes_tp_to_cf[P2HEXA][14]=14;
  m_nodes_tp_to_cf[P2HEXA][15]=15;
  m_nodes_tp_to_cf[P2HEXA][16]=16;
  m_nodes_tp_to_cf[P2HEXA][17]=17;
  m_nodes_tp_to_cf[P2HEXA][18]=18;
  m_nodes_tp_to_cf[P2HEXA][19]=19;
  m_nodes_tp_to_cf[P2HEXA][20]=20;
  m_nodes_tp_to_cf[P2HEXA][21]=21;
  m_nodes_tp_to_cf[P2HEXA][22]=22;
  m_nodes_tp_to_cf[P2HEXA][23]=23;
  m_nodes_tp_to_cf[P2HEXA][24]=24;
  m_nodes_tp_to_cf[P2HEXA][25]=25;
  m_nodes_tp_to_cf[P2HEXA][26]=26;

  //Point
  m_nodes_tp_to_cf[P1POINT].resize(1);
  m_nodes_tp_to_cf[P1POINT][0]=0;

  //P3 triag
  m_nodes_cf_to_tp[P3TRIAG].resize(10);
  m_nodes_cf_to_tp[P3TRIAG][0] = 0;
  m_nodes_cf_to_tp[P3TRIAG][1] = 1;
  m_nodes_cf_to_tp[P3TRIAG][2] = 2;
  m_nodes_cf_to_tp[P3TRIAG][3] = 3;
  m_nodes_cf_to_tp[P3TRIAG][4] = 4;
  m_nodes_cf_to_tp[P3TRIAG][5] = 5;
  m_nodes_cf_to_tp[P3TRIAG][6] = 6;
  m_nodes_cf_to_tp[P3TRIAG][7] = 7;
  m_nodes_cf_to_tp[P3TRIAG][8] = 8;
  m_nodes_cf_to_tp[P3TRIAG][9] = 9;


  m_nodes_tp_to_cf[P3TRIAG].resize(10);
  m_nodes_tp_to_cf[P3TRIAG][0] = 0;
  m_nodes_tp_to_cf[P3TRIAG][1] = 1;
  m_nodes_tp_to_cf[P3TRIAG][2] = 2;
  m_nodes_tp_to_cf[P3TRIAG][3] = 3;
  m_nodes_tp_to_cf[P3TRIAG][4] = 4;
  m_nodes_tp_to_cf[P3TRIAG][5] = 5;
  m_nodes_tp_to_cf[P3TRIAG][6] = 6;
  m_nodes_tp_to_cf[P3TRIAG][7] = 7;
  m_nodes_tp_to_cf[P3TRIAG][8] = 8;
  m_nodes_tp_to_cf[P3TRIAG][9] = 9;

  //P3 line
  m_nodes_cf_to_tp[P3LINE].resize(4);
  m_nodes_cf_to_tp[P3LINE][0] = 0;
  m_nodes_cf_to_tp[P3LINE][1] = 1;
  m_nodes_cf_to_tp[P3LINE][2] = 2;
  m_nodes_cf_to_tp[P3LINE][3] = 3;

  m_nodes_tp_to_cf[P3LINE].resize(4);
  m_nodes_tp_to_cf[P3LINE][0] = 0;
  m_nodes_tp_to_cf[P3LINE][1] = 1;
  m_nodes_tp_to_cf[P3LINE][2] = 2;
  m_nodes_tp_to_cf[P3LINE][3] = 3;

  //P3 quad (16 nodes)
  m_nodes_cf_to_tp[P3QUAD].resize(16);
  m_nodes_cf_to_tp[P3QUAD][0]  = 0;
  m_nodes_cf_to_tp[P3QUAD][1]  = 1;
  m_nodes_cf_to_tp[P3QUAD][2]  = 2;
  m_nodes_cf_to_tp[P3QUAD][3]  = 3;
  m_nodes_cf_to_tp[P3QUAD][4]  = 4;
  m_nodes_cf_to_tp[P3QUAD][5]  = 5;
  m_nodes_cf_to_tp[P3QUAD][6]  = 6;
  m_nodes_cf_to_tp[P3QUAD][7]  = 7;
  m_nodes_cf_to_tp[P3QUAD][8]  = 8;
  m_nodes_cf_to_tp[P3QUAD][9]  = 9;
  m_nodes_cf_to_tp[P3QUAD][10] = 10;
  m_nodes_cf_to_tp[P3QUAD][11] = 11;
  m_nodes_cf_to_tp[P3QUAD][12] = 12;
  m_nodes_cf_to_tp[P3QUAD][13] = 13;
  m_nodes_cf_to_tp[P3QUAD][14] = 14;
  m_nodes_cf_to_tp[P3QUAD][15] = 15;

  m_nodes_tp_to_cf[P3QUAD].resize(16);
  m_nodes_tp_to_cf[P3QUAD][0]  = 0;
  m_nodes_tp_to_cf[P3QUAD][1]  = 1;
  m_nodes_tp_to_cf[P3QUAD][2]  = 2;
  m_nodes_tp_to_cf[P3QUAD][3]  = 3;
  m_nodes_tp_to_cf[P3QUAD][4]  = 4;
  m_nodes_tp_to_cf[P3QUAD][5]  = 5;
  m_nodes_tp_to_cf[P3QUAD][6]  = 6;
  m_nodes_tp_to_cf[P3QUAD][7]  = 7;
  m_nodes_tp_to_cf[P3QUAD][8]  = 8;
  m_nodes_tp_to_cf[P3QUAD][9]  = 9;
  m_nodes_tp_to_cf[P3QUAD][10] = 10;
  m_nodes_tp_to_cf[P3QUAD][11] = 11;
  m_nodes_tp_to_cf[P3QUAD][12] = 12;
  m_nodes_tp_to_cf[P3QUAD][13] = 13;
  m_nodes_tp_to_cf[P3QUAD][14] = 14;
  m_nodes_tp_to_cf[P3QUAD][15] = 15;

}


//////////////////////////////////////////////////////////////////////////////

std::string Shared::tp_name_to_cf_name(const Uint dim, const Uint tp_type)
{
  //Compose the name of the form   "CF.Mesh.SF.Line1DLagrangeP1"
  const Uint order = m_tp_elem_order[tp_type];
  std::string name = "CF.Mesh.SF." + tp_elem_geo_name[tp_type] + dim_name[dim] + "Lagrange" + order_name[order];
  return name;
}

//////////////////////////////////////////////////////////////////////////////

} // Tecplot
} // Mesh
} // CF
