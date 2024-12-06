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
    const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback,
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

    if ( mfnMessageFeedback )
    {
        mfnMessageFeedback( MESSAGE_INFO, "reading CityGML ..." );
    }
    mpCityGMLManager = std::make_unique<CityGMLManager>( strInputTerrainCityGML );
    if ( !mpCityGMLManager->isValid() )
    {
        mfnMessageFeedback( MESSAGE_ERROR, mpCityGMLManager->getLastError() );
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
        std::cout << "srs is not defined.. " << std::endl;
        OGRSpatialReference oSrs;
        oSrs.importFromEPSG( 4612 );
        oSrs.SetAxisMappingStrategy( OAMS_TRADITIONAL_GIS_ORDER );
        //oSrs.SetDataAxisToSRSAxisMapping( {1, 0} );

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
        mfnMessageFeedback( MESSAGE_INFO, "calculating grid height in triangles ..." );
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

