cmake_minimum_required(VERSION 2.8)
project (LabelMapOverlapModule)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(LabelMapOverlapModule_SRC LabelMapOverlapModule.cxx)

add_library(LabelMapOverlapModuleLib SHARED ${LabelMapOverlapModule_SRC})
set_target_properties(LabelMapOverlapModuleLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(LabelMapOverlapModuleLib ${ITK_LIBRARIES})

add_executable(LabelMapOverlapModule ${LabelMapOverlapModule_SRC})
target_link_libraries(LabelMapOverlapModule ${ITK_LIBRARIES})