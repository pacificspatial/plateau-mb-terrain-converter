/***************************************************************************
                      PlateauTileManager.h  -  description
                             -------------------

        Create tilesets from PLATEAU DEM data.

        begin                : Jan. 21, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/
#pragma once

#include "WebTileManager.h"

class PlateauTileManager : public WebTileManager
{
public:
    PlateauTileManager() = delete;
    PlateauTileManager(const std::string &strOutputDirectory,
                       const int nMinZoomLevel,
                       const int nMaxZoomLevel,
                       const bool bOverwrite = false,
                       const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr,
                       const std::function<void(int)> &fnProgressFeedback = nullptr);
    virtual ~PlateauTileManager();

    bool pushPixelInfo(const TILE_PIXEL_INFO &info);
    void finalizePushing();
    bool createTilesFromDB();

private:
    int mnPushCount;
    sqlite3 *mpDb;
    sqlite3_stmt *mpStmt;
};
