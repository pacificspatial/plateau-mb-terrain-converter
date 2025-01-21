// plateau-mb-terrain-converter.h : 標準のシステム インクルード ファイル用のインクルード ファイル、
// または、プロジェクト専用のインクルード ファイル。

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
    bool PMTC_DLL createPlateauTileset(
        const std::string &strInputTerrainCityGML, 
        const std::string &strOutputTileDirectory, 
        const int nMinZoomLevel, 
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr
    );

    bool PMTC_DLL createGsiTileset(
        const std::string &strInputGsiGml,
        const std::string &strOutputTileDirectory,
        const int nMinZoomLevel,
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr
    );

    void PMTC_DLL mergeTilesets( 
        const std::vector<std::string> &vstrInputDirs,
        const std::string& strOutDir, 
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr 
    );

    void PMTC_DLL fill_zero(
        const std::string &strTileDir,
        const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr 
    );
}
