cmake_minimum_required(VERSION 2.8)
project (FiducialCode)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(investigateFiducials.exe investigateFiducials.cxx)

target_link_libraries(investigateFiducials.exe ${ITK_LIBRARIES})

