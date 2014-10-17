cmake_minimum_required(VERSION 2.8)
project (Code)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_executable(main main.cxx)

target_link_libraries(main ${ITK_LIBRARIES})

