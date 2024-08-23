#include "WTMCalculator.h"



#include <cmath>
#include <iostream>


#define WTM_MIN_X -20037508.342789244
#define WTM_MAX_Y 20037508.342789244
#define WTM_WIDTH 40075016.685578488
#define WTM_HEIGHT 40075016.685578488

#define ULONGLONG unsigned long long



WTMCalculator::WTMCalculator( const OGRSpatialReference *poSrs, int nPixels, int nZoomLevel, COLOR_TYPE eType )
	: mbValid(false),
	mSourceEpsg(*poSrs),
	mnTilePixels(nPixels),
	mnZoomLevel(nZoomLevel),
	meType(eType)
{
	mWTMResolution = calcWTMResolution( mnZoomLevel, mnTilePixels );
	prepareTransformer();
}


WTMCalculator::~WTMCalculator()
{

}


void WTMCalculator::prepareTransformer()
{
	OGRSpatialReference poSrsDst;

//	poSrsSrc.importFromEPSG( mnSourceEpsg );
	poSrsDst.importFromEPSG( 3857 );
	OGRCoordinateTransformationOptions options = OGRCoordinateTransformationOptions();
	options.SetBallparkAllowed( true );
	mTransform = OGRCreateCoordinateTransformation( &mSourceEpsg, &poSrsDst, options );	
}


WTM_BBOX WTMCalculator::calcTriangleToWTMBbox( OGRPoint &p1, OGRPoint &p2, OGRPoint &p3 )
{
	p1.transform( mTransform );
	p2.transform( mTransform );
	p3.transform( mTransform );

	double dMinX = std::min( std::min(p1.getX(), p2.getX()), p3.getX() );
	double dMaxX = std::max( std::max(p1.getX(), p2.getX()), p3.getX() );
	double dMinY = std::min( std::min(p1.getY(), p2.getY()), p3.getY() );
	double dMaxY = std::max( std::max(p1.getY(), p2.getY()), p3.getY() );

	return {{dMinX, dMaxY}, {dMaxX, dMinY}};
}


OGRPoint WTMCalculator::calcWTMResolution( uint32_t nZoomLevel, uint32_t nPixels )
{
	uint64_t nDiv = 1 << nZoomLevel;

	return {WTM_WIDTH / (nDiv*nPixels), WTM_HEIGHT / (nDiv*nPixels)};
}


void WTMCalculator::setZoomLevel( uint32_t nZoom, uint32_t nPixels )
{
	mnZoomLevel = nZoom;
	mWTMResolution = calcWTMResolution( mnZoomLevel, nPixels );
}


PIXEL_COORD WTMCalculator::calcTotalPixelCoord( OGRPoint cCoord )
{
	double dWX = cCoord.getX() - WTM_MIN_X;
	double dWY = WTM_MAX_Y - cCoord.getY();
	double dPX = dWX / mWTMResolution.getX();
	double dPY = dWY / mWTMResolution.getY();

	return {(uint32_t)std::floor(dPX), (uint32_t)std::floor(dPY)};
}


TILE_PIXEL_INFO WTMCalculator::calcTilePixelCoordFromTotalPixelCoord( PIXEL_COORD cPixCoord )
{
	uint32_t nTileX = (uint32_t)std::floor( cPixCoord.nU / mnTilePixels );
	uint32_t nTileY = (uint32_t)std::floor( cPixCoord.nV / mnTilePixels );
	uint32_t nPixX = (uint32_t)( cPixCoord.nU - nTileX*mnTilePixels );
	uint32_t nPixY = (uint32_t)( cPixCoord.nV - nTileY*mnTilePixels );

	return {{nTileX, nTileY, mnZoomLevel}, {nPixX, nPixY}, {0, 0, 0, 0}};
} 


int WTMCalculator::clcw( const OGRPoint &p1, const OGRPoint &p2, const OGRPoint &p3 )
{
	int a;
	double dx2, dy2, dx3, dy3;

	dx2 = p2.getX() - p1.getX();
	dy2 = p2.getY() - p1.getY();
	dx3 = p3.getX() - p1.getX();
	dy3 = p3.getY() - p1.getY();

	if ( ( dx2 * dy3 ) > ( dx3 * dy2 ) ) a = -1;
	else if ( ( dx2 * dy3 ) < ( dx3 * dy2 ) ) a = 1;
	else a = 0;

	return a;
}


double WTMCalculator::calcZ( OGRPoint &&p, const OGRPoint &tp1, const OGRPoint &tp2, const OGRPoint &tp3 )
{
	OGRPoint v1, v2;
	double x, y, z, w;

	v1.setX( tp1.getX() - tp2.getX() );
	v1.setY( tp1.getY() - tp2.getY() );
	v1.setZ( tp1.getZ() - tp2.getZ() );
	v2.setX( tp2.getX() - tp3.getX() );
	v2.setY( tp2.getY() - tp3.getY() );
	v2.setZ( tp2.getZ() - tp3.getZ() );

	x = v1.getY()*v2.getZ() - v1.getZ()*v2.getY();
	y = v1.getZ()*v2.getX() - v1.getX()*v2.getZ();
	z = v1.getX()*v2.getY() - v1.getY()*v2.getX();

	double dLen =sqrt( x*x + y*y + z*z );
	if ( dLen == 0 ) dLen=1.0;

	x /= dLen;
	y /= dLen;
	z /= dLen;
	w = -( x*tp1.getX() + y*tp1.getY() + z*tp1.getZ() );

	return ( x*p.getX() + y*p.getY() + w ) / (-1 * z);
}


PIXEL_INFO WTMCalculator::calcPix( double dZ )
{
	PIXEL_INFO info;
	double dVal;

	if ( meType == MAPBOX_RGB )
	{
		if ( dZ == HUGE_VAL )
		{
			info = {255, 255, 255, 0};
		}
		else
		{
			dVal = (dZ + 10000) / 0.1;
			info.nR = static_cast<uint8_t>( std::floor( dVal / (1<<16) ) );
			dVal = std::fmod( dVal, 1<<16 );
			info.nG = static_cast<uint8_t>( std::floor( dVal / (1<<8) ) );
			info.nB = static_cast<uint8_t>( std::fmod( dVal, 1<<8 ) );
			info.nA = 255;
		}
	}
	else
	{
		if ( dZ == HUGE_VAL )
		{
			if ( meType == AIST_RGB )
			{
				info = {255, 255, 255, 0};
			}
			else
			{
				info = {128, 0, 0, 255};
			}
		}
		else
		{
			dVal = dZ * 100;
			if ( dVal > 1<<23 ) dVal -= 1<<24;
			info.nR = static_cast<uint8_t>( std::floor( dVal / (1<<16) ) );
			dVal = std::fmod( dVal, 1<<16);
			info.nG = static_cast<uint8_t>( std::floor( dVal / (1<<8) ) );
			info.nB = static_cast<uint8_t>( std::fmod( dVal, 1<<8 ) );
		}
	}

	return info;
}


std::vector<TILE_PIXEL_INFO> WTMCalculator::getGridInTriangle( OGRPoint p1, OGRPoint p2, OGRPoint p3 )
{
	int f1, f2, f3;
	std::vector<TILE_PIXEL_INFO> vInfo;

	auto bbox = calcTriangleToWTMBbox( p1, p2, p3 );

	//std::cout << bbox.tl.getX() << " | " << bbox.tl.getY() << std::endl;
	//std::cout << bbox.br.getX() << " | " << bbox.br.getY() << std::endl;

	auto pixcoord_bbox_tl = calcTotalPixelCoord( {bbox.tl.getX(), bbox.tl.getY()} );
	auto pixcoord_bbox_br = calcTotalPixelCoord( {bbox.br.getX(), bbox.br.getY()} );
	pixcoord_bbox_br.nU += 1;
	pixcoord_bbox_br.nV += 1;

	//std::cout << pixcoord_bbox_tl.nU <<  " | " << pixcoord_bbox_tl.nV << std::endl;
	//std::cout << pixcoord_bbox_br.nU <<  " | " << pixcoord_bbox_br.nV << std::endl;

	//uint64_t nStartPixX = pixcoord_bbox_tl.nU;
	//uint64_t nStartPixY = pixcoord_bbox_tl.nV;

	double dSX, dSY;
	uint64_t v = pixcoord_bbox_tl.nV;
	while ( v <= pixcoord_bbox_br.nV )
	{
		dSY = WTM_MAX_Y - v * mWTMResolution.getY() + 1;
		uint64_t u = pixcoord_bbox_tl.nU;
		while ( u < pixcoord_bbox_br.nU )
		{
			dSX = WTM_MIN_X + u * mWTMResolution.getX() + 1;
			f1 = clcw( {dSX, dSY}, p1, p2 );
			f2 = clcw( {dSX, dSY}, p2, p3 );
			f3 = clcw( {dSX, dSY}, p3, p1 );

			if ( (f1*f2)>0 && (f2*f3)>0 && (f3*f1)>0 && (f1 != 0 && f2 != 0 && f3 != 0 ) )
			{
				double dZ = calcZ( {dSX, dSY}, p1, p2, p3 );
				auto pixInfo = calcTilePixelCoordFromTotalPixelCoord( {u, v} );
				pixInfo.pixValues = calcPix( dZ );
				vInfo.push_back( std::move(pixInfo) );
#ifdef _DEBUG
				auto info = vInfo.back();
				printf( "tile : [%7d %7d] pix : [%3d %3d] rgb : [%3d %3d %3d %3d]\n",
						info.tileNum.nX, info.tileNum.nY, info.pixCoord.nU, info.pixCoord.nV, 
						info.pixValues.nR, info.pixValues.nG, info.pixValues.nB, info.pixValues.nA );
#endif
			}
			//dSX += mWTMResolution.x;
			u++;
		}
		//dSY -= mWTMResolution.y;
		v++;
	}

	return std::move( vInfo );
}

