list( APPEND coolfluid_mesh_ptscotch_files
      CPartitioner.hpp
      CPartitioner.cpp
      LibPTScotch.cpp
      LibPTScotch.hpp
    )

list( APPEND coolfluid_mesh_ptscotch_cflibs coolfluid_mesh )
list( APPEND coolfluid_mesh_ptscotch_libs ${PTSCOTCH_LIBRARIES} )
list( APPEND coolfluid_mesh_ptscotch_includedirs ${PTSCOTCH_INCLUDE_DIR} )

set( coolfluid_mesh_ptscotch_kernellib TRUE )

set( coolfluid_mesh_ptscotch_condition ${CF_HAVE_PTSCOTCH} )

coolfluid_add_library( coolfluid_mesh_ptscotch )

