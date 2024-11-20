

add_executable(convertTerrainToMapboxRGB convertTerrainToMapboxRGB_main.cpp)
target_compile_features(convertTerrainToMapboxRGB PUBLIC cxx_std_17)
target_link_libraries(convertTerrainToMapboxRGB PRIVATE libplateauterrainrgb)

add_executable(terrain_tiles2mapbox_rgb terrain_tiles2mapbox_rgb.cpp)
target_compile_features(terrain_tiles2mapbox_rgb PUBLIC cxx_std_17)
find_package(TIFF REQUIRED)
if (NOT TIFF_FOUND)
	message(TIFF_LIBRARIES=${TIFF_LIBRARIES})
	message(TIFF_INCLUDE_DIRES=${TIFF_INCLUDE_DIRS})
	message(SEND_ERROR "libtiff was not found.")
endif()
find_package(PNG REQUIRED)
if (NOT PNG_FOUND)
  message(PNG_LIBRARIES=${PNG_LIBRARIES})
  message(PNG_INCLUDE_DIRS=${PNG_INCLUDE_DIRS})
  message(SEND_ERROR "libpng was not found.")
endif()
target_link_libraries(terrain_tiles2mapbox_rgb PRIVATE ${TIFF_LIBRARIES})
target_include_directories(terrain_tiles2mapbox_rgb PRIVATE ${TIFF_INCLUDE_DIRS})

if (UNIX)
	set_target_properties(convertTerrainToMapboxRGB 
		PROPERTIES INSTALL_RPATH
		${CMAKE_INSTALL_PREFIX}/lib)
	set_target_properties(terrain_tiles2mapbox_rgb 
		PROPERTIES INSTALL_RPATH
		${CMAKE_INSTALL_PREFIX}/lib)
endif()

install(TARGETS convertTerrainToMapboxRGB RUNTIME DESTINATION bin)
install(TARGETS terrain_tiles2mapbox_rgb RUNTIME DESTINATION bin)

