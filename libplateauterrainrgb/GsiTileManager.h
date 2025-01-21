#pragma once

#include "WebTileManager.h"


class GsiTileManager : public WebTileManager
{
public:
	GsiTileManager() = delete;
	GsiTileManager( const std::string &strOutputDirectory,
		const int nMinZoomLevel,
		const int nMaxZoomLevel,
		const bool bOverwrite = false,
		const std::function<void(MESSAGE_STATUS, std::string)> &fnMessageFeedback = nullptr,
		const std::function<void(int)> &fnProgressFeedback = nullptr
	);
	virtual ~GsiTileManager();

	inline bool isValid() const { return mbValid; }
	bool createBaseTilesFromImage( 
		WTM_BBOX bbox_bl, 
		int nWidth, 
		int nHeight, 
		double *pData, 
		std::shared_ptr<WTMCalculator> calculator 
	);
	bool createOverviews();

private:
	std::vector<TILE_COORD> mvTiles;
};
