set(calculateDisplacement_SRC calculateDisplacement.cxx)

add_library(calculateDisplacementLib SHARED ${calculateDisplacement_SRC})
set_target_properties(calculateDisplacementLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(calculateDisplacementLib ${ITK_LIBRARIES})

add_executable(calculateDisplacement ${calculateDisplacement_SRC})
target_link_libraries(calculateDisplacement ${ITK_LIBRARIES})