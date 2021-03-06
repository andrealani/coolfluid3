// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Test module for CF::FVM::BuildGhostStates"

#include <boost/test/unit_test.hpp>
#include <boost/assign/list_of.hpp>

#include "Common/BoostFilesystem.hpp"

#include "Common/Log.hpp"
#include "Common/Core.hpp"
#include "Common/CRoot.hpp"

#include "Tools/MeshGeneration/MeshGeneration.hpp"

#include "Mesh/CRegion.hpp"
#include "Mesh/CNodes.hpp"
#include "Mesh/CDomain.hpp"
#include "Mesh/CMesh.hpp"
#include "Mesh/CMeshWriter.hpp"
#include "Mesh/CMeshReader.hpp"
#include "Mesh/CMeshTransformer.hpp"
#include "Mesh/CSimpleMeshGenerator.hpp"
#include "Mesh/CFaceCellConnectivity.hpp"
#include "Mesh/CFaces.hpp"
#include "Mesh/Actions/CBuildFaces.hpp"
#include "Mesh/Actions/CBuildFaceNormals.hpp"
#include "Mesh/Actions/CExtract.hpp"
#include "Mesh/CCells.hpp"


#include "FVM/Core/BuildGhostStates.hpp"

using namespace boost;
using namespace boost::assign;
using namespace boost::filesystem;
using namespace CF;
using namespace CF::Common;
using namespace CF::Common::XML;
using namespace CF::Mesh;
using namespace CF::Mesh::Actions;
using namespace CF::FVM;
using namespace CF::FVM::Core;
using namespace CF::Tools::MeshGeneration;

//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( FVM_Suite )

//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( test_buildghoststates )
{
  CMesh::Ptr mesh = Common::Core::instance().root().create_component_ptr<CMesh>("mesh");

  CMeshReader::Ptr meshreader = build_component_abstract_type<CMeshReader>("CF.Mesh.Neu.CReader","meshreader");

  boost::filesystem::path fp_in("quadtriag.neu");
  //meshreader->do_read_mesh_into(fp_in,mesh);

  //Tools::MeshGeneration::create_line(*mesh, 10. , 10);
  CSimpleMeshGenerator::create_rectangle(*mesh, 10. , 10., 4 , 4 );

  CBuildFaces::Ptr build_faces = allocate_component<CBuildFaces>("build_faces");
  build_faces->transform(mesh);

  build_component_abstract_type<CMeshTransformer>("CF.Mesh.Actions.CreateSpaceP0","create_spaceP0")->transform(mesh);

  CBuildFaceNormals::Ptr build_facenormals = allocate_component<CBuildFaceNormals>("build_facenormals");
  build_facenormals->transform(mesh);

  BuildGhostStates::Ptr build_gstates = allocate_component<BuildGhostStates>("build_gstates");
  build_gstates->transform(mesh);


  CMeshWriter::Ptr meshwriter = build_component_abstract_type<CMeshWriter>("CF.Mesh.Gmsh.CWriter","meshwriter");

  std::vector<CField::Ptr> fields;
  boost_foreach(CField& field, find_components_recursively<CField>(*mesh))
    fields.push_back(field.as_ptr<CField>());
  meshwriter->set_fields(fields);


  meshwriter->write_from_to(*mesh,"quadtriag_ghosts.msh");

}

////////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()

////////////////////////////////////////////////////////////////////////////////

