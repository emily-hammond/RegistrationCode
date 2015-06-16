cmake_minimum_required(VERSION 2.8.9)

find_package(Slicer COMPONENTS ConfigurePrerequisites)

if(NOT Slicer_DIR)
  
message(FATAL_ERROR "Slicer_DIR must be defined before continuing")
  
return()

endif()

project(supplementaryCode)

find_package(Slicer REQUIRED)

include(${Slicer_USE_FILE})



find_package(SlicerExecutionModel REQUIRED GenerateCLP)

include(${GenerateCLP_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(RigidToAffineRegistration_SRC RigidToAffineRegistration.cxx)
generateclp(RigidToAffineRegistration_SRC RigidToAffineRegistration.xml)

## Build Slicer3 Shared Library module
add_library(${HAWKID}RigidToAffineRegistrationLib SHARED ${RigidToAffineRegistration_SRC})
set_target_properties (${HAWKID}RigidToAffineRegistrationLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries (${HAWKID}RigidToAffineRegistrationLib ${ITK_LIBRARIES})

## Build stand-alone command line module.
add_executable(${HAWKID}RigidToAffineRegistration ${RigidToAffineRegistration_SRC})
target_link_libraries(${HAWKID}RigidToAffineRegistration ${ITK_LIBRARIES})
