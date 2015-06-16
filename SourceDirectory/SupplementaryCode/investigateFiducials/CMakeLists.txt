set(investigateFiducials_SRC investigateFiducials.cxx)

add_library(investigateFiducialsLib SHARED ${investigateFiducials_SRC})
set_target_properties(investigateFiducialsLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(investigateFiducialsLib ${ITK_LIBRARIES})

add_executable(investigateFiducials ${investigateFiducials_SRC})
target_link_libraries(investigateFiducials ${ITK_LIBRARIES})