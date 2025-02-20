#################################################################################
#
# plateau-mb-terrain-converter build configuration file for CMake build system
#
# (c) 2025 MLIT Japan.
# (c) 2025 Pacific Spatial Solutions Inc.
#
# Author : Yamate, N
# 
#################################################################################

cmake_minimum_required (VERSION 3.8)
cmake_policy(SET CMP0078 NEW)
cmake_policy(SET CMP0086 NEW)

project (plateau-mb-terrain-converter
	VERSION 1.0.0
	DESCRIPTION "plateau mapbox terrain converter"
	LANGUAGES CXX
	)
set(PROJECT_NAMESPACE pmtc)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set(PACKAGE_NAME libPMTC)

message(${plateau-mb-terrain-converter_SOURCE_DIR})

set(PMTC_SOURCE_DIR plateau-mb-terrain-converter_SOURCE_DIR)

add_subdirectory ("libplateauterrainrgb")
add_subdirectory ("tools")
add_subdirectory ("swig")
