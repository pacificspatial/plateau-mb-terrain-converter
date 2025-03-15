/***************************************************************************
                       GsiGmlReader.h  -  description
                            -------------------

        Read and manage GSI Dem file data.

        begin                : Jan. 21, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/

#pragma once

#include "CommonStruct.h"
#include <string>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

namespace pmtc
{
    class GsiGmlReader
    {
    public:
        GsiGmlReader() = delete;
        GsiGmlReader(const std::string &strFName);
        ~GsiGmlReader();

        inline bool isValid() const { return mbValid; }
        inline std::string getErrorMsg() const { return mstrErrorMsg; }
        void getImage(WTM_BBOX *bbox, uint32_t *nWidth, uint32_t *nHeight, double **pData);

    private:
        bool mbValid;

        void parseHeightList(const xmlChar *pszData);
        bool processNode(xmlTextReaderPtr pReader);
        void createImage();

        uint32_t mStateFlags;
        WTM_BBOX mrcDemRect;
        PIXEL_COORD mpntLow, mpntHigh;
        PIXEL_COORD mpntStartPoint;
        std::vector<double> mvData;
        double *mpData;

        std::string mstrFName;
        std::string mstrErrorMsg;
    };
}
