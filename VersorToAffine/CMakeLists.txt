cmake_minimum_required(VERSION 2.8)
project (FinalProject)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(VersorToAffine.exe VersorToAffine.cxx)

target_link_libraries(VersorToAffine.exe ${ITK_LIBRARIES})
