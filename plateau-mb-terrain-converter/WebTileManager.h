#pragma once


#include "CommonStruct.h"

#include <sqlite3.h>
#include <string>
#include <filesystem>


class WebTileManager
{
public:
	WebTileManager() = delete;
	WebTileManager( const std::string &strOutputDirectory,
					const int nMinZoomLevel,
					const int nMaxZoomLevel );
	virtual ~WebTileManager();

	inline bool isValid() const { return mbValid; }
	inline std::string getDbError() const { return mstrErrorMsg; }

	bool pushPixelInfo( const TILE_PIXEL_INFO &info );
	bool createTilesFromDB();

private:
	bool writePng( const std::string strFName, uint8_t *pImgR, uint8_t *pImgG, uint8_t *pImgB, uint8_t *pImgA );
	std::string makeOutputFilePath( std::filesystem::path pathBase, const int nX, const int nY, const int nZ );
	bool buildOverviews( std::vector<TILE_COORD> &vBaseTiles );
	bool createOverviewTileFromQuadTiles( 
		const std::filesystem::path &pathOutput,
		const std::filesystem::path &pathTileTL, 
		const std::filesystem::path &pathTileTR, 
		const std::filesystem::path &pathTileBL, 
		const std::filesystem::path &pathTileBR );
	std::vector<TILE_COORD>&& getOverviewTileList( std::vector<TILE_COORD> &vTileList );

	bool mbValid;

	int mnMinZoomLevel;
	int mnMaxZoomLevel;

	sqlite3 *mpDb;
	std::filesystem::path mpathOutputDirectory;
	std::string mstrErrorMsg;
};

