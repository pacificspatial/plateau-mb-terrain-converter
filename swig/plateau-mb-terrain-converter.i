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

%include "exception.i"

%exception 
{
	try
	{
		$action
	}
	catch (std::range_error &e)
	{
		SWIG_exception(SWIG_ValueError, "Range Error");
	}
	catch (...)
	{
		SWIG_exception(SWIG_RuntimeError, "Unknown exception");
	}
}


%inline %{
class PyPlateauMapboxTerrainConverter
{
public:
	inline PyPlateauMapboxTerrainConverter(
        const std::string &strInputTerrainCityGML, 
        const std::string &strOutputTileDirectory, 
        const int nMinZoomLevel, 
        const int nMaxZoomLevel,
        const bool bOverwrite,
		PMTCFeedback *pFeedback
	)
	{
		gpFeedback = pFeedback;
		mObj = std::make_unique<PlateauMapboxTerrainConverter>(
			strInputTerrainCityGML,
			strOutputTileDirectory,
			nMinZoomLevel,
			nMaxZoomLevel,
			bOverwrite,
			&messageCallbackHandler,
			&progressCallbackHandler
		);
	}

	inline void createTileset(){ if ( mObj->isValid() ) mObj->createTileset(); }
	inline bool isValid() const { return mObj->isValid(); }

    inline static void mergeTilesets( 
        const std::vector<std::string> &vstrInputDirs,
        const std::string& strOutDir, 
        const bool bOverwrite,
		PMTCFeedback *pFeedback
        )
	{
		gpFeedback = pFeedback;
		PlateauMapboxTerrainConverter::mergeTilesets(
			vstrInputDirs, strOutDir, bOverwrite,
			&messageCallbackHandler, &progressCallbackHandler
		);
	}

private:
	std::unique_ptr<PlateauMapboxTerrainConverter> mObj = nullptr;
};
%}
