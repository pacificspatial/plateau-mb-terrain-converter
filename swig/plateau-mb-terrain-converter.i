/***************************************************************************
							plateau-mb-terrain-converter.i  -  description
														 -------------------

		SWIG interface of libplateauterrainrgb library

		begin                : Jan. 21, 2025
		Copyright            : (c) 2025 MLIT Japan.
												 : (c) 2025 Pacific Spatial Solutions Inc.
		author               : Yamate, N
 ***************************************************************************/

%module(directors = "1") plateaumbterrainconverter;
%include<std_string.i> 
%include<std_vector.i> 
%include<windows.i>

%template(VStr) std::vector<std::string>;

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
		virtual void messageFeedback(MESSAGE_STATUS eStatus, const std::string &strMessage) = 0;
		virtual void progressFeedback(int nProgress) = 0;
		virtual ~PMTCFeedback() {}
	};
%}

%{
	static PMTCFeedback *gpFeedback = nullptr;
	static void messageCallbackHandler(MESSAGE_STATUS eStatus, const std::string &strMessage)
	{
		if (gpFeedback)
		{
			gpFeedback->messageFeedback(eStatus, strMessage);
		}
	}
	static void progressCallbackHandler(int nProgress)
	{
		if (gpFeedback)
		{
			gpFeedback->progressFeedback(nProgress);
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
	catch ([[maybe_unused]] std::range_error &e)
	{
		SWIG_exception(SWIG_ValueError, "Range Error");
	}
	catch (...)
	{
		SWIG_exception(SWIG_RuntimeError, "Unknown exception");
	}
}

%inline %{
	inline bool CreatePlateauTileset(
			const std::string &strInputTerrainCityGML,
			const std::string &strOutputTileDirectory,
			const int nMinZoomLevel,
			const int nMaxZoomLevel,
			const bool bOverwrite,
			PMTCFeedback *pFeedback)
	{
		gpFeedback = pFeedback;
		return pmtc::createPlateauTileset(
				strInputTerrainCityGML,
				strOutputTileDirectory,
				nMinZoomLevel,
				nMaxZoomLevel,
				bOverwrite,
				&messageCallbackHandler,
				&progressCallbackHandler);
	}

	inline bool CreateGsiTileset(
			const std::string &strInputGsiGml,
			const std::string &strOutputTileDirectory,
			const int nMinZoomLevel,
			const int nMaxZoomLevel,
			const bool bOverwrite,
			PMTCFeedback *pFeedback)
	{
		gpFeedback = pFeedback;
		return pmtc::createGsiTileset(
				strInputGsiGml,
				strOutputTileDirectory,
				nMinZoomLevel,
				nMaxZoomLevel,
				bOverwrite,
				&messageCallbackHandler,
				&progressCallbackHandler);
	}

	inline static void MergeTilesets(
			const std::vector<std::string> &vstrInputDirs,
			const std::string &strOutDir,
			const bool bOverwrite,
			PMTCFeedback *pFeedback)
	{
		gpFeedback = pFeedback;
		pmtc::mergeTilesets(
				vstrInputDirs, strOutDir, bOverwrite,
				&messageCallbackHandler, &progressCallbackHandler);
	}

	inline static void Fill_zero(
			const std::string &strTileDir,
			PMTCFeedback *pFeedback)
	{
		gpFeedback = pFeedback;
		pmtc::fill_zero(
				strTileDir,
				&messageCallbackHandler, &progressCallbackHandler);
	}
%}
