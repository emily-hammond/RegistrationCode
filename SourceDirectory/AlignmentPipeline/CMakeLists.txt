cmake_minimum_required(VERSION 2.8.9)

project(main)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(main_SRC main.cxx)

add_library(mainLib SHARED ${main_SRC})
set_target_properties(mainLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(mainLib ${ITK_LIBRARIES})

add_executable(main ${main_SRC})
target_link_libraries(main ${ITK_LIBRARIES})