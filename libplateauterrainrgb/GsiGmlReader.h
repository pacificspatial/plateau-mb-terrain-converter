#pragma once

#include "CommonStruct.h"
#include <string>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>


class GsiGmlReader
{
public:
	GsiGmlReader() = delete;
	GsiGmlReader( const std::string &strFName	);
	~GsiGmlReader();

	inline bool isValid() const { return mbValid; }
	inline std::string getErrorMsg() const { return mstrErrorMsg; }
	void getImage( WTM_BBOX *bbox, int *nWidth, int *nHeight, double **pData );

private:
	bool mbValid;

	void parseHeightList( const xmlChar *pszData );
	bool processNode( xmlTextReaderPtr pReader );
	void createImage();

	uint32_t mStateFlags;
	WTM_BBOX mrcDemRect;
	PIXEL_COORD mpntLow, mpntHigh;
	PIXEL_COORD mpntStartPoint;
	//uint32_t mnWidth, mnHeight;
	//uint8_t *gpData;
	std::vector<double> mvData;
	double *mpData;

	std::string mstrFName;
	std::string mstrErrorMsg;
};

