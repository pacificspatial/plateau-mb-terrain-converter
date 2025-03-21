/***************************************************************************
                        CityGMLReader.cpp  -  description
                                 -------------------

        Read and manage CityGML file data.

        begin                : Jan. 21, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/

#include "CityGMLReader.h"

#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <iostream>

namespace pmtc
{

    CityGMLReader::CityGMLReader(
        const std::string &strFName)
        : mbValid(false),
        mpCurrentLayer(nullptr),
        mdFilterLonMin(-1.0),
        mdFilterLonMax(-1.0),
        mdFilterLatMin(-1.0),
        mdFilterLatMax(-1.0)
    {
        GDALAllRegister();
        CPLSetConfigOption("GML_SRS_DIMENSION_IF_MISSING", "3");

        char **papszAllowedDrivers = nullptr;

        papszAllowedDrivers = CSLAddString(papszAllowedDrivers, "GML");
        mpDS = GDALDatasetUniquePtr(
            GDALDataset::Open(strFName.c_str(), GDAL_OF_VECTOR | GDAL_OF_READONLY, papszAllowedDrivers, NULL));

        if (!mpDS)
        {
            mstrErrorMsg = "unable to open file : [" + strFName + "]";
            return;
        }

        for (int i = 0; i < mpDS->GetLayerCount(); i++)
        {
            OGRLayer *pLayer = mpDS->GetLayer(i);
            if (pLayer->GetGeomType() == wkbTINZ)
            {
                if (pLayer->GetFeatureCount() > 0)
                {
                    mvTerrainLayersNum.push_back(i);
                    if (i == 0)
                    {
                        pLayer->GetExtent(&mEnvelop);
                    }
                    else
                    {
                        OGREnvelope envelop;
                        pLayer->GetExtent(&envelop);
                        mEnvelop.Merge(envelop);
                    }
                }
                pLayer->GetFeature(0);
            }
        }

        if (mvTerrainLayersNum.size() == 0)
        {
            mstrErrorMsg = "no terrain layer found.";
            return;
        }

        mnCurrentLayer = 0;
        mpCurrentLayer = mpDS->GetLayer(mvTerrainLayersNum.at(mnCurrentLayer));
        mpCurrentLayer->ResetReading();
        mnCurrentFeature = 0;
        mpCurrentFeature = mpCurrentLayer->GetNextFeature();
        if (!mpCurrentFeature)
        {
            mstrErrorMsg = "cannot read featurs.";
            return;
        }

        mnCurrentTriangle = 0;
        auto geom = mpCurrentFeature->GetGeometryRef();
        mpCurrentGeom = geom->toTriangulatedSurface();

        mbValid = true;
    }

    CityGMLReader::~CityGMLReader()
    {
        if (mpCurrentFeature)
        {
            OGRFeature::DestroyFeature(mpCurrentFeature);
        }
    }

    void CityGMLReader::reset()
    {
        mnCurrentLayer = 0;
        mpCurrentLayer->ResetReading();
        mnCurrentFeature = 0;
        mpCurrentFeature = mpCurrentLayer->GetNextFeature();
        mnCurrentTriangle = 0;
        auto geom = mpCurrentFeature->GetGeometryRef();
        mpCurrentGeom = geom->toTriangulatedSurface();
    }

    const OGRSpatialReference *CityGMLReader::getSpatialRef() const
    {
        if (!mbValid)
            return nullptr;

        auto cs = mpDS->GetSpatialRef();
        if (!cs)
            return nullptr;

        return cs;
    }

    const OGREnvelope CityGMLReader::getExtent() const
    {
        return mEnvelop;
    }

    void CityGMLReader::setSpatialFilter(const double dLonMin, const double dLonMax, const double dLatMin, const double dLatMax)
    {
        mdFilterLonMin = dLonMin;
        mdFilterLonMax = dLonMax;
        mdFilterLatMin = dLatMin;
        mdFilterLatMax = dLatMax;
        mpCurrentLayer->SetSpatialFilterRect(mdFilterLonMin, mdFilterLatMin, mdFilterLonMax, mdFilterLatMax);
    }

    bool CityGMLReader::getNextTriangle(OGRPoint &p1, OGRPoint &p2, OGRPoint &p3)
    {
        if (!mbValid)
        {
            return false;
        }

        if (mnCurrentTriangle >= mpCurrentGeom->getNumGeometries())
        {
            mnCurrentFeature++;
            if (mnCurrentFeature >= mpCurrentLayer->GetFeatureCount())
            {
                mnCurrentLayer++;
                if (mnCurrentLayer >= mvTerrainLayersNum.size())
                {
                    reset();
                    return false;
                }
                mpCurrentLayer = mpDS->GetLayer(mvTerrainLayersNum.at(mnCurrentLayer));
                mpCurrentLayer->SetSpatialFilterRect(mdFilterLonMin, mdFilterLatMin, mdFilterLonMax, mdFilterLatMax);
                mnCurrentFeature = 0;
            }
            OGRFeature::DestroyFeature(mpCurrentFeature);
            mpCurrentFeature = mpCurrentLayer->GetNextFeature();
            auto geom = mpCurrentFeature->GetGeometryRef();
            mpCurrentGeom = geom->toTriangulatedSurface();
            mnCurrentTriangle = 0;
        }

        auto geomTriRing = mpCurrentGeom->getGeometryRef(mnCurrentTriangle)->getExteriorRing();
        geomTriRing->getPoint(0, &p1);
        geomTriRing->getPoint(1, &p2);
        geomTriRing->getPoint(2, &p3);
        mnCurrentTriangle++;

        return true;
    }
}
