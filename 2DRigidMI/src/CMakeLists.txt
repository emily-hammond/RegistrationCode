# This CMAKELISTS.TXT file was compied from the ITK software guide (v4.5) on page 49.
# Modified by EMH: 3/4/2014

cmake_minimum_required(VERSION 2.8)
# required for Visual Studio
project (2DRigidMI)

# loads a CMake file with a strategy for finding ITK. If fails, prompting for the directory with ITK is will ensue
find_package(ITK REQUIRED)
# loads the UseITK.cmake file to perform all the necessary configuration
include(${ITK_USE_FILE})

# defines as its the first argument the name of hte executable will be produced, remaining arguments are the names of the source files to be complied and linked
add_executable(2DRigidMI 2DRigidMIExample.cxx)

# specified which ITK libraries will be linked with this project
target_link_libraries(2DRigidMI ${ITK_LIBRARIES})
