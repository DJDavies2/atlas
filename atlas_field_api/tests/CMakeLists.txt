
foreach( prec sp dp )
    set( atlas_field_api_parkind atlas_field_api_parkind_${prec} )
    add_library( ${atlas_field_api_parkind} SHARED parkind1.F90 )
    set_target_properties( ${atlas_field_api_parkind} PROPERTIES Fortran_MODULE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/module/${atlas_field_api_parkind} )
    target_include_directories( ${atlas_field_api_parkind} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/module/${atlas_field_api_parkind}>)
    if( prec STREQUAL "sp" )
        target_compile_definitions( ${atlas_field_api_parkind} PRIVATE SINGLE_PRECISION )
    endif()
endforeach()

list( APPEND programs
    init_owner_gpu
    init_wrapper_gpu
    init_wrapper_non_contiguous
    init_wrapper_non_contiguous_gpu
    test_last_dim
    test_field_tracking
    test_sync_on_final
)
foreach( program ${programs} )
    foreach( prec sp dp )
        set( target atlas_field_api_${program}_${prec} )
        add_executable( ${target} )
        target_sources( ${target} PUBLIC ${program}.F90 )
        target_link_libraries( ${target} PUBLIC atlas_field_api_${prec} atlas_field_api_parkind_${prec} )
        add_test(${target} ${target})
    endforeach()
endforeach()

# ----------------------------------------------------------------------------------------
# Test installation of atlas_field_api is working

configure_file( test-install.sh.in ${CMAKE_CURRENT_BINARY_DIR}/test-install.sh @ONLY )

unset( _test_args )
if( CMAKE_TOOLCHAIN_FILE )
  list( APPEND _test_args "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}" )
endif()
foreach( lang C CXX Fortran )
  if( CMAKE_${lang}_COMPILER )
    list( APPEND _test_args "-DCMAKE_${lang}_COMPILER=${CMAKE_${lang}_COMPILER}" )
  endif()
  if( CMAKE_${lang}_FLAGS )
    list( APPEND _test_args "-DCMAKE_${lang}_FLAGS=${CMAKE_${lang}_FLAGS}" )
  endif()
  if( CMAKE_EXE_LINKER_FLAGS )
    list( APPEND _test_args "-DCMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}" )
  endif()
endforeach()

add_test( NAME atlas_field_api_test_install
          COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test-install.sh ${_test_args} )
