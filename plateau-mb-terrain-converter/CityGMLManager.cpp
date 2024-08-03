
#include "CityGMLManager.h"

#include <gdal_priv.h>
#include <ogrsf_frmts.h>


CityGMLManager::CityGMLManager( const std::string& strFName )
	:
	mbValid( false ),
	mpCurrentLayer( nullptr )
{
	GDALAllRegister();
	char **papszAllowedDrivers = nullptr;

	papszAllowedDrivers = CSLAddString( papszAllowedDrivers, "GML" );
	mpDS =  GDALDatasetUniquePtr(
		GDALDataset::Open( strFName.c_str(), GDAL_OF_VECTOR | GDAL_OF_READONLY, papszAllowedDrivers, NULL ) );

	if ( !mpDS )
	{
		mstrErrorMsg = "unable to open file : [" + strFName + "]";
		return;
	}

	for ( int i = 0; i < mpDS->GetLayerCount(); i++ )
	{
		OGRLayer *pLayer = mpDS->GetLayer( i );
		if ( pLayer->GetGeomType() == wkbTINZ )
		{
			mvTerrainLayersNum.push_back( i );
			pLayer->GetFeature( 0 );
		}
	}

	//mnLayerCount = mpDS->GetLayerCount();

	if ( mvTerrainLayersNum.size() == 0 )
	{
		mstrErrorMsg = "no terrain layer found.";
		return;
	}

	mnCurrentLayer = mvTerrainLayersNum.front();

	mnCurrentLayer = 0;
	mpCurrentLayer = mpDS->GetLayer( mnCurrentLayer );

	mnCurrentFeature = 0;
	mpCurrentFeature = mpCurrentLayer->GetFeature( mnCurrentFeature );

	mnCurrentTriangle = 0;
	auto geom = mpCurrentFeature->GetGeometryRef();
	mpCurrentGeom = geom->toTriangulatedSurface();

	mbValid = true;
}


CityGMLManager::~CityGMLManager()
{

}


const OGRSpatialReference* CityGMLManager::getSpatialRef() const
{
	if ( mbValid ) return nullptr;

	return mpDS->GetSpatialRef();
}


bool CityGMLManager::getNextTriangle( OGRPoint& p1, OGRPoint& p2, OGRPoint& p3 )
{
	if ( !mbValid )
	{
		return false;
	}

	if ( mnCurrentTriangle >= mpCurrentGeom->getNumGeometries() )
	{
		mnCurrentFeature++;
		if ( mnCurrentFeature >= mpCurrentLayer->GetFeatureCount() )
		{
			mvTerrainLayersNum.pop_front();
			mnCurrentLayer = mvTerrainLayersNum.front();
			if ( mnCurrentLayer >= mpDS->GetLayerCount() )
			{
				return false;
			}
			mpCurrentLayer = mpDS->GetLayer( mnCurrentLayer );
			mnCurrentFeature = 0;
		}
		mpCurrentFeature = mpCurrentLayer->GetFeature( mnCurrentFeature );
		auto geom = mpCurrentFeature->GetGeometryRef();
		mpCurrentGeom = geom->toTriangulatedSurface();
		mnCurrentTriangle = 0;
	}

	auto geomTriRing = mpCurrentGeom->getGeometryRef( mnCurrentTriangle )->getExteriorRing();
	geomTriRing->getPoint( 0, &p1 );
	geomTriRing->getPoint( 1, &p2 );
	geomTriRing->getPoint( 2, &p3 );
	mnCurrentTriangle++;

	return true;
}
