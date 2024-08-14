#pragma once


#include "CommonStruct.h"

#include <ogr_spatialref.h>
#include <vector>


class WTMCalculator
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
	WTMCalculator( const OGRSpatialReference *poSrs, int nPixels, int nZoomLevel, COLOR_TYPE eType );
	virtual ~WTMCalculator();

	std::vector<TILE_PIXEL_INFO> getGridInTriangle( OGRPoint p1, OGRPoint p2, OGRPoint p3 );

	static OGRPoint calcWTMResolution( uint32_t nZoomLevel, uint32_t nPixels );

	//inline void setSourceEPSG( int nEpsg ){ mnSourceEpsg = nEpsg; }
	inline void setTilePixelSize( uint32_t nPixels ){ mnTilePixels = nPixels; }

protected:
	void prepareTransformer();

	PIXEL_COORD calcTotalPixelCoord( OGRPoint cCoord );
	TILE_PIXEL_INFO calcTilePixelCoordFromTotalPixelCoord( PIXEL_COORD cPixCoord );
	void setZoomLevel( uint32_t nZoom, uint32_t nPixels );
	WTM_BBOX calcTriangleToWTMBbox( OGRPoint &p1, OGRPoint &p2, OGRPoint &p3 );

	static int clcw( const OGRPoint &p1, const OGRPoint &p2, const OGRPoint &p3 );
	static double calcZ( OGRPoint &&p, const OGRPoint &tp1, const OGRPoint &tp2, const OGRPoint &tp3 );
	PIXEL_INFO calcPix( double dZ );

	OGRSpatialReference mSourceEpsg;
	uint32_t mnZoomLevel;
	uint32_t mnTilePixels;
	bool mbValid;
	OGRPoint mWTMResolution;
	COLOR_TYPE meType;

	OGRCoordinateTransformation *mTransform;
};

