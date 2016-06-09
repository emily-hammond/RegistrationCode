set(Validation_SRC Validation.cxx)

add_library(ValidationLib SHARED ${Validation_SRC})
set_target_properties(ValidationLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(ValidationLib ${ITK_LIBRARIES})

add_executable(Validation ${Validation_SRC})
target_link_libraries(Validation ${ITK_LIBRARIES})