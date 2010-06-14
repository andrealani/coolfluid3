#include <boost/foreach.hpp>

#include "Common/ObjectProvider.hpp"
#include "Mesh/P1/Triag2D.hpp"
#include "Mesh/P1/Line2D.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace CF {
namespace Mesh {
namespace P1 {
  
////////////////////////////////////////////////////////////////////////////////

Common::ObjectProvider < Triag2D,
                         ElementType,
                         P1Lib >
aP1Triag2D_Provider ( "P1-"+Triag2D::getClassName() );


Triag2D::Triag2D() 
{
  m_shape=GeoShape::TRIAG;
  m_shapeName=GeoShape::Convert::to_str(m_shape);
  m_order=1;
  m_nbNodes=3;
  m_dimensionality=2;
  m_nbEdges=0;
  
  // set faces
  m_faces.reserve(3);
  boost::shared_ptr<ElementType> line(new Line2D);
  std::vector<Uint> nodes(2);

  nodes[0]=0;   nodes[1]=1;   m_faces.push_back( Face(line,nodes));
  nodes[0]=1;   nodes[1]=2;   m_faces.push_back( Face(line,nodes));
  nodes[0]=2;   nodes[1]=0;   m_faces.push_back( Face(line,nodes));
}

////////////////////////////////////////////////////////////////////////////////

} // P1
} // Mesh
} // CF
