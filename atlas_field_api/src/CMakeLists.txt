set( atlas_field_api_link_libraries
  atlas_f
  pluto_f
  $<${HAVE_OMP}:OpenMP::OpenMP_Fortran>
  $<${HAVE_ACC}:OpenACC::OpenACC_Fortran>
)

ecbuild_add_library( TARGET atlas_field_api_objects
                     TYPE OBJECT
                     SOURCES
                       field_ranksuff_module.fypp
                       field_factory_module.fypp
                       field_abort_module.F90
                       field_helper_module.F90
                       field_async_module.F90
                       field_defaults_module.F90
                     PUBLIC_LIBS ${atlas_field_api_link_libraries}
                     PUBLIC_INCLUDES
                       $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/module/atlas_field_api>
                       $<INSTALL_INTERFACE:module/atlas_field_api>
)
set_target_properties( atlas_field_api_objects PROPERTIES Fortran_MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/module/atlas_field_api )
fckit_target_preprocess_fypp( atlas_field_api_objects )

foreach( prec sp dp )
  set( atlas_field_api atlas_field_api_${prec} )
  ecbuild_add_library( TARGET ${atlas_field_api}
                       SOURCES
                         $<TARGET_OBJECTS:atlas_field_api_objects>
                         field_module.F90
                       PUBLIC_LIBS ${atlas_field_api_link_libraries}
                       PUBLIC_INCLUDES
                         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/module/atlas_field_api>
                         $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/module/atlas_field_api/${prec}>
                         $<INSTALL_INTERFACE:module/atlas_field_api/${prec}>
                         $<INSTALL_INTERFACE:module/atlas_field_api>
  )
  if( prec STREQUAL "sp" )
    target_compile_definitions( ${atlas_field_api} PRIVATE SINGLE_PRECISION )
  endif()
  set_target_properties( ${atlas_field_api} PROPERTIES Fortran_MODULE_DIRECTORY ${CMAKE_BINARY_DIR}/module/atlas_field_api/${prec} )
endforeach()

if( ECBUILD_INSTALL_FORTRAN_MODULES )
  install( DIRECTORY ${CMAKE_BINARY_DIR}/module/atlas_field_api/
           DESTINATION module/atlas_field_api
           COMPONENT modules )
endif()
