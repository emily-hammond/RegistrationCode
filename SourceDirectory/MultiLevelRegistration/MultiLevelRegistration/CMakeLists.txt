set(MultiLevelRegistration_SRC main.cxx)
GENERATECLP(MultiLevelRegistration_SRC main.xml)

SEMMacroBuildCLI(
   NAME main
   LOGO_HEADER ${Slicer_SOURCE_DIR}/Resources/NAMICLogo.h
   ADDITIONAL_SRCS
     main.cxx
   TARGET_LIBRARIES ModuleDescriptionParser ${ITK_LIBRARIES} vtkTeem MRMLCore SlicerBaseCLI ${VTK_LIBRARIES}
   INCLUDE_DIRECTORIES
     ${vtkTeem_INCLUDE_DIRS}
     ${MRMLCore_INCLUDE_DIRS}
     ${vtkITK_INCLUDE_DIRS}
     ${SlicerBaseCLI_SOURCE_DIR} ${SlicerBaseCLI_BINARY_DIR}
   )

add_library(MultiLevelRegistrationLib SHARED ${MultiLevelRegistration_SRC})
set_target_properties(MultiLevelRegistrationLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(MultiLevelRegistrationLib ${ITK_LIBRARIES})

add_executable(MultiLevelRegistration ${MultiLevelRegistration_SRC})
target_link_libraries(MultiLevelRegistration ${ITK_LIBRARIES})