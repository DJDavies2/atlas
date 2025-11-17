# (C) Copyright 2020- ECMWF.
#
# This software is licensed under the terms of the Apache Licence Version 2.0
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# In applying this licence, ECMWF does not waive the privileges and immunities
# granted to it by virtue of its status as an intergovernmental organisation
# nor does it submit to any jurisdiction.

cmake_minimum_required( VERSION 3.12 FATAL_ERROR )
find_package( ecbuild 3.4 REQUIRED )
project( atlas_field_api_test_install VERSION 0.0.0 LANGUAGES Fortran )

ecbuild_find_package( atlas_field_api REQUIRED )

foreach( prec sp dp )
    set( parkind atlas_field_api_test_install_parkind_${prec} )
    ecbuild_add_library( TARGET ${parkind} SOURCES parkind1.F90 )
    set_target_properties( ${parkind} PROPERTIES Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/module/${parkind} )
    target_include_directories( ${parkind} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/module/${parkind}>)
    if( prec STREQUAL "sp" )
        target_compile_definitions( ${parkind} PRIVATE SINGLE_PRECISION )
    endif()

    ecbuild_add_executable( TARGET  main_${prec}
                        SOURCES main.F90
                        LIBS    atlas_field_api_${prec} ${parkind} )

endforeach()
