

add_executable(convertTerrainToMapboxRGB convertTerrainToMapboxRGB_main.cpp)
target_compile_features(convertTerrainToMapboxRGB PUBLIC cxx_std_17)
target_link_libraries(convertTerrainToMapboxRGB PRIVATE libplateauterrainrgb)

add_executable(merge_tilesets merge_tilesets.cpp)
target_compile_features(merge_tilesets PUBLIC cxx_std_17)
target_link_libraries(merge_tilesets PRIVATE libplateauterrainrgb)

# find_package(PNG REQUIRED)
# if (NOT PNG_FOUND)
#   message(PNG_LIBRARIES=${PNG_LIBRARIES})
#   message(PNG_INCLUDE_DIRS=${PNG_INCLUDE_DIRS})
#   message(SEND_ERROR "libpng was not found.")
# endif()

target_link_libraries(merge_tilesets PRIVATE ${PNG_LIBRARIES})
target_include_directories(merge_tilesets PRIVATE ${PNG_INCLUDE_DIRS})

if (UNIX)
	set_target_properties(convertTerrainToMapboxRGB 
		PROPERTIES INSTALL_RPATH
		${CMAKE_INSTALL_PREFIX}/lib)
	# set_target_properties(terrain_tiles2mapbox_rgb 
	# 	PROPERTIES INSTALL_RPATH
	# 	${CMAKE_INSTALL_PREFIX}/lib)
endif()

install(TARGETS convertTerrainToMapboxRGB RUNTIME DESTINATION bin)
install(TARGETS merge_tilesets RUNTIME DESTINATION bin)

