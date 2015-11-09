# CMake Script for sumisu

##############################################
# Project 
##############################################
project(sumisu)
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
set(TARGET_PLATFORM darwin)

##############################################
# Tools
##############################################
#gcc, jlink, etc
add_subdirectory(${PROJECT_SOURCE_DIR}/tools/)

#change below to switch compiler
set(CC_ROOT ${PROJECT_SOURCE_DIR}/tools/${GCC_PACKAGE}/bin)
set(CC_BIN arm-none-eabi-gcc)
set(CMAKE_CC_COMPILER ${CC_ROOT}/${CC_BIN})

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
