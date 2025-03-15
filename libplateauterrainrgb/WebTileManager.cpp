/***************************************************************************
                         WebTileManager.cpp  -  description
                                -------------------

        Base class of tile manager.

        begin                : Jan. 21, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/

#include "WebTIleManager.h"

#include <cpl_string.h>
#include <png.h>

#include <vector>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <cstring>

namespace pmtc
{
    WebTileManager::WebTileManager(
        const std::string &strOutputDirectory,
        const int nMinZoomLevel,
        const int nMaxZoomLevel,
        const bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback,
        const std::function<void(int)> &fnProgressFeedback)
        : mbValid(false),
        mbOverwrite(bOverwrite),
        mpathOutputDirectory(strOutputDirectory),
        mnMinZoomLevel(nMinZoomLevel),
        mnMaxZoomLevel(nMaxZoomLevel),
        mfnMessageFeedback(fnMessageFeedback),
        mfnProgressFeedback(fnProgressFeedback)
    {
    }

    WebTileManager::~WebTileManager()
    {
    }

    bool WebTileManager::writePng(const std::string &strFName, uint8_t *pImg)
    {
        png_image png;

        std::memset(&png, 0x00, sizeof(png_image));
        png.version = PNG_IMAGE_VERSION;
        png.width = TILE_PIXELS;
        png.height = TILE_PIXELS;
        png.format = PNG_FORMAT_RGBA;

        return png_image_write_to_file(&png, strFName.c_str(), 0, pImg, PNG_IMAGE_ROW_STRIDE(png), nullptr) != 0;
    }

    bool WebTileManager::mergePng(const std::string &strFName, uint8_t *pImg, bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback)
    {
        png_image png;
        uint32_t nStride, nWidth, nHeight;
        uint8_t *pBuf;
        bool bRet;

        std::memset(&png, 0x00, sizeof(png_image));
        png.version = PNG_IMAGE_VERSION;

        png_image_begin_read_from_file(&png, strFName.c_str());
        if (PNG_IMAGE_FAILED(png))
        {
            return writePng(strFName, pImg);
        }

        nWidth = png.width;
        nHeight = png.height;
        if (nWidth != TILE_PIXELS || nHeight != TILE_PIXELS)
        {
            if (fnMessageFeedback)
            {
                fnMessageFeedback(MESSAGE_ERROR,
                    "tile size that will be merged is not 256.");
                png_image_free(&png);
                return false;
            }
        }

        nStride = PNG_IMAGE_ROW_STRIDE(png);
        pBuf = new uint8_t[PNG_IMAGE_BUFFER_SIZE(png, nStride)];
        png_image_finish_read(&png, nullptr, pBuf, nStride, nullptr);

        png_image_free(&png);

        // merge
        for (int i = 0; i < TILE_PIXELS * TILE_PIXELS * 4; i += 4)
        {
            if (bOverwrite)
            {
                if (pImg[i + 3] != 0)
                {
                    pBuf[i + 0] = pImg[i + 0];
                    pBuf[i + 1] = pImg[i + 1];
                    pBuf[i + 2] = pImg[i + 2];
                    pBuf[i + 3] = pImg[i + 3];
                }
            }
            else
            {
                if (pBuf[i + 3] == 0)
                {
                    pBuf[i + 0] = pImg[i + 0];
                    pBuf[i + 1] = pImg[i + 1];
                    pBuf[i + 2] = pImg[i + 2];
                    pBuf[i + 3] = pImg[i + 3];
                }
            }
        }

        bRet = writePng(strFName, pBuf);

        delete[] pBuf;

        return bRet;
    }

    bool WebTileManager::mergePng(const std::string &strSrcFName, const std::string &strDstFName, bool bOverwrite,
        const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback)
    {
        uint8_t *pImgIn = static_cast<uint8_t *>(std::malloc(TILE_PIXELS * TILE_PIXELS * 4));
        std::memset(pImgIn, 0x00, TILE_PIXELS * TILE_PIXELS * 4);
        if (!readPng(strSrcFName, &pImgIn))
        {
            if (fnMessageFeedback)
            {
                fnMessageFeedback(MESSAGE_ERROR,
                    std::string("failed to merge image [") + strSrcFName + std::string("]"));
                return false;
            }
        }

        bool bRes = mergePng(strDstFName, pImgIn, bOverwrite, fnMessageFeedback);
        std::free(pImgIn);

        return bRes;
    }

    bool WebTileManager::fill_zeroPng(const std::string &strFName)
    {
        bool bNullExists = false;
        uint8_t *pImgIn;

        if (!readPng(strFName, &pImgIn))
        {
            return false;
        }

        for (int i = 0; i < TILE_PIXELS * TILE_PIXELS * 4; i += 4)
        {
            if (pImgIn[i + 3] == 0)
            {
                pImgIn[i + 0] = 1;
                pImgIn[i + 1] = 134;
                pImgIn[i + 2] = 160;
                pImgIn[i + 3] = 255;

                if (!bNullExists)
                {
                    bNullExists = true;
                }
            }
        }

        if (bNullExists)
        {
            if (!writePng(strFName, pImgIn))
            {
                std::free(pImgIn);
                return false;
            }
        }

        std::free(pImgIn);
        return true;
    }

    std::vector<TILE_COORD> WebTileManager::getOverviewTileList(std::vector<TILE_COORD> &vTileList)
    {
        std::vector<TILE_COORD> vOverviewTiles;

        for (auto &t : vTileList)
        {
            TILE_COORD sOverviewTile;
            sOverviewTile.nX = t.nX >> 1;
            sOverviewTile.nY = t.nY >> 1;
            sOverviewTile.nZ = t.nZ - 1;
            vOverviewTiles.push_back(sOverviewTile);
        }

        std::sort(vOverviewTiles.begin(), vOverviewTiles.end(),
            [](TILE_COORD &a, TILE_COORD &b)
        {
            return a.nY == b.nY ? a.nX < b.nX : a.nY < b.nY;
        });

        auto pEnd = std::unique(vOverviewTiles.begin(), vOverviewTiles.end(),
            [](TILE_COORD &a, TILE_COORD &b)
        {
            return a.nX == b.nX && a.nY == b.nY && a.nZ == b.nZ;
        });
        vOverviewTiles.erase(pEnd, vOverviewTiles.end());

        return vOverviewTiles;
    }

    bool WebTileManager::buildOverviews(std::vector<TILE_COORD> &vBaseTiles)
    {
        auto vTiles = getOverviewTileList(vBaseTiles);
        uint32_t nCurrentZoomLevel = vBaseTiles.front().nZ - 1;

        if (nCurrentZoomLevel >= mnMinZoomLevel)
        {
            if (mfnMessageFeedback)
            {
                mfnMessageFeedback(
                    MESSAGE_INFO,
                    "creating zoom level " + std::to_string(nCurrentZoomLevel) + " tiles ...");
            }

            int nProcessedTiles = 0;
            for (auto &t : vTiles)
            {
                std::filesystem::path pathBassTileTL =
                    makeOutputFilePath(mpathOutputDirectory, t.nX << 1, t.nY << 1, t.nZ + 1);
                std::filesystem::path pathBassTileTR =
                    makeOutputFilePath(mpathOutputDirectory, (t.nX << 1) + 1, t.nY << 1, t.nZ + 1);
                std::filesystem::path pathBassTileBL =
                    makeOutputFilePath(mpathOutputDirectory, t.nX << 1, (t.nY << 1) + 1, t.nZ + 1);
                std::filesystem::path pathBassTileBR =
                    makeOutputFilePath(mpathOutputDirectory, (t.nX << 1) + 1, (t.nY << 1) + 1, t.nZ + 1);

                if (!std::filesystem::exists(pathBassTileTL) &&
                    !std::filesystem::exists(pathBassTileTR) &&
                    !std::filesystem::exists(pathBassTileBL) &&
                    !std::filesystem::exists(pathBassTileBR))
                {
                    continue;
                }

                std::filesystem::path pathOutput =
                    makeOutputFilePath(mpathOutputDirectory, t.nX, t.nY, t.nZ);
                createDirectoryFromTilePath(pathOutput);
                createOverviewTileFromQuadTiles(
                    pathOutput, pathBassTileTL, pathBassTileTR, pathBassTileBL, pathBassTileBR);
                if (mfnProgressFeedback)
                {
                    mfnProgressFeedback(nProcessedTiles++);
                }
            }

            return buildOverviews(vTiles);
        }
        else
        {
            return false;
        }
    }

    bool WebTileManager::createDirectoryFromTilePath(const std::filesystem::path pathTileName)
    {
        std::filesystem::path pathZ = pathTileName.parent_path().parent_path();
        if (!std::filesystem::exists(pathZ))
        {
            if (!std::filesystem::create_directory(pathZ))
            {
                if (mfnMessageFeedback)
                {
                    mfnMessageFeedback(
                        MESSAGE_ERROR,
                        "unable to create directory : " + pathZ.u8string());
                }
                return false;
            }
        }

        std::filesystem::path pathX = pathTileName.parent_path();
        if (!std::filesystem::exists(pathX))
        {
            if (!std::filesystem::create_directory(pathX))
            {
                if (mfnMessageFeedback)
                {
                    mfnMessageFeedback(
                        MESSAGE_ERROR,
                        "unable to create directory : " + pathX.u8string());
                }
                return false;
            }
        }

        return true;
    }

    std::string WebTileManager::makeOutputFilePath(const std::filesystem::path pathBase, const int nX, const int nY, const int nZ)
    {
        std::filesystem::path pathOutput = pathBase;

        pathOutput /= std::to_string(nZ);
        pathOutput /= std::to_string(nX);
        pathOutput /= std::to_string(nY) + ".png";
        return pathOutput.u8string();
    }

    bool WebTileManager::readPng(const std::string strFName, uint8_t **pImg)
    {
        png_image png;
        std::memset(&png, 0x00, sizeof(png_image));
        png.version = PNG_IMAGE_VERSION;

        png_image_begin_read_from_file(&png, strFName.c_str());
        if (PNG_IMAGE_FAILED(png))
        {
            std::cout << png.message << std::endl;
            return false;
        }

        uint32_t nStride = PNG_IMAGE_ROW_STRIDE(png);
        *pImg = static_cast<uint8_t *>(std::malloc(PNG_IMAGE_BUFFER_SIZE(png, nStride)));

        png_image_finish_read(&png, nullptr, *pImg, nStride, nullptr);

        return true;
    }

    bool WebTileManager::createOverviewTileFromQuadTiles(
        const std::filesystem::path &pathOutput,
        const std::filesystem::path &pathTileTL,
        const std::filesystem::path &pathTileTR,
        const std::filesystem::path &pathTileBL,
        const std::filesystem::path &pathTileBR)
    {
        uint8_t *pImgIn = static_cast<uint8_t *>(std::malloc(TILE_PIXELS * TILE_PIXELS * 4));
        uint8_t *pImgOut = static_cast<uint8_t *>(std::malloc(TILE_PIXELS * TILE_PIXELS * 4));
        if (!pImgOut)
        {
            return false;
        }

        std::memset(pImgIn, 0x00, TILE_PIXELS * TILE_PIXELS * 4);
        std::memset(pImgOut, 0x00, TILE_PIXELS * TILE_PIXELS * 4);

        if (std::filesystem::exists(pathTileTL))
        {
            readPng(pathTileTL.u8string().c_str(), &pImgIn);
            for (int i = 0; i < TILE_PIXELS; i += 2)
            {
                for (int j = 0; j < TILE_PIXELS; j += 2)
                {
                    int nOutIndex = (i >> 1) * TILE_PIXELS * 4 + (j >> 1) * 4;
                    pImgOut[nOutIndex + 0] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 0];
                    pImgOut[nOutIndex + 1] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 1];
                    pImgOut[nOutIndex + 2] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 2];
                    pImgOut[nOutIndex + 3] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 3];
                }
            }
            std::free(pImgIn);
        }

        if (std::filesystem::exists(pathTileTR))
        {
            readPng(pathTileTR.u8string().c_str(), &pImgIn);
            for (int i = 0; i < TILE_PIXELS; i += 2)
            {
                for (int j = 0; j < TILE_PIXELS; j += 2)
                {
                    int nOutIndex = (i >> 1) * TILE_PIXELS * 4 + ((TILE_PIXELS >> 1) + (j >> 1)) * 4;
                    pImgOut[nOutIndex + 0] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 0];
                    pImgOut[nOutIndex + 1] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 1];
                    pImgOut[nOutIndex + 2] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 2];
                    pImgOut[nOutIndex + 3] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 3];
                }
            }
            std::free(pImgIn);
        }

        if (std::filesystem::exists(pathTileBL))
        {
            readPng(pathTileBL.u8string().c_str(), &pImgIn);
            for (int i = 0; i < TILE_PIXELS; i++)
            {
                for (int j = 0; j < TILE_PIXELS; j++)
                {
                    int nOutIndex = ((TILE_PIXELS >> 1) + (i >> 1)) * TILE_PIXELS * 4 + (j >> 1) * 4;
                    pImgOut[nOutIndex + 0] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 0];
                    pImgOut[nOutIndex + 1] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 1];
                    pImgOut[nOutIndex + 2] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 2];
                    pImgOut[nOutIndex + 3] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 3];
                }
            }
            std::free(pImgIn);
        }

        if (std::filesystem::exists(pathTileBR))
        {
            readPng(pathTileBR.u8string().c_str(), &pImgIn);
            for (int i = 0; i < TILE_PIXELS; i++)
            {
                for (int j = 0; j < TILE_PIXELS; j++)
                {
                    int nOutIndex = ((TILE_PIXELS >> 1) + (i >> 1)) * TILE_PIXELS * 4 + ((TILE_PIXELS >> 1) + (j >> 1)) * 4;
                    pImgOut[nOutIndex + 0] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 0];
                    pImgOut[nOutIndex + 1] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 1];
                    pImgOut[nOutIndex + 2] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 2];
                    pImgOut[nOutIndex + 3] = pImgIn[i * TILE_PIXELS * 4 + j * 4 + 3];
                }
            }
            std::free(pImgIn);
        }

        bool bRes = writePng(pathOutput.u8string(), pImgOut);
        std::free(pImgOut);

        return bRes;
    }
}
