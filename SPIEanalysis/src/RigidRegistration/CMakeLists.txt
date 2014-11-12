set(IsolateSkeleton_SRC IsolateSkeleton.cxx)

add_library(IsolateSkeletonLib SHARED ${IsolateSkeleton_SRC})
set_target_properties(IsolateSkeletonLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(IsolateSkeletonLib ${ITK_LIBRARIES})

add_executable(IsolateSkeleton ${IsolateSkeleton_SRC})
target_link_libraries(IsolateSkeleton ${ITK_LIBRARIES})