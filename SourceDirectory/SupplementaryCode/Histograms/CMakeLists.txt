set(ImageHistogram3_SRC ImageHistogram3.cxx)

add_library(ImageHistogram3Lib SHARED ${ImageHistogram3_SRC})
set_target_properties(ImageHistogram3Lib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(ImageHistogram3Lib ${ITK_LIBRARIES})

add_executable(ImageHistogram3 ${ImageHistogram3_SRC})
target_link_libraries(ImageHistogram3 ${ITK_LIBRARIES})