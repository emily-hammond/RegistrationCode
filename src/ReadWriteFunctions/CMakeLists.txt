set(ReadWriteFunctions_SRC ReadWriteFunctions.cxx)

add_library(ReadWriteFunctionsLib SHARED ${ReadWriteFunctions_SRC})
set_target_properties(ReadWriteFunctionsLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(ReadWriteFunctionsLib ${ITK_LIBRARIES})

add_executable(ReadWriteFunctions ${ReadWriteFunctions_SRC})
target_link_libraries(ReadWriteFunctions ${ITK_LIBRARIES})