
#include <png.h>
#include <tiffio.h>
#include <filesystem>


static bool ReadTIFF( const char* pszFName, int* pnWidth, int* pnHeight, float** ppaPixels )
{
	int nBitsPerSample, nSamplesPerPixel, ePhotometric, eSampleFormat, nRowsPerStrip;

	TIFF *pTif = TIFFOpen( pszFName, "r" );
	if ( !pTif )
	{
		fprintf( stderr, "Unable to open image [%s]\n", pszFName );
		return false;
	}

	TIFFGetField( pTif, TIFFTAG_IMAGEWIDTH, pnWidth );
	TIFFGetField( pTif, TIFFTAG_IMAGELENGTH, pnHeight );
	TIFFGetField( pTif, TIFFTAG_BITSPERSAMPLE, &nBitsPerSample );
	TIFFGetField( pTif, TIFFTAG_SAMPLESPERPIXEL, &nSamplesPerPixel );
	TIFFGetField( pTif, TIFFTAG_PHOTOMETRIC, &ePhotometric );
	TIFFGetField( pTif, TIFFTAG_SAMPLEFORMAT, &eSampleFormat );
	TIFFGetField( pTif, TIFFTAG_ROWSPERSTRIP, &nRowsPerStrip );

	if ( nSamplesPerPixel != 1 )
	{
		fprintf( stderr, "Input image format is not a Single-band image [%s].\n", pszFName );
		TIFFClose( pTif );
		return false;
	}
	if ( ePhotometric != PHOTOMETRIC_MINISBLACK )
	{
		fprintf( stderr, "Input image format is not a Single-band image [%s].\n", pszFName );
		TIFFClose( pTif );
		return false;
	}
	if ( eSampleFormat != SAMPLEFORMAT_IEEEFP )
	{
		fprintf( stderr, "Pixel format is not floating point number. [%s].\n", pszFName );
		TIFFClose( pTif );
		return false;
	}

	*ppaPixels = (float *)_TIFFmalloc( *pnWidth * *pnHeight * sizeof(float) );
	auto *p = *ppaPixels;
	for ( int i = 0; i < *pnHeight; i++ )
	{
		TIFFReadScanline( pTif, p, i );
		p += *pnWidth;
	}

	TIFFClose( pTif );
	return true;
}


int main( int argc, char* argv[] )
{
	return 0;
}
