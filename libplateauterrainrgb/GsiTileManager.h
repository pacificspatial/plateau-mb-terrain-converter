/***************************************************************************
                           GsiTileManager.h  -  description
                                 -------------------

        Create tilesets from GSI DEM image.

        begin                : Jan. 21, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/
#pragma once

#include "WebTileManager.h"

class GsiTileManager : public WebTileManager
{
public:
    GsiTileManager() = delete;
    GsiTileManager(const std::string &strOutputDirectory,
                   const uint32_t nMinZoomLevel,
                   const uint32_t nMaxZoomLevel,
                   const bool bOverwrite = false,
                   const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr,
                   const std::function<void(int)> &fnProgressFeedback = nullptr);
    virtual ~GsiTileManager();

    inline bool isValid() const { return mbValid; }
    bool createBaseTilesFromImage(
        WTM_BBOX bbox_bl,
        const uint32_t nWidth,
        const uint32_t nHeight,
        double *pData,
        std::shared_ptr<WTMCalculator> calculator);
    bool createOverviews();

private:
    std::vector<TILE_COORD> mvTiles;
};
