// plateau-mb-terrain-converter.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "plateau-mb-terrain-converter.h"
#include "WTMCalculator.h"
#include "CityGMLManager.h"
#include "WebTileManager.h"


#include <gdal_priv.h>


PlateauMapboxTerrainConverter::PlateauMapboxTerrainConverter(
    const std::string& strInputTerrainCityGML,
    const std::string& strOutputTileDirectory,
    const int nMinZoomLevel,
    const int nMaxZoomLevel
)
    : 
    mstrInputTerrainCityGML( strInputTerrainCityGML ),
    mstrTileDirectory( strOutputTileDirectory ),
    mnMinZoomLevel( nMinZoomLevel ),
    mnMaxZoomLevel( nMaxZoomLevel )
{
    mpCityGMLManager = std::make_unique<CityGMLManager>( strInputTerrainCityGML );
    auto pInputSpatialRef = mpCityGMLManager->getSpatialRef();
    if ( pInputSpatialRef )
    {
//        mpWTMCalculator = std::make_unique<WTMCalculator>( pInputSpatialRef->GetEPSGGeogCS(), TILE_PIXELS, mnMaxZoomLevel, WTMCalculator::MAPBOX_RGB );
        mpWTMCalculator = std::make_unique<WTMCalculator>( pInputSpatialRef, TILE_PIXELS, mnMaxZoomLevel, WTMCalculator::MAPBOX_RGB );
    }
    else
    {
        OGRSpatialReference oSrs;
        oSrs.importFromEPSG( 4612 );
        oSrs.SetAxisMappingStrategy( OAMS_TRADITIONAL_GIS_ORDER );
        mpWTMCalculator = std::make_unique<WTMCalculator>( &oSrs, TILE_PIXELS, mnMaxZoomLevel, WTMCalculator::MAPBOX_RGB );
    }
    mpWebTileManager = std::make_unique<WebTileManager>( strOutputTileDirectory, nMinZoomLevel, nMaxZoomLevel );
}


PlateauMapboxTerrainConverter::~PlateauMapboxTerrainConverter()
{
}


void PlateauMapboxTerrainConverter::createTileset()
{
    OGRPoint p1, p2, p3;

    while ( mpCityGMLManager->getNextTriangle( p1, p2, p3 ) )
    {
        auto vGridInfo = mpWTMCalculator->getGridInTriangle( p1, p2, p3 );
        for ( auto pix : vGridInfo )
        {
            mpWebTileManager->pushPixelInfo( pix );
        }
    }
    mpWebTileManager->createTilesFromDB();
}

