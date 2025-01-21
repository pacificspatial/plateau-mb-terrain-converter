
#include "GsiTileManager.h"
#include "WTMCalculator.h"


GsiTileManager::GsiTileManager( 
	const std::string& strOutputDirectory,
	const uint32_t nMinZoomLevel,
	const uint32_t nMaxZoomLevel,
	const bool bOverwrite,
	const std::function<void( MESSAGE_STATUS, std::string )>& fnMessageFeedback,
	const std::function<void( int )>& fnProgressFeedback
)
	:
	WebTileManager( 
		strOutputDirectory, 
		nMinZoomLevel, 
		nMaxZoomLevel, 
		bOverwrite, 
		fnMessageFeedback, 
		fnProgressFeedback 
	)
{
	if ( !std::filesystem::exists( mpathOutputDirectory ) )
	{
		if ( mfnMessageFeedback )
		{
			mfnMessageFeedback( 
				MESSAGE_WARNING,
				"output directory does not exist [ " + mpathOutputDirectory.u8string() + " ]." );
		}
	}

	if ( !std::filesystem::is_directory( mpathOutputDirectory ) )
	{
		if ( mfnMessageFeedback )
		{
			mfnMessageFeedback( 
				MESSAGE_ERROR,
				"output is not a directory [ " + mpathOutputDirectory.u8string() + " ]." );
		}
		return;
	}

	if ( mnMinZoomLevel < 0 || mnMaxZoomLevel < 0 ||
		( mnMinZoomLevel > mnMaxZoomLevel )
		)
	{
		mfnMessageFeedback(
			MESSAGE_ERROR,
			"Invalid zoom level : " + std::to_string(mnMinZoomLevel) + " - " + std::to_string(mnMaxZoomLevel) );
		return;
	}

	if ( mnMaxZoomLevel >= 18 )
	{
		mfnMessageFeedback(
			MESSAGE_WARNING,
			"Zoom levels above 18 can result in too many files being generated."
		);
	}

	std::filesystem::path pathDB = mpathOutputDirectory;
	pathDB /= "tempdb.sqlite";
	if ( std::filesystem::exists( pathDB ) )
	{
		std::filesystem::remove( pathDB );
	}

	mbValid = true;
	return;
}


GsiTileManager::~GsiTileManager()
{

}


bool GsiTileManager::createOverviews()
{
	return buildOverviews( mvTiles );
}


bool GsiTileManager::createBaseTilesFromImage( 
	WTM_BBOX bbox_bl, 
	const uint32_t nWidth, 
	const uint32_t nHeight, 
	double* pData, 
	std::shared_ptr<WTMCalculator> calculator
)
{
	bool bRes;
	PIXEL_COORD pixTL, pixBR;

	calculator->transformLatLonToWTM( bbox_bl.tl );
	calculator->transformLatLonToWTM( bbox_bl.br );
	double dOrigRes_X = (bbox_bl.br.getX() - bbox_bl.tl.getX()) / nWidth;
	double dOrigRes_Y = (bbox_bl.tl.getY() - bbox_bl.br.getY()) / nHeight;

	auto sWTMResolution = WTMCalculator::calcWTMResolution( mnMaxZoomLevel, TILE_PIXELS );

	//auto pixTL = calculator->calcTotalPixelCoord( bbox_bl.tl );
	//auto pixBR = calculator->calcTotalPixelCoord( bbox_bl.br );

	double dWX = bbox_bl.tl.getX() - WTM_MIN_X;
	double dWY = WTM_MAX_Y - bbox_bl.tl.getY();
	pixTL.nU = (uint32_t)floor( dWX / sWTMResolution.getX() );
	pixTL.nV = (uint32_t)floor( dWY / sWTMResolution.getY() );
	dWX = bbox_bl.br.getX() - WTM_MIN_X;
	dWY = WTM_MAX_Y - bbox_bl.br.getY();
	pixBR.nU = (uint32_t)ceil( dWX / sWTMResolution.getX() );
	pixBR.nV = (uint32_t)ceil( dWY / sWTMResolution.getY() );

	auto infoTL = calculator->calcTilePixelCoordFromTotalPixelCoord( pixTL );
	auto infoBR = calculator->calcTilePixelCoordFromTotalPixelCoord( pixBR );

	for ( uint32_t nTileY = infoTL.tileNum.nY; 
		nTileY <= infoBR.tileNum.nY; 
		nTileY++ )
	{
		//std::cout << nTileY << std::endl;

		for ( uint32_t nTileX = infoTL.tileNum.nX;
			nTileX <= infoBR.tileNum.nX;
			nTileX++ )
		{
			uint8_t *pImgBuf = static_cast<uint8_t *>( CPLCalloc( TILE_PIXELS*TILE_PIXELS*4, 1 ) );
			uint64_t nSX = nTileX == infoTL.tileNum.nX ? infoTL.pixCoord.nU : 0;
			uint64_t nEX = nTileX == infoBR.tileNum.nX ? infoBR.pixCoord.nU : TILE_PIXELS - 1;
			uint64_t nSY = nTileY == infoTL.tileNum.nY ? infoTL.pixCoord.nV : 0;
			uint64_t nEY = nTileY == infoBR.tileNum.nY ? infoBR.pixCoord.nV : TILE_PIXELS - 1;

			//std::cout << nSX << "|" << nSY << "|" << nEX << "|" << nEY << std::endl;

			for ( uint64_t i = nSY; i <= nEY; i++ )
			{
				double dWTM_Y = WTM_MAX_Y - (nTileY*TILE_PIXELS + i + 0.5)*sWTMResolution.getY();
				if ( dWTM_Y > bbox_bl.tl.getY() || dWTM_Y < bbox_bl.br.getY() )
				{
					continue;
				}
				for ( uint64_t j = nSX; j <= nEX; j++ )
				{
					double dWTM_X = WTM_MIN_X + (nTileX*TILE_PIXELS + j + 0.5)*sWTMResolution.getX();
					if ( dWTM_X < bbox_bl.tl.getX() || dWTM_X > bbox_bl.br.getX() )
					{
						continue;
					}

					//std::cout << std::fixed << std::setprecision(10) << dWTM_X << "|" << dWTM_Y << std::endl;

					// NN
					uint32_t nU = (uint32_t)floor( ((dWTM_X - bbox_bl.tl.getX()) / dOrigRes_X) /*+ 0.5*/ );
					uint32_t nV = (uint32_t)floor( ((bbox_bl.tl.getY() - dWTM_Y) / dOrigRes_Y) /*+ 0.5*/ );
					//std::cout << std::fixed << std::setprecision(10) << bbox_bl.tl.getX() << "|" << bbox_bl.tl.getY() << std::endl;

					auto pixInfo = calculator->calcPix( pData[nV*nWidth + nU] );
					pImgBuf[i*TILE_PIXELS*4 + j*4 + 0] = pixInfo.nR;
					pImgBuf[i*TILE_PIXELS*4 + j*4 + 1] = pixInfo.nG;
					pImgBuf[i*TILE_PIXELS*4 + j*4 + 2] = pixInfo.nB;
					pImgBuf[i*TILE_PIXELS*4 + j*4 + 3] = pixInfo.nA;
				}
			}
			auto strFName = makeOutputFilePath( mpathOutputDirectory, nTileX, nTileY, mnMaxZoomLevel );
			if ( !createDirectoryFromTilePath( std::filesystem::path(strFName) ) )
			{
				CPLFree( pImgBuf );
				return false;
			}

			if ( std::filesystem::exists( std::filesystem::path( strFName ) ) )
			{
				bRes = mergePng( strFName, pImgBuf, mbOverwrite, mfnMessageFeedback );
			}
			else
			{
				auto path = std::filesystem::path( strFName );
				bRes = writePng( strFName, pImgBuf );
			}
			TILE_COORD coord = {nTileX, nTileY, mnMaxZoomLevel};
			mvTiles.push_back( coord );
			CPLFree( pImgBuf );
		}
	}

	return true;
}
