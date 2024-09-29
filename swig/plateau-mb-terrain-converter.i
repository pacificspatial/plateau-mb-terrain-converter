%module(directors="1") plateaumbterrainconverter;
%include <std_string.i>
%include <windows.i>

%{
#include "../plateau-mb-terrain-converter/plateau-mb-terrain-converter.h"
#include <memory>
#include <iostream>
#include <functional>
%}

%include "../plateau-mb-terrain-converter/plateau-mb-terrain-converter.h"

%feature("director") Feedback;

%inline %{
class Feedback
{
public:
	virtual void messageFeedback(PlateauMapboxTerrainConverter::MESSAGE_STATUS eStatus, const std::string &strMessage ) = 0;
	virtual void progressFeedback( int nProgress ) = 0;
	virtual ~Feedback(){}
};
%}

%{
static Feedback *gpFeedback = nullptr;
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
		Feedback *pFeedback
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

	inline void progress(){ if ( mObj->isValid() ) mObj->createTileset(); }

private:
	std::unique_ptr<PlateauMapboxTerrainConverter> mObj = nullptr;
};
%}
