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
    const int nMaxZoomLevel,
    const std::function<void(std::string)> &fnMessageFeedback,
    const std::function<void(int)> &fnProgressFeedback
)
    : 
    mstrInputTerrainCityGML( strInputTerrainCityGML ),
    mstrTileDirectory( strOutputTileDirectory ),
    mnMinZoomLevel( nMinZoomLevel ),
    mnMaxZoomLevel( nMaxZoomLevel ),
    mfnMessageFeedback( fnMessageFeedback ),
    mfnProgressFeedback( fnProgressFeedback ),
    mbValid( false )
{
    mpWebTileManager = std::make_unique<WebTileManager>( strOutputTileDirectory, nMinZoomLevel, nMaxZoomLevel, mfnMessageFeedback, mfnProgressFeedback );
    if ( !mpWebTileManager->isValid() )
    {
        return;
    }

    mpCityGMLManager = std::make_unique<CityGMLManager>( strInputTerrainCityGML, mfnMessageFeedback );
    if ( !mpCityGMLManager->isValid() )
    {
        return;
    }

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

    mbValid = true;
}


PlateauMapboxTerrainConverter::~PlateauMapboxTerrainConverter()
{
}


void PlateauMapboxTerrainConverter::createTileset()
{
    OGRPoint p1, p2, p3;

    if ( mfnMessageFeedback )
    {
        mfnMessageFeedback( "calculating grid height in triangles ..." );
    }

    int nProcessedTriangle = 0;
    while ( mpCityGMLManager->getNextTriangle( p1, p2, p3 ) )
    {
        auto vGridInfo = mpWTMCalculator->getGridInTriangle( p1, p2, p3 );
        for ( auto &pix : vGridInfo )
        {
            mpWebTileManager->pushPixelInfo( pix );
        }
        if ( mfnProgressFeedback )
        {
            mfnProgressFeedback( nProcessedTriangle++ );
        }
    }
    mpWebTileManager->createTilesFromDB();
}

