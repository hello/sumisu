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
include_directories("${PROJECT_SOURCE_DIR}/include/service")

##############################################
# Platform Sources
##############################################
set(PLATFORM_ROOT ${PROJECT_SOURCE_DIR}/platform)
set(PLATFORM_DIRECTORY ${PLATFORM_ROOT}/${TARGET_PLATFORM})
add_subdirectory(${PLATFORM_DIRECTORY})

##############################################
# Common Sources
##############################################
add_subdirectory(${PLATFORM_ROOT}/common)

##############################################
# App Sources
##############################################
add_subdirectory(${PROJECT_SOURCE_DIR}/app)

##############################################
# Generate Linker Flags From Platform
##############################################
get_directory_property(CMAKE_EXE_LINKER_FLAGS DIRECTORY ${PLATFORM_DIRECTORY} DEFINITION LINKER_FLAGS)
string(REGEX REPLACE ";" " " CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}" CACHE STRING "")

##############################################
# Build
##############################################
add_executable(sumisu $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:platform_common> $<TARGET_OBJECTS:app>)
set_target_properties(sumisu
    PROPERTIES
    OUTPUT_NAME "${BUILD_OUTPUT_NAME}.elf"
    )

##############################################
# Post Build
##############################################
if(NOT ${OBJCOPY} STREQUAL "")
    add_custom_command(TARGET sumisu POST_BUILD
        COMMAND ${OBJCOPY} -O ihex ${EXECUTABLE_OUTPUT_PATH}/${BUILD_OUTPUT_NAME}.elf ${EXECUTABLE_OUTPUT_PATH}/${BUILD_OUTPUT_NAME}.hex)
endif(NOT ${OBJCOPY} STREQUAL "")

if(NOT ${READELF} STREQUAL "")
    add_custom_command(TARGET sumisu POST_BUILD
        COMMAND ${READELF} -a ${EXECUTABLE_OUTPUT_PATH}/${BUILD_OUTPUT_NAME}.elf > ${EXECUTABLE_OUTPUT_PATH}/${BUILD_OUTPUT_NAME}.readelf)
endif(NOT ${READELF} STREQUAL "")

