/***************************************************************************
               plateau-mb-terrain-converter.h  -  description
                            -------------------

    libplateauterrainrgb entry point.

    begin                : Jan. 21, 2025
    Copyright            : (c) 2025 MLIT Japan.
                         : (c) 2025 Pacific Spatial Solutions Inc.
    author               : Yamate, N
 ***************************************************************************/

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <iostream>
#include <filesystem>

#ifndef PMTC_DLL
#if defined(_MSC_VER)
#ifdef PMTC_DLL_EXPORT
#define PMTC_DLL __declspec(dllexport)
#else
#define PMTC_DLL __declspec(dllimport)
#endif
#elif defined(__GNUC__)
#define PMTC_DLL __attribute__((visibility("default")))
#else
#define PMTC_DLL
#endif
#endif

class WTMCalculator;
class CityGMLManager;
class WebTileManager;

enum MESSAGE_STATUS
{
    MESSAGE_INFO,
    MESSAGE_WARNING,
    MESSAGE_ERROR
};

namespace pmtc
{
    /**
     * @brief  Create Mapbox-TerrainRGB tilesets from PLATEAU CityGML (TIN) data file.
     *
     * @param strInputTerrainCityGML input PLATEAU DEM file (*.gml)
     * @param strOutputTileDirectory output directory
     * @param nMinZoomLevel minimum zoom level
     * @param nMaxZoomLevel maximum zoom level
     * @param bOverwrite overwrite existing tileset
     * @param fnMessageFeedback callback function that tells the process messages.
     * @param fnProgressFeedback callback function that tells the progress number.
     * @return True : processs finished successfully. False : error occured in process.
     */
    bool PMTC_DLL createPlateauTileset(
        const std::string &strInputTerrainCityGML,
        const std::string &strOutputTileDirectory,
        const int nMinZoomLevel,
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr);

    /**
     * @brief  Create Mapbox-TerrainRGB tilesets from GSI DEM data file.
     *
     * @param strInputGsiGml input GSI DEM file (*.gml)
     * @param strOutputTileDirectory output directory
     * @param nMinZoomLevel minimum zoom level
     * @param nMaxZoomLevel maximum zoom level
     * @param bOverwrite overwrite existing tileset
     * @param fnMessageFeedback callback function that tells the process messages.
     * @param fnProgressFeedback callback function that tells the progress number.
     * @return True : processs finished successfully. False : error occured in process.
     */
    bool PMTC_DLL createGsiTileset(
        const std::string &strInputGsiGml,
        const std::string &strOutputTileDirectory,
        const int nMinZoomLevel,
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr);

    /**
     * @brief  Merge multiple tilesets of Mapbox-TerrainRGB.
     *
     * @param vstrInputDirs input directories of tilesets
     * @param strOutDir output directory
     * @param bOverwrite True : the later directory takes precedence.
     * @param fnMessageFeedback callback function that tells the process messages.
     * @param fnProgressFeedback callback function that tells the progress number.
     */
    void PMTC_DLL mergeTilesets(
        const std::vector<std::string> &vstrInputDirs,
        const std::string &strOutDir,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr);

    /**
     * @brief replace NoData pixels with 0m.
     *
     * @param strTileDir input directories of tilesets. This function overwrites existing tile images.
     * @param fnMessageFeedback callback function that tells the process messages.
     * @param fnProgressFeedback callback function that tells the progress number.
     */
    void PMTC_DLL fill_zero(
        const std::string &strTileDir,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr);

    /**
     * @brief convert plateau dem (TIN) to geotiff.
     * 
     * @param strInputTerrainCityGML input PLATEAU DEM file (*.gml)
     * @param strOutputGTif output geotiff file name (*.tif)
     * @param dResolutionLon longitude resolution in output geotiff.
     * @param dResolutionLat latitude resolution in output geotiff.
     * @param fnMessageFeedback callback function that tells the process messages.
     * @param fnProgressFeedback callback function that tells the progress number.
     * 
     * @return True : processs finished successfully. False : error occured in process.
     */
    bool PMTC_DLL terrain2gtif(
        const std::string &strInputTerrainCityGML,
        const std::string &strOutputGTif,
        double dResolutionLon,
        double dResolutionLat,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr
    );
}
