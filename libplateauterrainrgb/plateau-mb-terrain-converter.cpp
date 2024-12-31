// plateau-mb-terrain-converter.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "plateau-mb-terrain-converter.h"
#include "WTMCalculator.h"
#include "CityGMLManager.h"
#include "WebTileManager.h"

#include <gdal_priv.h>
#include <stdexcept>
#include <filesystem>
#include <regex>


PlateauMapboxTerrainConverter::PlateauMapboxTerrainConverter(
    const std::string& strInputTerrainCityGML,
    const std::string& strOutputTileDirectory,
    const int nMinZoomLevel,
    const int nMaxZoomLevel,
    const bool bOverwrite,
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
    mpWebTileManager = std::make_unique<WebTileManager>( strOutputTileDirectory, nMinZoomLevel, nMaxZoomLevel, bOverwrite, mfnMessageFeedback, mfnProgressFeedback );
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
        for ( auto& pix : vGridInfo )
        {
            mpWebTileManager->pushPixelInfo( pix );
        }
        if ( mfnProgressFeedback )
        {
            mfnProgressFeedback( nProcessedTriangle++ );
        }
    }

    mpWebTileManager->finalizePushing();

    mpWebTileManager->createTilesFromDB();
}


void PlateauMapboxTerrainConverter::mergeTilesets( 
    const std::vector<std::string> &vstrInputDirs,
    const std::string& strOutDir, 
    const bool bOverwrite,
    const std::function<void(MESSAGE_STATUS, const std::string&)> &fnMessageFeedback,
    const std::function<void(int)> &fnProgressFeedback 
    )
{
    std::filesystem::path pathSrc1( vstrInputDirs[0] );
//    std::filesystem::path pathSrc2( strSourceDir2 );
    std::filesystem::path pathOutDir( strOutDir );

    if ( !std::filesystem::exists( pathSrc1 ) )
    {
        if ( fnMessageFeedback )
        {
            fnMessageFeedback( PlateauMapboxTerrainConverter::MESSAGE_ERROR,
                std::string( "input PLATEAU tile is not exists " ) + pathSrc1.u8string().c_str() );
            return;
        }
    }

    //if ( !std::filesystem::exists( strSourceDir2 ) )
    //{
    //    if ( fnMessageFeedback )
    //    {
    //        fnMessageFeedback( PlateauMapboxTerrainConverter::MESSAGE_ERROR,
    //            std::string( "input PLATEAU tile is not exists " ) + strSourceDir2 );
    //        return;
    //    }
    //}

    std::filesystem::copy( pathSrc1, pathOutDir, std::filesystem::copy_options::recursive );

    std::regex reTileDir( "[0-9]+" );
    
    for ( auto iter = vstrInputDirs.begin() + 1; iter != vstrInputDirs.end(); iter++ )
    {
        fnMessageFeedback( PlateauMapboxTerrainConverter::MESSAGE_INFO,
            std::string( "merge " ) + *iter );
        for ( const std::filesystem::directory_entry& eZ :
            std::filesystem::directory_iterator( *iter ) )
        {
            if ( eZ.is_directory() && 
                std::regex_match( eZ.path().stem().u8string(), reTileDir ) )
            {
                auto pathZFolder = eZ.path();
                std::filesystem::path pathDestinationZ = pathOutDir;
                pathDestinationZ /= eZ.path().stem();
                if ( !std::filesystem::exists( pathDestinationZ ) )
                {
                    std::filesystem::copy( pathZFolder, pathDestinationZ, std::filesystem::copy_options::recursive );
                    continue;
                }
                for ( const std::filesystem::directory_entry& eX :
                    std::filesystem::directory_iterator( pathZFolder ) )
                {
                    if ( eX.is_directory() &&
                        std::regex_match( eX.path().stem().u8string(), reTileDir ) )
                    {
                        auto pathXFolder = eX.path();
                        std::filesystem::path pathDestinationX = pathOutDir;
                        pathDestinationX /= eZ.path().stem();
                        pathDestinationX /= eX.path().stem();
                        if ( !std::filesystem::exists( pathDestinationX ) )
                        {
                            std::filesystem::copy( pathXFolder, pathDestinationX, std::filesystem::copy_options::recursive );
                            continue;
                        }
                        for ( const std::filesystem::directory_entry& eY :
                            std::filesystem::directory_iterator( pathXFolder ) )
                        {
                            if ( eY.is_regular_file() &&
                                std::regex_match( eY.path().stem().u8string(), reTileDir ) )
                            {
                                //std::cout << "    " << eY.path().stem() << std::endl;
                                std::filesystem::path pathDestinationY = pathOutDir;
                                pathDestinationY /= eZ.path().stem();
                                pathDestinationY /= eX.path().stem();
                                pathDestinationY /= eY.path().filename();
                                if ( std::filesystem::exists( pathDestinationY ) )
                                {
                                    WebTileManager::mergePng( eY.path().u8string(), pathDestinationY.u8string(), bOverwrite, fnMessageFeedback );
                                }
                                else
                                {
                                    std::filesystem::copy( eY, pathDestinationY );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
