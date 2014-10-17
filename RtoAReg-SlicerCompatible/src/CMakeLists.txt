set(RigidToAffineRegistration_SRC RigidToAffineRegistration.cxx)
generateclp(RigidToAffineRegistration_SRC RigidToAffineRegistration.xml)

## Build Slicer3 Shared Library module
add_library(${HAWKID}RigidToAffineRegistrationLib SHARED ${RigidToAffineRegistration_SRC})
set_target_properties (${HAWKID}RigidToAffineRegistrationLib PROPERTIES COMPILE_FLAGS "-Dmain=ModuleEntryPoint")
target_link_libraries (${HAWKID}RigidToAffineRegistrationLib ${ITK_LIBRARIES})

## Build stand-alone command line module.
add_executable(${HAWKID}RigidToAffineRegistration ${RigidToAffineRegistration_SRC})
target_link_libraries(${HAWKID}RigidToAffineRegistration ${ITK_LIBRARIES})
