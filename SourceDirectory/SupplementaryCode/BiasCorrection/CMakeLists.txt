set(BiasCorrection_SRC itkN4BiasFieldCorrectionImageFilterTest.cxx)

add_library(BiasCorrectionLib SHARED ${BiasCorrection_SRC})
set_target_properties(BiasCorrectionLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(BiasCorrectionLib ${ITK_LIBRARIES})

add_executable(BiasCorrection ${BiasCorrection_SRC})
target_link_libraries(BiasCorrection ${ITK_LIBRARIES})