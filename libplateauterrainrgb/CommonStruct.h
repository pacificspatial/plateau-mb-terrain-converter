/***************************************************************************
                       CommonStruct.h  -  description
                            -------------------

        Common structure definitions.

        begin                : Jul. 29, 2024
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/

#pragma once

#include <gdal_priv.h>

#define TILE_PIXELS 256

namespace pmtc
{
    using PIXEL_COORD = struct _pixel_coord
    {
        uint64_t nU;
        uint64_t nV;
    };

    using WTM_BBOX = struct _wtm_bbox
    {
        OGRPoint tl;
        OGRPoint br;
    };

    using TILE_COORD = struct _tile_coord
    {
        uint32_t nX;
        uint32_t nY;
        uint32_t nZ;
    };

    using PIXEL_INFO = struct _pixel_info
    {
        uint8_t nR;
        uint8_t nG;
        uint8_t nB;
        uint8_t nA;
    };

    using TILE_PIXEL_INFO = struct _tile_pixel_infos
    {
        TILE_COORD tileNum;
        PIXEL_COORD pixCoord;
        PIXEL_INFO pixValues;
    };

    using GRID_INFO = struct _grid_info
    {
        uint32_t nU;
        uint32_t nV;
        double dHeight;
    };
}
