set(ResampleAndCropImages_SRC ResampleAndCropImages.cxx)

add_library(ResampleAndCropImagesLib SHARED ${ResampleAndCropImages_SRC})
set_target_properties(ResampleAndCropImagesLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries(ResampleAndCropImagesLib ${ITK_LIBRARIES})

add_executable(ResampleAndCropImages ${ResampleAndCropImages_SRC})
target_link_libraries(ResampleAndCropImages ${ITK_LIBRARIES})