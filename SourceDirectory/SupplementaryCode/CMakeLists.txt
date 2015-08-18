cmake_minimum_required(VERSION 2.8.9)

project(supplementaryCode)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_subdirectory(BiasCorrection)
add_subdirectory(investigateFiducials)
add_subdirectory(ReadWriteFunctions)
add_subdirectory(Histograms)
# add_subdirectory(transformFiducials)
add_subdirectory(parseInputFile)