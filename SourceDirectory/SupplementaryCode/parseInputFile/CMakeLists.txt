set(parseInputFile_SRC parseInputFile.cxx)

add_library(parseInputFileLib SHARED ${parseInputFile_SRC})
set_target_properties(parseInputFileLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(parseInputFileLib ${ITK_LIBRARIES} ${VTK_LIBRARIES})

add_executable(parseInputFile ${parseInputFile_SRC})
target_link_libraries(parseInputFile ${ITK_LIBRARIES} ${VTK_LIBRARIES})