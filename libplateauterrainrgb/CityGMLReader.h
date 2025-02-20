/***************************************************************************
											 CityGMLReader.h  -  description
														 -------------------

		Read and manage CityGML file data.

		begin                : Jan. 21, 2025
		Copyright            : (c) 2025 MLIT Japan.
												 : (c) 2025 Pacific Spatial Solutions Inc.
		author               : Yamate, N
 ***************************************************************************/

#include "CommonStruct.h"

#include <string>
#include <gdal_priv.h>
#include <deque>
#include <ogr_core.h>
#include <functional>

class CityGMLReader
{
public:
	CityGMLReader() = delete;
	CityGMLReader(const std::string &strFName);
	virtual ~CityGMLReader();

	bool getNextTriangle(OGRPoint &p1, OGRPoint &p2, OGRPoint &p3);
	const OGRSpatialReference *getSpatialRef() const;
	void setSpatialFilter(const double dLonMin, const double dLonMax, const double dLatMin, const double dLatMax);
	const OGREnvelope &getExtent() const;

	inline bool isValid() const { return mbValid; }
	inline std::string getLastError() const { return mstrErrorMsg; }

private:
	bool mbValid;

	int mnCurrentLayer;
	std::deque<int> mvTerrainLayersNum;
	void reset();

	double mdFilterLonMin;
	double mdFilterLonMax;
	double mdFilterLatMin;
	double mdFilterLatMax;

	int mnCurrentFeature;

	int mnCurrentTriangle;

	std::string mstrErrorMsg;

	GDALDatasetUniquePtr mpDS;
	OGRLayer *mpCurrentLayer;
	OGRFeature *mpCurrentFeature;
	OGRTriangulatedSurface *mpCurrentGeom;
	OGREnvelope mEnvelop;
};