cmake_minimum_required(VERSION 2.8.9)

project(MultiLevelRegistration)

find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

find_package(SlicerExecutionModel REQUIRED GenerateCLP)
include(${GenerateCLP_USE_FILE})

add_subdirectory(Validation)
add_subdirectory(MultiLevelRegistration)
add_subdirectory(CalculateDisplacement)
