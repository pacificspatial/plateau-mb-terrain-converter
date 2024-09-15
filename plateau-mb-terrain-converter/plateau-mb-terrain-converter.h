﻿// plateau-mb-terrain-converter.h : 標準のシステム インクルード ファイル用のインクルード ファイル、
// または、プロジェクト専用のインクルード ファイル。

#pragma once


#include <string>
#include <memory>
#include <functional>


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


class PMTC_DLL PlateauMapboxTerrainConverter
{
public:
    PlateauMapboxTerrainConverter() = delete;
    PlateauMapboxTerrainConverter(
        const std::string &strInputTerrainCityGML, 
        const std::string &strOutputTileDirectory, 
        const int nMinZoomLevel, 
        const int nMaxZoomLevel,
        const std::function<void(std::string)> &fnMessageFeedback = nullptr,
        const std::function<void(int)> &fnProgressFeedback = nullptr
    );
    virtual ~PlateauMapboxTerrainConverter();

    inline bool isValid() const { return mbValid; }

    void createTileset();

private:
    std::string mstrInputTerrainCityGML;
    std::string mstrTileDirectory;
    int mnMinZoomLevel;
    int mnMaxZoomLevel;
    bool mbValid;

    std::unique_ptr<WTMCalculator> mpWTMCalculator;
    std::unique_ptr<CityGMLManager> mpCityGMLManager;
    std::unique_ptr<WebTileManager> mpWebTileManager;

    std::function<void(std::string)> mfnMessageFeedback;
    std::function<void(int)> mfnProgressFeedback;
};

