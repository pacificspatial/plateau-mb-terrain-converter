# CMakeList.txt : 最上位の CMake プロジェクト ファイル。グローバル構成を行います
# また、サブプロジェクトをここに含めます。
#
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

set(PMTC_SOURCE_DIR plateau-mb-terrain-converter_SOURCE_DIR)

add_subdirectory ("plateau-mb-terrain-converter")
add_subdirectory ("convertTerrainToMapboxRGB")
add_subdirectory ("swig")
