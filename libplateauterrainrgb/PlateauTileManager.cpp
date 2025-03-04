/***************************************************************************
                      PlateauTileManager.cpp  -  description
                             -------------------

        Create tilesets from PLATEAU DEM data.

        begin                : Jan. 21, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/

#include "PlateauTileManager.h"
#include <cstring>

#define ERRORCHECK(f)   \
    if (f != SQLITE_OK) \
        goto ERROR;
#define TRANSACTION_CHUNC 10000

union outData
{
    char szData[256];
    TILE_PIXEL_INFO info;
};

PlateauTileManager::PlateauTileManager(
    const std::string &strOutputDirectory,
    const int nMinZoomLevel,
    const int nMaxZoomLevel,
    const bool bOverwrite,
    const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback,
    const std::function<void(int)> &fnProgressFeedback)
    : WebTileManager(
          strOutputDirectory,
          nMinZoomLevel,
          nMaxZoomLevel,
          bOverwrite,
          fnMessageFeedback,
          fnProgressFeedback),
      mpDb(nullptr),
      mpStmt(nullptr),
      mnPushCount(0)
{
    if (!std::filesystem::exists(mpathOutputDirectory))
    {
        if (mfnMessageFeedback)
        {
            mfnMessageFeedback(
                MESSAGE_WARNING,
                "output directory does not exist [ " + mpathOutputDirectory.u8string() + " ].");
        }
    }

    if (!std::filesystem::is_directory(mpathOutputDirectory))
    {
        if (mfnMessageFeedback)
        {
            mfnMessageFeedback(
                MESSAGE_ERROR,
                "output is not a directory [ " + mpathOutputDirectory.u8string() + " ].");
        }
        return;
    }

    if (mnMinZoomLevel < 0 || mnMaxZoomLevel < 0 ||
        (mnMinZoomLevel > mnMaxZoomLevel))
    {
        mfnMessageFeedback(
            MESSAGE_ERROR,
            "Invalid zoom level : " + std::to_string(mnMinZoomLevel) + " - " + std::to_string(mnMaxZoomLevel));
        return;
    }

    if (mnMaxZoomLevel >= 18)
    {
        mfnMessageFeedback(
            MESSAGE_WARNING,
            "Zoom levels above 18 can result in too many files being generated.");
    }

    std::filesystem::path pathDB = mpathOutputDirectory;
    pathDB /= "tempdb.sqlite";
    if (std::filesystem::exists(pathDB))
    {
        std::filesystem::remove(pathDB);
    }

    ERRORCHECK(sqlite3_open_v2(pathDB.u8string().c_str(), &mpDb,
                               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr));

    ERRORCHECK(sqlite3_exec(
        mpDb,
        "create table plist( tile_x integer, tile_y integer, tile_z integer, pixel_u integer, pixel_v integer, r integer, g integer, b integer, a integer )",
        nullptr, nullptr, nullptr));

    ERRORCHECK(sqlite3_prepare_v2(mpDb, "insert into plist values( ?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9)", -1, &mpStmt, nullptr));

    mbValid = true;
    return;

ERROR:
    if (mfnMessageFeedback)
    {
        mfnMessageFeedback(MESSAGE_ERROR,
                           sqlite3_errstr(sqlite3_errcode(mpDb)));
    }
}

PlateauTileManager::~PlateauTileManager()
{
    if (mpDb)
    {
        std::filesystem::path pathDB = sqlite3_db_filename(mpDb, NULL);
        sqlite3_close_v2(mpDb);
        if (std::filesystem::exists(pathDB))
        {
            std::filesystem::remove(pathDB);
        }
    }
}

bool PlateauTileManager::pushPixelInfo(const TILE_PIXEL_INFO &info)
{
    sqlite3_reset(mpStmt);
    if (!(mnPushCount % TRANSACTION_CHUNC))
    {
        ERRORCHECK(sqlite3_exec(mpDb, "begin transaction", nullptr, nullptr, nullptr));
    }
    ERRORCHECK(sqlite3_bind_int(mpStmt, 1, info.tileNum.nX));
    ERRORCHECK(sqlite3_bind_int(mpStmt, 2, info.tileNum.nY));
    ERRORCHECK(sqlite3_bind_int(mpStmt, 3, info.tileNum.nZ));
    ERRORCHECK(sqlite3_bind_int64(mpStmt, 4, info.pixCoord.nU));
    ERRORCHECK(sqlite3_bind_int64(mpStmt, 5, info.pixCoord.nV));
    ERRORCHECK(sqlite3_bind_int(mpStmt, 6, info.pixValues.nR));
    ERRORCHECK(sqlite3_bind_int(mpStmt, 7, info.pixValues.nG));
    ERRORCHECK(sqlite3_bind_int(mpStmt, 8, info.pixValues.nB));
    ERRORCHECK(sqlite3_bind_int(mpStmt, 9, info.pixValues.nA));

    if (sqlite3_step(mpStmt) != SQLITE_DONE)
        goto ERROR;

    mnPushCount++;

    if (mnPushCount >= TRANSACTION_CHUNC)
    {
        ERRORCHECK(sqlite3_exec(mpDb, "commit", nullptr, nullptr, nullptr));
        mnPushCount = 0;
    }

    return true;

ERROR:
    if (mfnMessageFeedback)
    {
        mfnMessageFeedback(MESSAGE_ERROR,
                           sqlite3_errstr(sqlite3_errcode(mpDb)));
    }
    sqlite3_finalize(mpStmt);
    return false;
}

void PlateauTileManager::finalizePushing()
{
    ERRORCHECK(sqlite3_exec(mpDb, "commit", nullptr, nullptr, nullptr));

ERROR:
    if (mfnMessageFeedback)
    {
        mfnMessageFeedback(MESSAGE_ERROR,
                           sqlite3_errstr(sqlite3_errcode(mpDb)));
    }
    sqlite3_finalize(mpStmt);
}

bool PlateauTileManager::createTilesFromDB()
{
    std::vector<TILE_COORD> vTiles;
    sqlite3_stmt *pStmt;
    std::filesystem::path pathDB = sqlite3_db_filename(mpDb, NULL);
    uint8_t *pImgBuf;
    int nProcessedTiles = 0;
    bool bRes;

    if (mfnMessageFeedback)
    {
        mfnMessageFeedback(MESSAGE_INFO, "creating base tiles... ");
    }

    ERRORCHECK(sqlite3_prepare_v2(mpDb, "select distinct tile_x, tile_y, tile_z from plist;", -1, &pStmt, nullptr));
    while (sqlite3_step(pStmt) != SQLITE_DONE)
    {
        TILE_COORD tile;
        tile.nX = sqlite3_column_int(pStmt, 0);
        tile.nY = sqlite3_column_int(pStmt, 1);
        tile.nZ = sqlite3_column_int(pStmt, 2);
        vTiles.push_back(tile);
    }
    sqlite3_finalize(pStmt);

    pImgBuf = static_cast<uint8_t *>(CPLCalloc(TILE_PIXELS * TILE_PIXELS * 4, 1));

    ERRORCHECK(sqlite3_prepare_v2(mpDb, "select pixel_u,pixel_v,r,g,b,a from plist where tile_x=?1 and tile_y=?2 and tile_z=?3", -1, &pStmt, nullptr));
    for (auto &t : vTiles)
    {
        sqlite3_bind_int(pStmt, 1, t.nX);
        sqlite3_bind_int(pStmt, 2, t.nY);
        sqlite3_bind_int(pStmt, 3, t.nZ);
        while (sqlite3_step(pStmt) != SQLITE_DONE)
        {
            int nU = sqlite3_column_int(pStmt, 0);
            int nV = sqlite3_column_int(pStmt, 1);
            uint8_t bR = static_cast<uint8_t>(sqlite3_column_int(pStmt, 2));
            uint8_t bG = static_cast<uint8_t>(sqlite3_column_int(pStmt, 3));
            uint8_t bB = static_cast<uint8_t>(sqlite3_column_int(pStmt, 4));
            uint8_t bA = static_cast<uint8_t>(sqlite3_column_int(pStmt, 5));
            pImgBuf[nV * TILE_PIXELS * 4 + nU * 4 + 0] = bR;
            pImgBuf[nV * TILE_PIXELS * 4 + nU * 4 + 1] = bG;
            pImgBuf[nV * TILE_PIXELS * 4 + nU * 4 + 2] = bB;
            pImgBuf[nV * TILE_PIXELS * 4 + nU * 4 + 3] = bA;
        }
        auto strFName = makeOutputFilePath(mpathOutputDirectory, t.nX, t.nY, t.nZ);
        if (!createDirectoryFromTilePath(std::filesystem::path(strFName)))
        {
            CPLFree(pImgBuf);
            sqlite3_finalize(pStmt);
            return false;
        }

        if (std::filesystem::exists(std::filesystem::path(strFName)))
        {
            bRes = mergePng(strFName, pImgBuf, mbOverwrite, mfnMessageFeedback);
        }
        else
        {
            bRes = writePng(strFName, pImgBuf);
        }

        sqlite3_reset(pStmt);
        std::memset(pImgBuf, 0x00, TILE_PIXELS * TILE_PIXELS * 4);
        if (mfnProgressFeedback)
        {
            mfnProgressFeedback(nProcessedTiles++);
        }
    }
    CPLFree(pImgBuf);
    sqlite3_finalize(pStmt);

    buildOverviews(vTiles);

    return bRes;

ERROR:
    if (mfnMessageFeedback)
    {
        mfnMessageFeedback(MESSAGE_ERROR,
                           sqlite3_errstr(sqlite3_errcode(mpDb)));
    }
    sqlite3_finalize(mpStmt);

    return false;
}
