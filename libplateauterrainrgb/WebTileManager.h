/***************************************************************************
                         WebTileManager.h  -  description
                               -------------------

        Base class of tile manager.

        begin                : Jan. 21, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/
#pragma once

#include "plateau-mb-terrain-converter.h"
#include "CommonStruct.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <functional>

namespace pmtc
{
    class PMTC_DLL WebTileManager
    {
    public:
        WebTileManager() = delete;
        WebTileManager(const std::string &strOutputDirectory,
            const int nMinZoomLevel,
            const int nMaxZoomLevel,
            const bool bOverwrite = false,
            const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr,
            const std::function<void(int)> &fnProgressFeedback = nullptr);
        virtual ~WebTileManager();

        inline bool isValid() const { return mbValid; }

        static bool mergePng(const std::string &strSrcFName, const std::string &strDstFName, bool bOverwrite,
            const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr);
        static bool fill_zeroPng(const std::string &strFName);

    protected:
        static bool writePng(const std::string &strFName, uint8_t *pImg);
        static bool mergePng(const std::string &strFName, uint8_t *pImg, bool bOverwrite = false,
            const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr);
        static bool readPng(const std::string strFName, uint8_t **pImg);
        std::string makeOutputFilePath(const std::filesystem::path pathBase, const int nX, const int nY, const int nZ);
        bool createDirectoryFromTilePath(const std::filesystem::path pathTileName);
        bool buildOverviews(std::vector<TILE_COORD> &vBaseTiles);
        bool createOverviewTileFromQuadTiles(
            const std::filesystem::path &pathOutput,
            const std::filesystem::path &pathTileTL,
            const std::filesystem::path &pathTileTR,
            const std::filesystem::path &pathTileBL,
            const std::filesystem::path &pathTileBR);
        std::vector<TILE_COORD> getOverviewTileList(std::vector<TILE_COORD> &vTileList);

        bool mbValid;
        bool mbOverwrite;

        uint32_t mnMinZoomLevel;
        uint32_t mnMaxZoomLevel;

        std::filesystem::path mpathOutputDirectory;
        std::function<void(MESSAGE_STATUS, std::string)> mfnMessageFeedback;
        std::function<void(int)> mfnProgressFeedback;
    };
}
