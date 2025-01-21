#pragma once


#include "plateau-mb-terrain-converter.h"
#include "CommonStruct.h"

#include <string>
#include <filesystem>
#include <fstream>
#include <functional>

//#if 0
#include <sqlite3.h>
//#endif


class WebTileManager
{
public:
	WebTileManager() = delete;
	WebTileManager( const std::string &strOutputDirectory,
					const int nMinZoomLevel,
					const int nMaxZoomLevel,
					const bool bOverwrite = false,
					const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr,
					const std::function<void(int)> &fnProgressFeedback = nullptr
	);
	virtual ~WebTileManager();

	inline bool isValid() const { return mbValid; }
	//inline std::string getDbError() const { return mstrErrorMsg; }

	static bool mergePng( const std::string& strSrcFName, const std::string& strDstFName, bool bOverwrite,
		const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr );
	static bool fill_zeroPng( const std::string& strFName );
#if 0
	bool createTilesFromFile();
#endif

protected:
#if 0
	bool writePng( const std::string strFName, uint8_t *pImgR, uint8_t *pImgG, uint8_t *pImgB, uint8_t *pImgA );
#endif
	static bool writePng( const std::string &strFName, uint8_t *pImg );
	static bool mergePng( const std::string &strFName, uint8_t *pImg, bool bOverwrite = false, 
		const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr );
	static bool readPng( const std::string strFName, uint8_t **pImg );
	std::string makeOutputFilePath( const std::filesystem::path pathBase, const int nX, const int nY, const int nZ );
	bool createDirectoryFromTilePath( const std::filesystem::path pathTileName );
	bool buildOverviews( std::vector<TILE_COORD> &vBaseTiles );
	bool createOverviewTileFromQuadTiles( 
		const std::filesystem::path &pathOutput,
		const std::filesystem::path &pathTileTL, 
		const std::filesystem::path &pathTileTR, 
		const std::filesystem::path &pathTileBL, 
		const std::filesystem::path &pathTileBR );
	std::vector<TILE_COORD> getOverviewTileList( std::vector<TILE_COORD> &vTileList );

	bool mbValid;
	bool mbOverwrite;

	int mnMinZoomLevel;
	int mnMaxZoomLevel;

	//std::string mstrErrorMsg;
	std::filesystem::path mpathOutputDirectory;
	std::function<void(MESSAGE_STATUS, std::string)> mfnMessageFeedback;
	std::function<void(int)> mfnProgressFeedback;

#if 0
	std::fstream mstmInputBaseTile;
	std::fstream mstmOutputBaseTile;
#endif
};

