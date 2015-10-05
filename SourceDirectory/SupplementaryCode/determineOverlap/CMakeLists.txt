set(determineOverlap_SRC determineOverlap.cxx)

add_library(determineOverlapLib SHARED ${determineOverlap_SRC})
set_target_properties(determineOverlapLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(determineOverlapLib ${ITK_LIBRARIES})

add_executable(determineOverlap ${determineOverlap_SRC})
target_link_libraries(determineOverlap ${ITK_LIBRARIES})