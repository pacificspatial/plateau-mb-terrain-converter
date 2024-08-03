

#include "CommonStruct.h"

#include <string>
#include <gdal_priv.h>
#include <deque>


class CityGMLManager
{
public:
	CityGMLManager() = delete;
	CityGMLManager( const std::string &strFName );
	virtual ~CityGMLManager();

	bool getNextTriangle( OGRPoint &p1, OGRPoint &p2, OGRPoint &p3 );
	const OGRSpatialReference* getSpatialRef() const;

	inline bool isValid() const { return mbValid; }

private:
	bool mbValid;

	int mnCurrentLayer;
	std::deque<int> mvTerrainLayersNum;

	int mnCurrentFeature;

	int mnCurrentTriangle;

	std::string mstrErrorMsg;

	GDALDatasetUniquePtr mpDS;
	OGRLayer *mpCurrentLayer;
	OGRFeature *mpCurrentFeature;
	OGRTriangulatedSurface *mpCurrentGeom;
};