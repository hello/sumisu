# CMake Script for sumisu

##############################################
# Project 
##############################################
project(sumisu C)
cmake_minimum_required(VERSION 2.8)
# Ensures that we do an out of source build

##############################################
# Paths and Target
##############################################
set(CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_LIBRARY_DIR})

set(TARGET_APP test)

##############################################
# Tools
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
add_subdirectory(${PROJECT_SOURCE_DIR}/platform/${TARGET_PLATFORM})

##############################################
# App Sources
##############################################
add_subdirectory(${PROJECT_SOURCE_DIR}/app/${TARGET_APP})
