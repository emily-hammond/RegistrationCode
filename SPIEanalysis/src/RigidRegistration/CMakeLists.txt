set(RigidRegistration_SRC RigidRegistration.cxx)

add_library(RigidRegistrationLib SHARED ${RigidRegistration_SRC})
set_target_properties(RigidRegistrationLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(RigidRegistrationLib ${ITK_LIBRARIES})

add_executable(RigidRegistration ${RigidRegistration_SRC})
target_link_libraries(RigidRegistration ${ITK_LIBRARIES})