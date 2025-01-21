#include "GsiGmlReader.h"

#include <cstring>
#include <vector>
//#include <iostream>
#include <libxml/xmlreader.h>


#define GML_COVERAGE     1
#define GML_BOUND        1 << 1
#define GML_ENVELOPE     1 << 2
#define GML_LOWER_CORNER 1 << 3
#define GML_UPPER_CORNER 1 << 4
#define GML_GRID_DOMAIN  1 << 5
#define GML_GRID         1 << 6
#define GML_LOW          1 << 7
#define GML_HIGH         1 << 8
#define GML_RANGESET     1 << 9
#define GML_DATABLOCK    1 << 10
#define GML_TUPLELIST    1 << 11
#define GML_COVERAGEFUNC 1 << 12
#define GML_GRIDFUNC     1 << 13
#define GML_SEQUENCERULE 1 << 14
#define GML_STARTPOINT   1 << 15


GsiGmlReader::GsiGmlReader( const std::string& strFName )
	:
	mStateFlags( 0x00000000 ),
	mrcDemRect( {OGRPoint(-1.0, -1.0), OGRPoint(-1.0, -1.0)} ),
	mpntLow( {UINT_MAX, UINT_MAX} ),
	mpntHigh( {UINT_MAX, UINT_MAX} ),
	mpntStartPoint( {UINT_MAX, UINT_MAX} ),
	mstrFName( strFName ),
	mpData( nullptr ),
	mbValid( false )
{
	LIBXML_TEST_VERSION
	xmlDefaultBufferSize = 65535;

	static int options = XML_PARSE_BIG_LINES | XML_PARSE_HUGE;

	auto pReader = xmlReaderForFile( strFName.c_str(), nullptr, options );
	if ( !pReader )
	{
		mstrErrorMsg = "failed to open xml file " + strFName;
	}

	int nRet = xmlTextReaderRead( pReader );
	while ( nRet == 1 )
	{
		if ( processNode( pReader ) == false )
		{
			break;
		}
		nRet = xmlTextReaderRead( pReader );
	}
	xmlFreeTextReader( pReader );
	if ( nRet != 0 )
	{
		mstrErrorMsg =  "failed to parse : " + strFName;
		return;
	}

	if ( mrcDemRect.tl.getX() == -1.0 || mrcDemRect.tl.getY() == -1.0 ||
		 mrcDemRect.br.getX() == -1.0 || mrcDemRect.br.getY() == -1.0 ||
		 mpntLow.nU == UINT_MAX || mpntLow.nV == UINT_MAX ||
		 mpntHigh.nU == UINT_MAX || mpntHigh.nV == UINT_MAX ||
		 mpntStartPoint.nU == UINT_MAX || mpntStartPoint.nV == UINT_MAX
		 )
	{
		mstrErrorMsg = "file is not a valid gml file " + strFName;
		return;
	}

	createImage();

	mbValid = true;
}


GsiGmlReader::~GsiGmlReader()
{
	if ( mpData ) delete[] mpData;
}


void GsiGmlReader::getImage( WTM_BBOX* bbox, uint32_t *nWidth, uint32_t *nHeight, double **pData )
{
	*bbox = mrcDemRect;
	*nWidth = static_cast<uint32_t>( mpntHigh.nU - mpntLow.nU + 1 );
	*nHeight = static_cast<uint32_t>( mpntHigh.nV - mpntLow.nV + 1 );
	*pData = mpData;
}


void GsiGmlReader::createImage()
{
	uint64_t nWidth = static_cast<uint32_t>(mpntHigh.nU - mpntLow.nU + 1);
	uint64_t nHeight = (mpntHigh.nV - mpntLow.nV + 1);

	mpData = new double[nWidth*nHeight];

	uint64_t n = mpntStartPoint.nV*nWidth + mpntStartPoint.nU;
	for ( auto d : mvData )
	{
		if ( d < -9998 )
		{
			mpData[n++] = HUGE_VAL;
		}
		else
		{
			mpData[n++] = d;
		}
	}

	mvData.clear();
}


void GsiGmlReader::parseHeightList( const xmlChar *pszData )
{
	double d;
	int n = 0;
	const char *pszDelim = " \n";

	auto szToken = std::strtok( (char *)pszData, pszDelim );
	while ( szToken )
	{
		std::sscanf( szToken, "%*[^0-9-]%lf", &d );
		mvData.push_back( d );
		szToken = std::strtok( nullptr, pszDelim );
	}
	//std::cout << n << std::endl;
}


bool GsiGmlReader::processNode( xmlTextReaderPtr pReader )
{
	const xmlChar *name;
	auto type = xmlTextReaderNodeType( pReader );
	auto ns = xmlTextReaderConstNamespaceUri( pReader );
	auto prefix = xmlTextReaderConstPrefix( pReader );

	if ( type == XML_READER_TYPE_ELEMENT )
	{
		name = xmlTextReaderName( pReader );
		if ( xmlStrcmp( name, BAD_CAST"Dataset" ) == 0 )
		{
			//int nAttr = xmlTextReaderAttributeCount( pReader );
			//std::cout << nAttr;
			auto attr = xmlTextReaderGetAttribute( pReader, BAD_CAST"xmlns" );
			if ( xmlStrcmp( attr, BAD_CAST "http://fgd.gsi.go.jp/spec/2008/FGD_GMLSchema") != 0 )
			{
				mstrErrorMsg = "input file is not a gsi_dem file : " + mstrFName;
				return false;
			}
			else
			{
				mbValid = true;
				return true;
			}
		}

		if ( xmlStrcmp(name, BAD_CAST "coverage") == 0 )
		{
			mStateFlags |= GML_COVERAGE;
			return true;
		}

		if ( mStateFlags & GML_COVERAGE )
		{
			if ( xmlStrcmp( name, BAD_CAST"gml:boundedBy" ) == 0 )
			{
				mStateFlags |= GML_BOUND;
				return true;
			}
			if ( mStateFlags & GML_BOUND )
			{
				if ( xmlStrcmp( name, BAD_CAST"gml:Envelope") == 0 )
				{
					mStateFlags |= GML_ENVELOPE;
					return true;
				}

				if ( mStateFlags & GML_ENVELOPE )
				{
					if ( xmlStrcmp( name, BAD_CAST"gml:lowerCorner") == 0 )
					{
						mStateFlags |= GML_LOWER_CORNER;
						return true;
					}
					if ( xmlStrcmp( name, BAD_CAST"gml:upperCorner" ) == 0 )
					{
						mStateFlags |= GML_UPPER_CORNER;
						return true;
					}
				}
			}

			if ( xmlStrcmp( name, BAD_CAST"gml:gridDomain" ) == 0 )
			{
				mStateFlags |= GML_GRID_DOMAIN;
				return true;
			}
			if ( mStateFlags & GML_GRID_DOMAIN )
			{
				if ( xmlStrcmp( name, BAD_CAST"gml:Grid") == 0 )
				{
					auto dim = xmlTextReaderGetAttribute( pReader, BAD_CAST"dimension" );
					if ( xmlStrcmp( dim, BAD_CAST"2" ) != 0 )
					{
						mstrErrorMsg = "dimension is not 2 " + mstrFName;
						return false;
					}
					else
					{
						mStateFlags |= GML_GRID;
						return true;
					}
				}
				if ( mStateFlags & GML_GRID )
				{
					if ( xmlStrcmp( name, BAD_CAST"gml:low") == 0 )
					{
						mStateFlags |= GML_LOW;
						return true;
					}
					if ( xmlStrcmp( name, BAD_CAST"gml:high" ) == 0 )
					{
						mStateFlags |= GML_HIGH;
						return true;
					}
				}
			}

			if ( xmlStrcmp( name, BAD_CAST"gml:rangeSet") == 0 )
			{
				mStateFlags |= GML_RANGESET;
				return true;
			}
			if ( mStateFlags & GML_RANGESET )
			{
				if ( xmlStrcmp( name, BAD_CAST"gml:DataBlock" ) == 0 )
				{
					mStateFlags |= GML_DATABLOCK;
					return true;
				}
				if ( mStateFlags & GML_DATABLOCK )
				{
					if ( xmlStrcmp( name, BAD_CAST"gml:tupleList") == 0 )
					{
						mStateFlags |= GML_TUPLELIST;
					}
				}
			}
		}

		if ( xmlStrcmp( name, BAD_CAST"gml:coverageFunction" ) == 0 )
		{
			mStateFlags |= GML_COVERAGEFUNC;
			return true;
		}
		if ( mStateFlags & GML_COVERAGEFUNC )
		{
			if ( xmlStrcmp( name, BAD_CAST"gml:GridFunction") == 0 )
			{
				mStateFlags |= GML_GRIDFUNC;
				return true;
			}
			if ( mStateFlags & GML_GRIDFUNC )
			{
				if ( xmlStrcmp( name, BAD_CAST"gml:sequenceRule") == 0 )
				{
					auto attr = xmlTextReaderGetAttribute(pReader, BAD_CAST"order");
					if ( xmlStrcmp( attr, BAD_CAST"+x-y") != 0 )
					{
						mstrErrorMsg = "error : sequence rule is not +x-y :" + mstrFName;
						return false;
					}
					mStateFlags |= GML_SEQUENCERULE;
					return true;
				}
				if ( xmlStrcmp( name, BAD_CAST"gml:startPoint" ) == 0 )
				{
					mStateFlags |= GML_STARTPOINT;
					return true;
				}
			}
		}
	}
	else if ( type == XML_READER_TYPE_TEXT )
	{
		if ( mStateFlags & GML_COVERAGE )
		{
			if ( mStateFlags & GML_BOUND &&
				 mStateFlags & GML_ENVELOPE )
			{
				double dX, dY;
				if ( mStateFlags & GML_LOWER_CORNER )
				{
					sscanf( (const char *)xmlTextReaderConstValue(pReader), "%lf %lf", 
							&dY, &dX );
					mrcDemRect.tl.setX( dX );
					mrcDemRect.br.setY( dY );
					return true;
				}
				if ( mStateFlags & GML_UPPER_CORNER )
				{
					sscanf( (const char *)xmlTextReaderConstValue(pReader), "%lf %lf", 
							&dY, &dX );
					mrcDemRect.br.setX( dX );
					mrcDemRect.tl.setY( dY );
					return true;
				}
			}

			if ( mStateFlags & GML_GRID_DOMAIN &&
				 mStateFlags & GML_GRID )
			{
				if ( mStateFlags & GML_LOW )
				{
					sscanf( (const char*)xmlTextReaderConstValue( pReader ), "%lld %lld",
							&mpntLow.nU, &mpntLow.nV );
					return true;
				}
				else if ( mStateFlags & GML_HIGH )
				{
					sscanf( (const char *)xmlTextReaderConstValue(pReader), "%lld %lld",
							&mpntHigh.nU, &mpntHigh.nV );
					return true;
				}
			}

			if ( mStateFlags & GML_RANGESET &&
				 mStateFlags & GML_DATABLOCK &&
				 mStateFlags & GML_TUPLELIST )
			{
				parseHeightList( xmlTextReaderConstValue( pReader ) );
				return true;
			}

			if ( mStateFlags & GML_COVERAGEFUNC &&
				 mStateFlags & GML_GRIDFUNC )
			{
				if ( mStateFlags & GML_SEQUENCERULE )
				{
					auto content = xmlTextReaderConstValue( pReader );
					if ( xmlStrcmp( content, BAD_CAST"Linear" ) != 0 )
					{
						mstrErrorMsg = "error : sequence rule is not Linear : " + mstrFName;
						return false;
					}
					return true;
				}
				if ( mStateFlags & GML_STARTPOINT )
				{
					sscanf( (const char *)xmlTextReaderConstValue(pReader), "%lld %lld",
							&mpntStartPoint.nU, &mpntStartPoint.nV );
					return true;
				}
			}
		}
	}
	else if ( type == XML_READER_TYPE_END_ELEMENT )
	{
		name = xmlTextReaderName( pReader );
		if ( xmlStrcmp( prefix, BAD_CAST"coverage" ) == 0 )
		{
			mStateFlags &= ~(GML_COVERAGE);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:lowerCorner") == 0 )
		{
			mStateFlags &= ~(GML_LOWER_CORNER);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:upperCorner" ) == 0 )
		{
			mStateFlags &= ~(GML_UPPER_CORNER);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:Envelope" ) == 0 )
		{
			mStateFlags &= ~(GML_ENVELOPE);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:boundedBy" ) == 0 )
		{
			mStateFlags &= ~(GML_BOUND);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:low") == 0 )
		{
			mStateFlags &= ~(GML_LOW);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:high" ) == 0 )
		{
			mStateFlags &= ~(GML_HIGH);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:Grid") == 0 )
		{
			mStateFlags &= ~(GML_GRID);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:gridDomain" ) == 0 )
		{
			mStateFlags &= ~(GML_GRID_DOMAIN);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:rangeSet") == 0 )
		{
			mStateFlags &= ~(GML_RANGESET);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:DataBlock" ) == 0 )
		{
			mStateFlags &= ~(GML_DATABLOCK);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:tupleList") == 0 )
		{
			mStateFlags &= ~(GML_TUPLELIST);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:coverageFunction" ) == 0 )
		{
			mStateFlags &= ~(GML_COVERAGEFUNC);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:GridFunction") == 0 )
		{
			mStateFlags &= ~(GML_GRIDFUNC);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:sequenceRule") == 0 )
		{
			mStateFlags &= ~(GML_SEQUENCERULE);
			return true;
		}
		if ( xmlStrcmp( name, BAD_CAST"gml:startPoint" ) == 0 )
		{
			mStateFlags &= ~(GML_STARTPOINT);
			return true;
		}
	}

	return true;

	//name = xmlTextReaderConstName( pReader );
	//name ? std::cout << name << " : " : std::cout << "---";
	//value = xmlTextReaderConstValue( pReader );
	//value ? std::cout << value : std::cout << std::endl;
}

