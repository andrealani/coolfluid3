##############################################################################
# RiemannSolvers plugin
##############################################################################

cmake_minimum_required( VERSION 2.8 FATAL_ERROR )

project( RungeKutta CXX )

coolfluid_define_plugin( RungeKutta ${CMAKE_CURRENT_SOURCE_DIR} )

if( CF_PLUGIN_RUNGEKUTTA )

  coolfluid_find_orphan_files()

  include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/src )

  add_subdirectory( src )  # library sources

  add_subdirectory( test ) # testing tree

endif()
