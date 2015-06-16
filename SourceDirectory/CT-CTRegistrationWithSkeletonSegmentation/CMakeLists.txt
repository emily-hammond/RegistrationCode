cmake_minimum_required(VERSION 2.8.9)

project(SPIEAnalysis)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_subdirectory(IsolateSkeleton)
add_subdirectory(RigidRegistration)