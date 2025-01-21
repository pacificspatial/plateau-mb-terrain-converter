

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
	CityGMLReader( const std::string &strFName	);
	virtual ~CityGMLReader();

	bool getNextTriangle( OGRPoint &p1, OGRPoint &p2, OGRPoint &p3 );
	const OGRSpatialReference* getSpatialRef() const;
	void setSpatialFilter( const double dLonMin, const double dLonMax, const double dLatMin, const double dLatMax );
	const OGREnvelope& getExtent() const;

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
//	std::function<void(std::string)> mfnMessageFeedback;

	GDALDatasetUniquePtr mpDS;
	OGRLayer *mpCurrentLayer;
	OGRFeature *mpCurrentFeature;
	OGRTriangulatedSurface *mpCurrentGeom;
	OGREnvelope mEnvelop;
};