cmake_minimum_required(VERSION 2.8.9)

project(MultiLevelRegistration)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_subdirectory(Validation)
add_subdirectory(MultiLevelRegistration)
add_subdirectory(CalculateDisplacement)
