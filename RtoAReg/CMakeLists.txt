cmake_minimum_required(VERSION 2.8)
project (FinalProject)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(RigidToAffineRegistration.exe RigidToAffineRegistration.cxx)

target_link_libraries(RigidToAffineRegistration.exe ${ITK_LIBRARIES})
