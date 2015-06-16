set(transformFiducials_SRC transformFiducials.cxx)

add_library(transformFiducialsLib SHARED ${transformFiducials_SRC})
set_target_properties(transformFiducialsLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(transformFiducialsLib ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(transformFiducials ${transformFiducials_SRC})
target_link_libraries(transformFiducials ${ITK_LIBRARIES} ${VTK_LIBRARIES})