# TQ: NOTE
#     I leave the sources here but deactivate this code because
#     it follows the old Physics API.
#     The owner of this module should upgrade it to the new API or remove it.

list( APPEND coolfluid_sfdm_sf_condition FALSE )

list( APPEND coolfluid_sfdm_sf_files
  LibSF.hpp
  LibSF.cpp
  LineFluxP1.hpp
  LineFluxP1.cpp
  LineFluxP2.hpp
  LineFluxP2.cpp
  LineFluxP3.hpp
  LineFluxP3.cpp
  LineSolutionP0.hpp
  LineSolutionP0.cpp
  LineSolutionP1.hpp
  LineSolutionP1.cpp
  LineSolutionP2.hpp
  LineSolutionP2.cpp
  QuadFluxP1.hpp
  QuadFluxP1.cpp
  QuadFluxP2.hpp
  QuadFluxP2.cpp
  QuadFluxP3.hpp
  QuadFluxP3.cpp
  QuadSolutionP0.hpp
  QuadSolutionP0.cpp
  QuadSolutionP1.hpp
  QuadSolutionP1.cpp
  QuadSolutionP2.hpp
  QuadSolutionP2.cpp
)

list( APPEND coolfluid_sfdm_sf_cflibs coolfluid_sfdm coolfluid_mesh coolfluid_mesh_sf )

coolfluid_add_library( coolfluid_sfdm_sf )

