
#include <plateau-mb-terrain-converter.h>
#include <iostream>
#include <filesystem>
#include <fstream>


void usage()
{
	std::cerr << "convertTerrainToMapboxRGB Usage: " << std::endl;
	std::cerr << "[input file path(CityGML)] [output directory] [minimum zoom level] [maximum zoom level] [(option) output log filename]" << std::endl;
	exit( 1 );
}


int main( int argc, char* argv[] )
{
	if ( argc != 5 && argc != 6 ) usage();

	std::ofstream ofsLog;

	int nMinZoom = std::stoi( argv[3] );
	int nMaxZoom = std::stoi( argv[4] );

	if ( argc == 6 )
	{
		ofsLog.open( argv[5], std::ios_base::app );
		if ( !ofsLog )
		{
			std::cerr << "unable to open log file : [" << argv[5] << "]" << std::endl;
			std::exit( 1 );
		}
	}

	std::cout << "[" << argv[1] << "] : process started" << std::endl;
	ofsLog << "[" << argv[1] << "]" << std::endl;
	auto chronoStart = std::chrono::system_clock::now();

	std::unique_ptr<PlateauMapboxTerrainConverter> converter = 
		std::make_unique<PlateauMapboxTerrainConverter>( 
			argv[1], argv[2], nMinZoom, nMaxZoom, 
			[&]( PlateauMapboxTerrainConverter::MESSAGE_STATUS eStatus, const std::string& strMessage ){
				if ( eStatus == PlateauMapboxTerrainConverter::MESSAGE_ERROR )
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
	if ( converter->isValid() )
	{
		try
		{
			converter->createTileset();
		}
		catch ( std::domain_error& e )
		{
			std::cerr << e.what();
		}
	}

	auto chronoEnd = std::chrono::system_clock::now();
	auto chronoTime = std::chrono::duration_cast<std::chrono::seconds>( chronoEnd - chronoStart );
	std::cout << "process finished successfully in " << chronoTime.count() << "seconds." << std::endl;
	if ( ofsLog )
	{
		ofsLog << "process finished successfully in " << chronoTime.count() << "seconds." << std::endl;
	}

	return 0;
}
