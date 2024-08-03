// plateau-mb-terrain-converter.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "plateau-mb-terrain-converter.h"

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
    mpWTMCalculator = std::make_unique<WTMCalculator>( pInputSpatialRef->GetEPSGGeogCS(), TILE_PIXELS, mnMaxZoomLevel, WTMCalculator::MAPBOX_RGB );
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
}

