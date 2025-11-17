# (C) Copyright 2025- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation nor
# does it submit to any jurisdiction.

############################################################################################
# atlas_field_api

cmake_minimum_required( VERSION 3.18 FATAL_ERROR )

find_package( ecbuild 3.4 REQUIRED HINTS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild ${CMAKE_CURRENT_SOURCE_DIR}/../../ecbuild )

################################################################################
# Initialise project

if( NOT DEFINED atlas_VERSION )
  set( atlas_VERSION 0.0.0 )
endif()

project( atlas_field_api VERSION ${atlas_VERSION} LANGUAGES Fortran )

################################################################################
# Features that can be enabled / disabled with -DENABLE_<FEATURE>

find_package( atlas REQUIRED )
find_package( fckit REQUIRED )
find_package( pluto REQUIRED )

find_package( OpenMP COMPONENTS Fortran )
set( HAVE_OMP 0 )
if( OpenMP_Fortran_FOUND )
  set( HAVE_OMP 1 )
endif()
message(STATUS "HAVE_OMP: ${HAVE_OMP}")

set (HAVE_ACC 0)
if (atlas_HAVE_ACC)
  find_package( OpenACC COMPONENTS Fortran REQUIRED )
  set (HAVE_ACC 1)
endif()
message(STATUS "HAVE_ACC: ${HAVE_ACC}")

add_subdirectory( src )
add_subdirectory( tests )

################################################################################
# Export and summarize

ecbuild_add_resources(
    TARGET atlas_field_api-others
    SOURCES_PACK
        README.md
        CHANGELOG.md
        LICENSE
)

ecbuild_install_project( NAME atlas_field_api )
ecbuild_print_summary()
