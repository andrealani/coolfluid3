##############################################################################
# BlockMeshReader Plugin
##############################################################################

cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

project( BlockMeshReader CXX )

coolfluid_define_plugin( BlockMeshReader ${CMAKE_CURRENT_SOURCE_DIR} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

# disable optimizations to avoid enormous compilation times and memory usage
option(CF_STRIP_BLOCKMESHREADER_COMPILE_FLAGS "Strip optimization and debug flags for BlockMeshReader, to avoid the long compilation" ON)
set(CF_BLOCKMESHREADER_COMPILE_FLAGS "")

if( CF_STRIP_BLOCKMESHREADER_COMPILE_FLAGS AND CMAKE_COMPILER_IS_GNUCC )
    set(CF_BLOCKMESHREADER_COMPILE_FLAGS "${CF_BLOCKMESHREADER_COMPILE_FLAGS} -g0 -O0")
endif()

coolfluid_find_orphan_files()

##############################################################################
# library

list( APPEND coolfluid_mesh_block_reader_files
  BlockMeshReader.cpp
  BlockMeshReader.hpp
  LibBlockMeshReader.cpp
  LibBlockMeshReader.hpp
  Parser.cpp
  Parser.hpp
)

list( APPEND coolfluid_mesh_block_reader_cflibs coolfluid_mesh coolfluid_mesh_generation coolfluid_mesh_block coolfluid_mesh_sf )

set( coolfluid_mesh_block_reader_condition ${CF_TMP_HAVE_SIMPLECOMM})

coolfluid_add_library( coolfluid_mesh_block_reader )

if( CF_BLOCKMESHREADER_COMPILE_FLAGS )
  set_source_files_properties(Parser.cpp PROPERTIES COMPILE_FLAGS ${CF_BLOCKMESHREADER_COMPILE_FLAGS})
endif()

##############################################################################
# unit tests

add_subdirectory( test )
