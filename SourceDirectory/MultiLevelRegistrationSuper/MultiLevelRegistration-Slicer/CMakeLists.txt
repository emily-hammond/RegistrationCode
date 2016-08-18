#-----------------------------------------------------------------------------
set(MODULE_NAME MultiLevelRegistration)

#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.5)
#
# SlicerExecutionModel
#
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

find_package(SlicerExecutionModel REQUIRED)
include(${GenerateCLP_USE_FILE})

#
# ITK
#
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

#-----------------------------------------------------------------------------
SEMMacroBuildCLI(
  NAME ${MODULE_NAME}
  LOGO_HEADER ${Slicer_SOURCE_DIR}/Resources/NAMICLogo.h
  TARGET_LIBRARIES ${ITK_LIBRARIES}
  INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/ITKRegistrationHelper
  )