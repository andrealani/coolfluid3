// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Test module for OSystem layer"

#include <boost/test/unit_test.hpp>

#include "Common/OSystem.hpp"
#include "Common/OSystemLayer.hpp"

using namespace std;
using namespace CF;
using namespace CF::Common;

//////////////////////////////////////////////////////////////////////////////

struct OSystemFixture
{
  /// common setup for each test case
  OSystemFixture() {}

  /// common tear-down for each test case
  ~OSystemFixture() {}
};

//////////////////////////////////////////////////////////////////////////////

BOOST_FIXTURE_TEST_SUITE( OSystem_TestSuite, OSystemFixture )

//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_CASE( system_layer_ptr )
{
}

BOOST_AUTO_TEST_CASE( libloader_ptr )
{
  BOOST_CHECK( OSystem::instance().lib_loader() != nullptr  );
}

BOOST_AUTO_TEST_CASE( execute_command )
{
  /// @todo this test is not cross-platform
  // should exit normally
  BOOST_CHECK_NO_THROW( OSystem::instance().execute_system_command("echo"));
  // the command does *normally* not exist, should throw an exception
  /// @todo find a command that throws an exception
  //BOOST_CHECK_THROW( OSystem::instance().execute_command("cd /aDirThatDoesNotExist"), OSystemError);
}

BOOST_AUTO_TEST_CASE( system_layer )
{
  BOOST_CHECK( OSystem::instance().system_layer() != nullptr );

  BOOST_CHECK( OSystem::instance().system_layer()->platform_name() != std::string() );
  BOOST_CHECK( OSystem::instance().system_layer()->back_trace() != std::string() );
  BOOST_CHECK( OSystem::instance().system_layer()->process_id() == 0 );

  BOOST_CHECK( OSystem::instance().system_layer()->memory_usage() > 0 );
}

//////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE_END()