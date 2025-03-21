﻿#################################################################################
#
# libplateauterrainrgb build configuration file for CMake build system
#
# (c) 2025 MLIT Japan.
# (c) 2025 Pacific Spatial Solutions Inc.
#
# Author : Yamate, N
# 
#################################################################################

cmake_minimum_required (VERSION 3.8)

set(lib_SRC
"plateau-mb-terrain-converter.cpp"
"WTMCalculator.cpp"
"WebTileManager.cpp"
 "CityGMLReader.cpp"
 "PlateauTileManager.cpp"
 "GsiGmlReader.cpp"
 "GsiTileManager.cpp")

set(lib_HEADERS
"plateau-mb-terrain-converter.h"
"WTMCalculator.h"
"WebTileManager.h"
"CommonStruct.h"
"CityGMLReader.h"
"PlateauTileManager.h"
"GsiGmlReader.h"
"GsiTileManager.h")

add_library (libplateauterrainrgb SHARED ${lib_SRC} ${lib_HEADERS})

find_package(PNG REQUIRED)
if (NOT PNG_FOUND)
  message(PNG_LIBRARIES=${PNG_LIBRARIES})
  message(PNG_INCLUDE_DIRS=${PNG_INCLUDE_DIRS})
  message(SEND_ERROR "libpng was not found.")
endif()

find_package(SQLite3 REQUIRED)
if (NOT SQLite3_FOUND)
  message(SEND_ERROR "SQLite3 was not found.")
endif()

find_package(LibXml2 REQUIRED)
if (NOT LibXml2_FOUND)
  message(SEND_ERROR "libxml2 was not found.")
endif()

find_package(GDAL REQUIRED)
if (NOT GDAL_FOUND)
  message(SEND_ERROR "Gdal was not found.")
endif()

target_compile_features(libplateauterrainrgb PRIVATE cxx_std_17)
target_include_directories(libplateauterrainrgb PRIVATE ${GDAL_INCLUDE_DIR} ${SQLite3_INCLUDE_DIRS} ${PNG_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR})
target_include_directories(libplateauterrainrgb PUBLIC 
    $<BUILD_INTERFACE:${plateau-mb-terrain-converter_SOURCE_DIR}/libplateauterrainrgb>
    $<INSTALL_INTERFACE:include>)
#message(WARNING ${plateau-mb-terrain-converter_SOURCE_DIR}/plateau-mb-terrain-converter)
target_link_libraries(libplateauterrainrgb ${GDAL_LIBRARY} ${SQLite3_LIBRARIES} ${PNG_LIBRARY} ${LIBXML2_LIBRARIES})

set_target_properties(libplateauterrainrgb PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER plateau-mb-terrain-converter.h)

if(BUILD_SHARED_LIBS)
  if(MSVC)
    target_compile_definitions(libplateauterrainrgb PRIVATE PMTC_DLL_EXPORT=1)
  endif()
else()
  target_compile_definitions(libplateauterrainrgb PUBLIC PMTC_DLL_EXPORT=)
endif()

add_library(${PROJECT_NAMESPACE}::libplateauterrainrgb ALIAS libplateauterrainrgb)

if (UNIX)
    set_target_properties(libplateauterrainrgb
        PROPERTIES INSTALL_RPATH
        ${CMAKE_INSTALL_PREFIX}/lib)
    if (APPLE)
        set_target_properties(libplateauterrainrgb
                PROPERTIES
            INSTALL_NAME_DIR "@rpath" BUILD_WITH_RPATH ON)
    endif()
endif()

include(GNUInstallDirs)

install(TARGETS libplateauterrainrgb 
    EXPORT ${PACKAGE_NAME}Targets
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT ${PACKAGE_NAME}Targets
    FILE ${PACKAGE_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAMESPACE}::
    DESTINATION cmake)
