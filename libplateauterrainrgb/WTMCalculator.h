/***************************************************************************
                           WTMCalculator.h  -  description
                                 -------------------

        Calculator of Web Tile Mercator coordinates and Mapbox-RGB Pixel values.

        begin                : Jan. 21, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/

#pragma once

#include "CommonStruct.h"
#include "plateau-mb-terrain-converter.h"

#include <ogr_spatialref.h>
#include <vector>

#define WTM_MIN_X -20037508.342789244
#define WTM_MAX_Y 20037508.342789244
#define WTM_WIDTH 40075016.685578488
#define WTM_HEIGHT 40075016.685578488

namespace pmtc
{
    class PMTC_DLL WTMCalculator
    {
    public:
        enum COLOR_TYPE
        {
            MAPBOX_RGB,
            AIST_RGB,
            GSI_RGB
        };

    public:
        WTMCalculator() = delete;
        WTMCalculator(const OGRSpatialReference *poSrs, int nPixels, int nZoomLevel, COLOR_TYPE eType);
        virtual ~WTMCalculator();

        std::vector<TILE_PIXEL_INFO> getGridInTriangle( OGRPoint &p1, OGRPoint &p2, OGRPoint &p3 );
        std::vector<GRID_INFO> getGridInTriangle( OGRPoint &p1, OGRPoint &p2, OGRPoint &p3, OGRPoint &pTL, OGRPoint &gridResolution );

        static OGRPoint calcWTMResolution(uint32_t nZoomLevel, uint32_t nPixels);

        // inline void setSourceEPSG( int nEpsg ){ mnSourceEpsg = nEpsg; }
        inline void setTilePixelSize(uint32_t nPixels) { mnTilePixels = nPixels; }
        TILE_PIXEL_INFO calcTilePixelCoordFromTotalPixelCoord(PIXEL_COORD cPixCoord);
        PIXEL_INFO calcPix(double dZ);

        void transformLatLonToWTM(OGRPoint &pnt);

    protected:
        void prepareTransformer();

        PIXEL_COORD calcTotalPixelCoord(OGRPoint cCoord);
        void setZoomLevel(uint32_t nZoom, uint32_t nPixels);
        WTM_BBOX calcTriangleToWTMBbox(OGRPoint &p1, OGRPoint &p2, OGRPoint &p3);

        static int clcw(const OGRPoint &p1, const OGRPoint &p2, const OGRPoint &p3);
        static double calcZ(OGRPoint &&p, const OGRPoint &tp1, const OGRPoint &tp2, const OGRPoint &tp3);

        OGRSpatialReference mSourceEpsg;
        uint32_t mnZoomLevel;
        uint32_t mnTilePixels;
        bool mbValid;
        OGRPoint mWTMResolution;
        COLOR_TYPE meType;

        OGRCoordinateTransformation *mTransform;
    };
}
