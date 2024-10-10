

add_executable(convertTerrainToMapboxRGB convertTerrainToMapboxRGB_main.cpp)
target_compile_features(convertTerrainToMapboxRGB PUBLIC cxx_std_17)
target_link_libraries(convertTerrainToMapboxRGB PRIVATE plateau-mb-terrain-converter)

if (UNIX)
	set_target_properties(convertTerrainToMapboxRGB 
		PROPERTIES INSTALL_RPATH
		${CMAKE_INSTALL_PREFIX}/lib)
endif()

install(TARGETS convertTerrainToMapboxRGB RUNTIME DESTINATION bin)
