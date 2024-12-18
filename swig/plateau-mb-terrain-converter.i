%module(directors="1") plateaumbterrainconverter;
%include <std_string.i>
%include <windows.i>

%{
#include "../libplateauterrainrgb/plateau-mb-terrain-converter.h"
#include <memory>
#include <iostream>
#include <functional>
%}

%include "../libplateauterrainrgb/plateau-mb-terrain-converter.h"

%feature("director") PMTCFeedback;

%inline %{
class PMTCFeedback
{
public:
	virtual void messageFeedback(PlateauMapboxTerrainConverter::MESSAGE_STATUS eStatus, const std::string &strMessage ) = 0;
	virtual void progressFeedback( int nProgress ) = 0;
	virtual ~PMTCFeedback(){}
};
%}

%{
static PMTCFeedback *gpFeedback = nullptr;
static void messageCallbackHandler( PlateauMapboxTerrainConverter::MESSAGE_STATUS eStatus, const std::string &strMessage )
{
	if ( gpFeedback )
	{
		gpFeedback->messageFeedback( eStatus, strMessage );
	}
}
static void progressCallbackHandler( int nProgress )
{
	if ( gpFeedback )
	{
		gpFeedback->progressFeedback( nProgress );
	}
}
%}


%inline %{
class PyPlateauMapboxTerrainConverter
{
public:
	inline PyPlateauMapboxTerrainConverter(
        const std::string &strInputTerrainCityGML, 
        const std::string &strOutputTileDirectory, 
        const int nMinZoomLevel, 
        const int nMaxZoomLevel,
		PMTCFeedback *pFeedback
	)
	{
		gpFeedback = pFeedback;
		mObj = std::make_unique<PlateauMapboxTerrainConverter>(
			strInputTerrainCityGML,
			strOutputTileDirectory,
			nMinZoomLevel,
			nMaxZoomLevel,
			&messageCallbackHandler,
			&progressCallbackHandler
		);
	}

	inline void createTileset(){ if ( mObj->isValid() ) mObj->createTileset(); }
	inline bool isValid() const { return mObj->isValid(); }

    inline static void mergeTilesets( 
        const std::string& strSourceDir1, 
        const std::string& strSourceDir2, 
        const std::string& strOutDir, 
		PMTCFeedback *pFeedback
        )
	{
		gpFeedback = pFeedback;
		PlateauMapboxTerrainConverter::mergeTilesets(
			strSourceDir1, strSourceDir2, strOutDir, &messageCallbackHandler, &progressCallbackHandler
		);
	}

private:
	std::unique_ptr<PlateauMapboxTerrainConverter> mObj = nullptr;
};
%}
