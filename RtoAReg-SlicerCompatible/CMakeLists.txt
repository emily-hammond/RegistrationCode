
set(HAWKID ehammond)

cmake_minimum_required(VERSION 2.8.9)

find_package(Slicer COMPONENTS ConfigurePrerequisites)
project(BME186_FinalProject)

find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

find_package(SlicerExecutionModel REQUIRED GenerateCLP)
include(${GenerateCLP_USE_FILE})

# Find the data directory.
# define a new variable BME186_DATA_ROOT that looks for a file in the path (define the environmental variable)
find_path(BME186_DATA_ROOT BME186Data.readme /usr/css/opt/BME186_2014/data $ENV{BME186_DATA_ROOT})
# marks as an advanced option
mark_as_advanced(BME186_DATA_ROOT)

#
add_subdirectory(RegistrationCode)

