

#include <plateau-mb-terrain-converter.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstring>


void usage()
{
	std::cerr << "convertGsiDemToMapboxRGB Usage: " << std::endl;
	std::cerr << "convertGsiDemToMapboxRGB [(options)] [input file path(CityGML)] [output directory]" << std::endl;
	std::cerr << "options :" << std::endl;
	std::cerr << "    --min_zoom [zoom no] : minimum zoom level (default = 6)" << std::endl;
	std::cerr << "    --max_zoom [zoom no] : maximum zoom level (default = 15)" << std::endl;
	std::cerr << "    --overwrite : overwrite existing tiles" << std::endl;
	std::cerr << "    --logfile [filename] : specify output log file" << std::endl;
	exit( 1 );
}


int main( int argc, char* argv[] )
{
	int nMinZoom = 6;
	int nMaxZoom = 15;
	bool bOverwrite = false;
	std::ofstream ofsLog;
	std::string strInputFName;
	std::string strOutputDir;


	if ( argc < 3 ) usage();

	for ( int i = 1; i < argc; i++ )
	{
		if ( std::strcmp( argv[i], "--min_zoom" ) == 0 )
		{
			nMinZoom = std::stoi( argv[++i] );
		}
		else if ( std::strcmp( argv[i], "--max_zoom" ) == 0 )
		{
			nMaxZoom = std::stoi( argv[++i] );
		}
		else if ( std::strcmp( argv[i], "--overwrite" ) == 0 )
		{
			bOverwrite = true;
		}
		else if ( std::strcmp( argv[i], "--logfile" ) == 0 )
		{
			ofsLog.open( argv[++i], std::ios_base::app );
			if ( !ofsLog )
			{
				std::cerr << "unable to open log file : [" << argv[i-1] << "]" << std::endl;
			}
		}
		else if ( std::strcmp( argv[i], "--help" ) == 0 )
		{
			usage();
		}
		else if ( strInputFName.empty() )
		{
			strInputFName = argv[i];
		}
		else if ( strOutputDir.empty() )
		{
			strOutputDir = argv[i];
		}
		else
		{
			std::cerr << "invalid argument or option [" << argv[i] << "]" << std::endl;
			usage();
		}
	}

	if ( strInputFName.empty() )
	{
		std::cerr << "input file is not specified." << std::endl;
		usage();
	}

	if ( strOutputDir.empty() )
	{
		std::cerr << "output directory is not specified." << std::endl;
		usage();
	}

	std::cout << "[" << strInputFName << "] : process started" << std::endl;
	if ( ofsLog.is_open() )
	{
		ofsLog << "[" << strInputFName << "]" << std::endl;
	}

	auto chronoStart = std::chrono::system_clock::now();

	try
	{
		pmtc::createGsiTileset(
			strInputFName, strOutputDir, nMinZoom, nMaxZoom, bOverwrite,
			[&]( MESSAGE_STATUS eStatus, const std::string& strMessage ){
			if ( eStatus == MESSAGE_ERROR )
			{
				std::cerr << "ERROR : " << strMessage << std::endl;
				if ( ofsLog )
				{
					ofsLog << "ERROR : " << strMessage << std::endl;
				}
			}
			else
			{
				std::cout << strMessage << std::endl;
			}
		},
			[]( int nProgress ){
			std::cout << nProgress << '\r' << std::flush;
		}
		);
	}
	catch ( std::range_error& e )
	{
		std::cerr << e.what();
	}

	auto chronoEnd = std::chrono::system_clock::now();
	auto chronoTime = std::chrono::duration_cast<std::chrono::seconds>( chronoEnd - chronoStart );
	std::cout << "process finished successfully in " << chronoTime.count() << "seconds." << std::endl;
	if ( ofsLog.is_open() )
	{
		ofsLog << "process finished successfully in " << chronoTime.count() << "seconds." << std::endl;
		ofsLog.close();
	}

	return 0;
}
