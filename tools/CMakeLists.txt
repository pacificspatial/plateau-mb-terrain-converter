#################################################################################
#
# plateau-mb-terrain-converter tools build configuration file for CMake build system
#
# (c) 2025 MLIT Japan.
# (c) 2025 Pacific Spatial Solutions Inc.
#
# Author : Yamate, N
# 
#################################################################################


add_executable(convertTerrainToMapboxRGB convertTerrainToMapboxRGB_main.cpp)
target_compile_features(convertTerrainToMapboxRGB PUBLIC cxx_std_17)
target_link_libraries(convertTerrainToMapboxRGB PRIVATE libplateauterrainrgb)

add_executable(convertGsiDemToMapboxRGB convertGsiDemToMapboxRGB_main.cpp)
target_compile_features(convertGsiDemToMapboxRGB PUBLIC cxx_std_17)
target_link_libraries(convertGsiDemToMapboxRGB PRIVATE libplateauterrainrgb)

add_executable(merge_tilesets merge_tilesets.cpp)
target_compile_features(merge_tilesets PUBLIC cxx_std_17)
target_link_libraries(merge_tilesets PRIVATE libplateauterrainrgb)
target_link_libraries(merge_tilesets PRIVATE ${PNG_LIBRARIES})
target_include_directories(merge_tilesets PRIVATE ${PNG_INCLUDE_DIRS})

add_executable(fill_zero_tileset fill_zero_tileset.cpp)
target_compile_features(fill_zero_tileset PUBLIC cxx_std_17)
target_link_libraries(fill_zero_tileset PRIVATE libplateauterrainrgb)
target_link_libraries(fill_zero_tileset PRIVATE ${PNG_LIBRARIES})
target_include_directories(fill_zero_tileset PRIVATE ${PNG_INCLUDE_DIRS})

add_executable(plateau2gtif plateau2gtif.cpp)
target_compile_features(plateau2gtif PUBLIC cxx_std_17)
target_link_libraries(plateau2gtif PRIVATE libplateauterrainrgb)
target_link_libraries(plateau2gtif PRIVATE ${GDAL_LIBRARY})
target_include_directories(plateau2gtif PRIVATE ${GDAL_INCLUDE_DIR})

if (UNIX)
    set_target_properties(convertTerrainToMapboxRGB 
        PROPERTIES INSTALL_RPATH
        ${CMAKE_INSTALL_PREFIX}/lib)
    set_target_properties(convertGsiDemToMapboxRGB
        PROPERTIES INSTALL_RPATH
        ${CMAKE_INSTALL_PREFIX}/lib)
    set_target_properties(merge_tilesets 
        PROPERTIES INSTALL_RPATH
         ${CMAKE_INSTALL_PREFIX}/lib)
    set_target_properties(fill_zero_tileset 
        PROPERTIES INSTALL_RPATH
        ${CMAKE_INSTALL_PREFIX}/lib)
    set_target_properties(plateau2gtif 
        PROPERTIES INSTALL_RPATH
        ${CMAKE_INSTALL_PREFIX}/lib)

    if (APPLE)
        set_target_properties(convertTerrainToMapboxRGB
            PROPERTIES
            INSTALL_NAME_DIR "@rpath" BUILD_WITH_RPATH ON)
        set_target_properties(convertGsiDemToMapboxRGB
            PROPERTIES
            INSTALL_NAME_DIR "@rpath" BUILD_WITH_RPATH ON)
        set_target_properties(merge_tilesets
            PROPERTIES
            INSTALL_NAME_DIR "@rpath" BUILD_WITH_RPATH ON)
        set_target_properties(fill_zero_tileset
            PROPERTIES
            INSTALL_NAME_DIR "@rpath" BUILD_WITH_RPATH ON)
        set_target_properties(plateau2gtif
            PROPERTIES
            INSTALL_NAME_DIR "@rpath" BUILD_WITH_RPATH ON)
    endif()
endif()

install(TARGETS convertTerrainToMapboxRGB RUNTIME DESTINATION bin)
install(TARGETS merge_tilesets RUNTIME DESTINATION bin)
install(TARGETS fill_zero_tileset RUNTIME DESTINATION bin)
install(TARGETS convertGsiDemToMapboxRGB RUNTIME DESTINATION bin)
install(TARGETS plateau2gtif RUNTIME DESTINATION bin)

