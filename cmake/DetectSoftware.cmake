##############################################################################
# finding boost (essential)

include( DetectBoost )

##############################################################################
# finding MPI (essential) must be after the GlobalOptions

find_package( MPI QUIET )

coolfluid_set_package( PACKAGE MPI DESCRIPTION "parallel communication" )

##############################################################################
# Find Qt - defines QT_USE_FILE and QT_LIBRARIES used below

#  set( QT_USE_QTXML TRUE     )
#  set( QT_USE_QTNETWORK TRUE )

if(APPLE)
  set(QT_USE_FRAMEWORKS TRUE)
endif()

find_package( Qt4 4.6.0 COMPONENTS QtCore QtGui QtXml QtNetwork QtTest QtSvg )

if(NOT DEFINED QT4_FOUND)
  set(QT4_FOUND NO)
endif()

coolfluid_log_file("QT_FOUND: [${QT4_FOUND}]")

if(${QT4_FOUND})
  coolfluid_log_file("Qt version: [${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}]")
  coolfluid_log_file("Qt libs: [${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${QT_QTTEST_LIBRARY}]")
endif()

coolfluid_set_package( PACKAGE Qt4 DESCRIPTION "UI framework" )

##############################################################################
#  Find ParaView

include( DetectParaView )

##############################################################################
# find non essential packages

# packages that don't influence functionality
# user should not be concerned with these
# so make them quiet and they don't appear on the enable package list

find_package(OpenSSL           QUIET ) # OpenSSL library
find_package(PythonInterp      QUIET ) # Python interpreter
find_package(CMath             QUIET ) # find the math library
find_package(Realtime          QUIET ) # POSIX Realtime library
find_package(Valgrind          QUIET ) # valgrind for profiling and memmory leak detection
find_package(GooglePerftools   QUIET ) # dynamic profiler and memory checker
find_package(PThread           QUIET ) # POSIX Threads library

# packages that enhance functionality
# this will appear on the list of enabled features

find_package(ZLIB)     # file compression support

coolfluid_log_file( "ZLIB_FOUND: [${ZLIB_FOUND}]" )
coolfluid_log_file( "  ZLIB_INCLUDE_DIRS: [${ZLIB_INCLUDE_DIRS}]" )
coolfluid_log_file( "  ZLIB_LIBRARIES:    [${ZLIB_LIBRARIES}]" )

coolfluid_set_package( PACKAGE ZLIB DESCRIPTION "file compression" )

find_package(BZip2)    # file compression support

coolfluid_log_file( "BZIP2_FOUND: [${BZIP2_FOUND}]" )
coolfluid_log_file( "  BZIP2_INCLUDE_DIR:  [${BZIP2_INCLUDE_DIR}]" )
coolfluid_log_file( "  BZIP2_LIBRARIES:    [${BZIP2_LIBRARIES}]" )
coolfluid_log_file( "  BZIP2_DEFINITIONS:  [${BZIP2_DEFINITIONS}]" )
coolfluid_log_file( "  BZIP2_NEED_PREFIX:  [${BZIP2_NEED_PREFIX}]" )

coolfluid_set_package( PACKAGE BZip2 DESCRIPTION "file compression" )

find_package(BlasLapack)      # search for Blas Lapack support
find_package(PTScotch)        # parallel domain decomposition
find_package(Metis)           # serial domain decomposition
find_package(Parmetis)        # parallel domain decomposition
find_package(Zoltan)          # parallel and serial domain decomposition using parmetis or pt-scotch
find_package(Curl)            # curl downloads files on the fly
find_package(CGNS)            # CGNS library
find_package(SuperLU)         # SuperLU sparse sirect solver
find_package(Trilinos)        # Trilinos sparse matrix library

# gpu software

# opencl support
if( CF_ENABLE_OPENCL AND CF_ENABLE_GPU )
  find_package(OpenCL)
  coolfluid_set_package( PACKAGE OpenCL DESCRIPTION "gpu computing" )
endif()

# cuda support
if( CF_ENABLE_CUDA AND CF_ENABLE_GPU )
  find_package(CUDA)
  coolfluid_log_file( "CUDA_FOUND: [${CUDA_FOUND}]" )
  coolfluid_set_package( PACKAGE CUDA DESCRIPTION "gpu computing" )
endif()
