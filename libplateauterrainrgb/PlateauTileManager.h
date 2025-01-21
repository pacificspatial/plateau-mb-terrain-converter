#pragma once

#include "WebTileManager.h"


class PlateauTileManager : public WebTileManager
{
public:
	PlateauTileManager() = delete;
	PlateauTileManager( const std::string &strOutputDirectory,
		const int nMinZoomLevel,
		const int nMaxZoomLevel,
		const bool bOverwrite = false,
		const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr,
		const std::function<void(int)> &fnProgressFeedback = nullptr
	);
	virtual ~PlateauTileManager();

	bool pushPixelInfo( const TILE_PIXEL_INFO &info );
	void finalizePushing();
	bool createTilesFromDB();

private:
	int mnPushCount;
	sqlite3 *mpDb;
	sqlite3_stmt *mpStmt;
};
