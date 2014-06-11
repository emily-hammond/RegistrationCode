cmake_minimum_required(VERSION 2.8)
project (RegistrationCode)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(Affine.exe Affine.cxx)

target_link_libraries(Affine.exe ${ITK_LIBRARIES})
