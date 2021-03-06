################################################################################
# test Component CAction

list( APPEND utest-solver-actions_cflibs coolfluid_solver_actions coolfluid_mesh_actions coolfluid_mesh_sf coolfluid_mesh_gmsh coolfluid_mesh_neu)
list( APPEND utest-solver-actions_files
  CDummyLoopOperation.hpp
  CDummyLoopOperation.cpp
  utest-solver-actions.cpp )

coolfluid_add_unit_test( utest-solver-actions )

list( APPEND mesh_files  rotation-tg-p1.neu  rotation-qd-p1.neu  )
foreach( mfile ${mesh_files} )
  add_custom_command(TARGET utest-solver-actions
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CF_RESOURCE_DIR}/${mfile} ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}
                    )
endforeach()


################################################################################
# proto tests
if(CF_ENABLE_PROTO)

  ################################################################################
  # test Proto operations

  list( APPEND utest-proto-benchmark_cflibs coolfluid_mesh coolfluid_solver_actions coolfluid_mesh_sf coolfluid_mesh_block coolfluid_testing coolfluid_mesh_generation coolfluid_solver )
  list( APPEND utest-proto-benchmark_files  utest-proto-benchmark.cpp )

  set( utest-proto-benchmark_profile ON )

  set( utest-proto-benchmark_condition ${CF_TMP_HAVE_SIMPLECOMM})

  coolfluid_add_unit_test( utest-proto-benchmark )

  ################################################################################

  list( APPEND utest-proto-operators_cflibs coolfluid_mesh coolfluid_solver_actions coolfluid_mesh_sf coolfluid_mesh_generation coolfluid_solver )
  list( APPEND utest-proto-operators_files
    utest-proto-operators.cpp )
  coolfluid_add_unit_test( utest-proto-operators )

  list( APPEND utest-proto-components_cflibs coolfluid_mesh coolfluid_mesh_sf coolfluid_mesh_generation coolfluid_solver_actions coolfluid_solver )
  list( APPEND utest-proto-components_files
    utest-proto-components.cpp )
  coolfluid_add_unit_test( utest-proto-components )

else()

  coolfluid_mark_not_orphan(utest-proto-benchmark.cpp utest-proto-operators.cpp utest-proto-components.cpp)

endif()
