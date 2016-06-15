# cmake_minimum_required(VERSION 2.8.9)

# project(MultiLevelRegistration)
# find_package(ITK REQUIRED)
# include(${ITK_USE_FILE})

# set(MultiLevelRegistration_SRC MultiLevelRegistration.cxx)

# find_package(SlicerExecutionModel REQUIRED GenerateCLP)
# include(${GenerateCLP_USE_FILE})

# GENERATECLP(MultiLevelRegistration_SRC MultiLevelRegistration.xml)

# add_executable(MultiLevelRegistration ${MultiLevelRegistration_SRC})
# target_link_libraries(MultiLevelRegistration ${ITK_LIBRARIES})

#-----------------------------------------------------------------------------
# set(MultiLevelRegistration_SRC MultiLevelRegistration.cxx)
# generateclp(MultiLevelRegistration_SRC MultiLevelRegistration.xml)

# Build Slicer3 Shared Library module
# add_library(MultiLevelRegistrationLib SHARED ${MultiLevelRegistration_SRC})
# set_target_properties (MultiLevelRegistrationLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
# target_link_libraries (MultiLevelRegistrationLib ${ITK_LIBRARIES})

# Build stand-alone command line module.
# add_executable(MultiLevelRegistration ${MultiLevelRegistration_SRC})
# target_link_libraries(MultiLevelRegistration ${ITK_LIBRARIES})

#-----------------------------------------------------------------------------
# set(MODULE_NAME MultiLevelRegistration)


# SlicerExecutionModel

find_package(SlicerExecutionModel REQUIRED)
include(${GenerateCLP_USE_FILE})

# set(MultiLevelRegistration_SOURCE MultiLevelRegistration.cxx)
# GENERATECLP(MultiLevelRegistration_SOURCE MultiLevelRegistration.xml)
 
# add_executable(MultiLevelRegistration ${MultiLevelRegistration_SOURCE})
# target_link_libraries(MultiLevelRegistration ${ITK_LIBRARIES})


#ITK

set(${PROJECT_NAME}_ITK_COMPONENTS
  ITKCommon
  ITKIOImageBase
  ITKIOTransformBase
  ITKImageFunction
  ITKImageGrid
  ITKImageIntensity
  ITKOptimizers
  ITKRegistrationCommon
  ITKSpatialObjects
  ITKStatistics
  ITKTestKernel
  ITKTransform
  )
find_package(ITK 4.6 COMPONENTS ${${PROJECT_NAME}_ITK_COMPONENTS} REQUIRED)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER 1) # See Libs/ITKFactoryRegistration/CMakeLists.txt
include(${ITK_USE_FILE})

# #-----------------------------------------------------------------------------
SEMMacroBuildCLI(
  NAME MultiLevelRegistration
  LOGO_HEADER C:/Slicer/Slicer/Resources/ITKLogo.h
  TARGET_LIBRARIES ${ITK_LIBRARIES}
  )
