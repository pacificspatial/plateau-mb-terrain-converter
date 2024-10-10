
cmake_minimum_required (VERSION 3.8)

set(lib_SRC
plateau-mb-terrain-converter.cpp
WTMCalculator.cpp
"WebTileManager.cpp"
 "CityGMLManager.cpp")

set(lib_HEADERS
plateau-mb-terrain-converter.h
WTMCalculator.h
"WebTileManager.h"
CommonStruct.h
 "CityGMLManager.h")

add_library (plateau-mb-terrain-converter SHARED ${lib_SRC} ${lib_HEADERS})

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

find_package(GDAL REQUIRED)
if (NOT GDAL_FOUND)
  message(SEND_ERROR "Gdal was not found.")
endif()

target_compile_features(plateau-mb-terrain-converter PRIVATE cxx_std_17)
target_include_directories(plateau-mb-terrain-converter PRIVATE ${GDAL_INCLUDE_DIR} ${SQLite3_INCLUDE_DIRS} ${PNG_INCLUDE_DIR})
target_include_directories(plateau-mb-terrain-converter PUBLIC 
    $<BUILD_INTERFACE:${plateau-mb-terrain-converter_SOURCE_DIR}/plateau-mb-terrain-converter>
    $<INSTALL_INTERFACE:include>)
#message(WARNING ${plateau-mb-terrain-converter_SOURCE_DIR}/plateau-mb-terrain-converter)
target_link_libraries(plateau-mb-terrain-converter ${GDAL_LIBRARY} ${SQLite3_LIBRARIES} ${PNG_LIBRARY})

set_target_properties(plateau-mb-terrain-converter PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER plateau-mb-terrain-converter.h)

if(BUILD_SHARED_LIBS)
  if(MSVC)
    target_compile_definitions(plateau-mb-terrain-converter PRIVATE PMTC_DLL_EXPORT=1)
  endif()
else()
  target_compile_definitions(plateau-mb-terrain-converter PUBLIC PMTC_DLL_EXPORT=)
endif()

add_library(${PROJECT_NAMESPACE}::plateau-mb-terrain-converter ALIAS plateau-mb-terrain-converter)

#if (UNIX)
#    get_filename_component(PNG_LIBDIRS ${PNG_LIBRARIES} PATH)
#    set(LIBPMTC_RPATH ${LIBPMTC_RPATH} ${PNG_LIBDIRS})
#    get_filename_component(SQLite3_LIBDIRS ${SQLite3_LIBRARIES} PATH)
#    set(LIBPMTC_RPATH ${LIBPMTC_RPATH} ${SQLite3_LIBDIRS})
#    get_filename_component(GDAL_LIBDIRS ${GDAL_LIBRARY} PATH)
#    set(LIBPMTC_RPATH ${LIBPMTC_RPATH} ${GDAL_LIBDIRS})
#    set_target_properties(
#        plateau-mb-terrain-converter PROPERTIES
#        INSTALL_RPATH "${LIBPMTC_RPATH}"
#    )
#endif()


include(GNUInstallDirs)

install(TARGETS plateau-mb-terrain-converter 
    EXPORT ${PACKAGE_NAME}Targets
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT ${PACKAGE_NAME}Targets
    FILE ${PACKAGE_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAMESPACE}::
    DESTINATION cmake)
