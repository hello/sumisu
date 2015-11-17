# CMake Script for sumisu

##############################################
# Project 
##############################################
project(sumisu C)
cmake_minimum_required(VERSION 2.8.12)
# Ensures that we do an out of source build

##############################################
# Paths, Environment and Properperties
##############################################
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_DIR})
set(CMAKE_LIBRARY_PATH ${PROJECT_SOURCE_DIR}/lib)

set(TARGET_APP test)
set(BUILD_OUTPUT_NAME "${TARGET_APP}-${TARGET_PLATFORM}")

##############################################
# Tools Path
##############################################
#gcc, jlink, etc
add_subdirectory(${PROJECT_SOURCE_DIR}/tools/)

##############################################
# Includes
##############################################
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include/os")
include_directories("${PROJECT_SOURCE_DIR}/include/misc")

##############################################
# Platform Sources
##############################################
set(PLATFORM_DIRECTORY ${PROJECT_SOURCE_DIR}/platform/${TARGET_PLATFORM})
add_subdirectory(${PLATFORM_DIRECTORY})

##############################################
# App Sources
##############################################
add_subdirectory(${PROJECT_SOURCE_DIR}/app/${TARGET_APP})

##############################################
# Generate Linker Flags From Platform
##############################################
get_directory_property(CMAKE_EXE_LINKER_FLAGS DIRECTORY ${PLATFORM_DIRECTORY} DEFINITION LINKER_FLAGS)
string(REGEX REPLACE ";" " " CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" CACHE STRING "")

##############################################
# Build
##############################################
add_executable(sumisu $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:app>)
set_target_properties(sumisu
    PROPERTIES
    OUTPUT_NAME "${BUILD_OUTPUT_NAME}"
    )

##############################################
# Post Build
##############################################
set(POST_BUILD_COMMAND
    "touch ${EXECUTABLE_OUTPUT_PATH}/test.hex")
#get_directory_property(POST_BUILD_COMMAND DIRECTORY ${PLATFORM_DIRECTORY} DEFINITION POST_BUILD_COMMAND)
#add_custom_command(TARGET sumisu POST_BUILD COMMAND "${POST_BUILD_COMMAND}")
add_custom_command(TARGET sumisu POST_BUILD COMMAND touch ${EXECUTABLE_OUTPUT_PATH}/test.hex)

