/***************************************************************************
              plateau-mb-terrain-converter.cpp  -  description
                            -------------------

    libplateauterrainrgb entry point.

    begin                : Jan. 21, 2025
    Copyright            : (c) 2025 MLIT Japan.
                         : (c) 2025 Pacific Spatial Solutions Inc.
    author               : Yamate, N
 ***************************************************************************/

#include "plateau-mb-terrain-converter.h"
#include "WTMCalculator.h"
#include "CityGMLReader.h"
#include "GsiTileManager.h"
#include "GsiGmlReader.h"
#include "PlateauTileManager.h"

#include <gdal_priv.h>
#include <stdexcept>
#include <filesystem>
#include <regex>

namespace pmtc
{
    bool createPlateauTileset(
        const std::string &strInputTerrainCityGML,
        const std::string &strOutputTileDirectory,
        const int nMinZoomLevel,
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback,
        const std::function<void(int)> &fnProgressFeedback)
    {
        std::unique_ptr<WTMCalculator> pWTMCalculator;

        auto pWebTileManager = std::make_unique<PlateauTileManager>(strOutputTileDirectory, nMinZoomLevel, nMaxZoomLevel, bOverwrite, fnMessageFeedback, fnProgressFeedback);
        if (!pWebTileManager->isValid())
        {
            return false;
        }

        if (fnMessageFeedback)
        {
            fnMessageFeedback(MESSAGE_INFO, "reading CityGML ...");
        }
        auto pCityGMLManager = std::make_unique<CityGMLReader>(strInputTerrainCityGML);
        if (!pCityGMLManager->isValid())
        {
            fnMessageFeedback(MESSAGE_ERROR, pCityGMLManager->getLastError());
            return false;
        }

        auto pInputSpatialRef = pCityGMLManager->getSpatialRef();
        if (pInputSpatialRef)
        {
            pWTMCalculator = std::make_unique<WTMCalculator>(pInputSpatialRef, TILE_PIXELS, nMaxZoomLevel, WTMCalculator::MAPBOX_RGB);
        }
        else
        {
            std::cout << "srs is not defined.. " << std::endl;
            OGRSpatialReference oSrs;
            oSrs.importFromEPSG(4612);
            oSrs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);

            pWTMCalculator = std::make_unique<WTMCalculator>(&oSrs, TILE_PIXELS, nMaxZoomLevel, WTMCalculator::MAPBOX_RGB);
        }

        ///----------------------------------------
        OGRPoint p1, p2, p3;

        if (fnMessageFeedback)
        {
            fnMessageFeedback(MESSAGE_INFO, "calculating grid height in triangles ...");
        }

        int nProcessedTriangle = 0;
        while (pCityGMLManager->getNextTriangle(p1, p2, p3))
        {
            auto vGridInfo = pWTMCalculator->getGridInTriangle(p1, p2, p3);
            for (auto &pix : vGridInfo)
            {
                pWebTileManager->pushPixelInfo(pix);
            }
            if (fnProgressFeedback)
            {
                fnProgressFeedback(nProcessedTriangle++);
            }
        }

        pWebTileManager->finalizePushing();

        return pWebTileManager->createTilesFromDB();
    }

    bool createGsiTileset(
        const std::string &strInputGsiGml,
        const std::string &strOutputTileDirectory,
        const int nMinZoomLevel,
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback,
        const std::function<void(int)> &fnProgressFeedback)
    {
        WTM_BBOX bbox;
        uint32_t nWidth, nHeight;
        double *pData;

        std::unique_ptr<GsiTileManager> tileManager =
            std::make_unique<GsiTileManager>(
                strOutputTileDirectory, nMinZoomLevel, nMaxZoomLevel, bOverwrite,
                [](MESSAGE_STATUS eStatus, const std::string &strMessage)
        {
            if (eStatus == MESSAGE_ERROR)
            {
                std::cerr << "ERROR : " << strMessage << std::endl;
            }
            else
            {
                std::cout << strMessage << std::endl;
            }
        },
                [](int nProgress)
        {
            std::cout << nProgress << '\r' << std::flush;
        });
        if (!tileManager->isValid())
        {
            return false;
        }

        OGRSpatialReference oSrs;
        oSrs.importFromEPSG(4612);
        oSrs.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
        auto calculator = std::make_shared<WTMCalculator>(&oSrs, TILE_PIXELS, nMaxZoomLevel, WTMCalculator::MAPBOX_RGB);

        std::shared_ptr<GsiGmlReader> gsi = std::make_shared<GsiGmlReader>(strInputGsiGml);
        if (!gsi->isValid())
        {
            fnMessageFeedback(MESSAGE_ERROR, gsi->getErrorMsg());
            return false;
        }

        gsi->getImage(&bbox, &nWidth, &nHeight, &pData);
        bool bRes = tileManager->createBaseTilesFromImage(
            bbox, nWidth, nHeight, pData, calculator);
        if (!bRes)
        {
            fnMessageFeedback(MESSAGE_ERROR, "error occured at image creation.");
            return false;
        }

        return tileManager->createOverviews();
    }

    void mergeTilesets(
        const std::vector<std::string> &vstrInputDirs,
        const std::string &strOutDir,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback,
        const std::function<void(int)> &fnProgressFeedback)
    {
        std::filesystem::path pathSrc1(vstrInputDirs[0]);
        std::filesystem::path pathOutDir(strOutDir);

        if (!std::filesystem::exists(pathSrc1))
        {
            if (fnMessageFeedback)
            {
                fnMessageFeedback(MESSAGE_ERROR,
                    std::string("input PLATEAU tile is not exists ") + pathSrc1.u8string().c_str());
                return;
            }
        }

        std::filesystem::copy(pathSrc1, pathOutDir, std::filesystem::copy_options::recursive);

        std::regex reTileDir("[0-9]+");

        for (auto iter = vstrInputDirs.begin() + 1; iter != vstrInputDirs.end(); iter++)
        {
            fnMessageFeedback(MESSAGE_INFO,
                std::string("merge ") + *iter);
            for (const std::filesystem::directory_entry &eZ :
                std::filesystem::directory_iterator(*iter))
            {
                if (eZ.is_directory() &&
                    std::regex_match(eZ.path().stem().u8string(), reTileDir))
                {
                    auto pathZFolder = eZ.path();
                    std::filesystem::path pathDestinationZ = pathOutDir;
                    pathDestinationZ /= eZ.path().stem();
                    if (!std::filesystem::exists(pathDestinationZ))
                    {
                        std::filesystem::copy(pathZFolder, pathDestinationZ, std::filesystem::copy_options::recursive);
                        continue;
                    }
                    for (const std::filesystem::directory_entry &eX :
                        std::filesystem::directory_iterator(pathZFolder))
                    {
                        if (eX.is_directory() &&
                            std::regex_match(eX.path().stem().u8string(), reTileDir))
                        {
                            auto pathXFolder = eX.path();
                            std::filesystem::path pathDestinationX = pathOutDir;
                            pathDestinationX /= eZ.path().stem();
                            pathDestinationX /= eX.path().stem();
                            if (!std::filesystem::exists(pathDestinationX))
                            {
                                std::filesystem::copy(pathXFolder, pathDestinationX, std::filesystem::copy_options::recursive);
                                continue;
                            }
                            for (const std::filesystem::directory_entry &eY :
                                std::filesystem::directory_iterator(pathXFolder))
                            {
                                if (eY.is_regular_file() &&
                                    std::regex_match(eY.path().stem().u8string(), reTileDir) &&
                                    eY.path().extension().u8string() == ".png")
                                {
                                    // std::cout << "    " << eY.path().stem() << std::endl;
                                    std::filesystem::path pathDestinationY = pathOutDir;
                                    pathDestinationY /= eZ.path().stem();
                                    pathDestinationY /= eX.path().stem();
                                    pathDestinationY /= eY.path().filename();
                                    if (std::filesystem::exists(pathDestinationY))
                                    {
                                        WebTileManager::mergePng(eY.path().u8string(), pathDestinationY.u8string(), bOverwrite, fnMessageFeedback);
                                    }
                                    else
                                    {
                                        std::filesystem::copy(eY, pathDestinationY);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void fill_zero(
        const std::string &strTileDir,
        const std::function<void(MESSAGE_STATUS, const std::string &)> &fnMessageFeedback,
        const std::function<void(int)> &fnProgressFeedback)
    {
        std::filesystem::path pathDir(strTileDir);
        if (!std::filesystem::exists(pathDir))
        {
            if (fnMessageFeedback)
            {
                fnMessageFeedback(MESSAGE_ERROR,
                    std::string("input PLATEAU tile is not exists ") + pathDir.u8string().c_str());
                return;
            }
        }

        std::regex reTileDir("[0-9]+");

        int i = 0;
        for (const std::filesystem::directory_entry &e :
            std::filesystem::recursive_directory_iterator(pathDir))
        {
            if (e.is_regular_file() &&
                std::regex_match(e.path().stem().u8string(), reTileDir))
            {
                if (!WebTileManager::fill_zeroPng(e.path().u8string()))
                {
                    fnMessageFeedback(MESSAGE_ERROR,
                        "failed to read/write image" + e.path().u8string());
                }
                fnProgressFeedback(++i);
            }
        }
    }
}
