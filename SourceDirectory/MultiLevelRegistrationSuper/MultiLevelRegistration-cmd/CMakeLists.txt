cmake_minimum_required(VERSION 2.8.9)

project(MultiLevelRegistration-cmd)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(MultiLevelRegistration_SRC MultiLevelRegistration.cxx)

add_library(MultiLevelRegistrationLib SHARED ${MultiLevelRegistration_SRC})
set_target_properties(MultiLevelRegistrationLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(MultiLevelRegistrationLib ${ITK_LIBRARIES})

add_executable(MultiLevelRegistration ${MultiLevelRegistration_SRC})
target_link_libraries(MultiLevelRegistration ${ITK_LIBRARIES})

add_subdirectory(Validation)