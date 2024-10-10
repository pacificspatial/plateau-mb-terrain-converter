
#include <plateau-mb-terrain-converter.h>
#include <iostream>
#include <filesystem>


void usage()
{
	std::cerr << "convertTerrainToMapboxRGB Usage: " << std::endl;
	std::cerr << "[input file path(CityGML)][output directory][minimum zoom level][maximum zoom level]" << std::endl;
	exit( 1 );
}


int main( int argc, char* argv[] )
{
	if ( argc < 5 ) usage();

	int nMinZoom = std::stoi( argv[3] );
	int nMaxZoom = std::stoi( argv[4] );

	std::unique_ptr<PlateauMapboxTerrainConverter> converter = 
		std::make_unique<PlateauMapboxTerrainConverter>( 
			argv[1], argv[2], nMinZoom, nMaxZoom, 
			[]( PlateauMapboxTerrainConverter::MESSAGE_STATUS eStatus, const std::string& strMessage ){
				if ( eStatus == PlateauMapboxTerrainConverter::MESSAGE_ERROR )
				{
					std::cerr << "ERROR : " << strMessage << std::endl;
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
		converter->createTileset();
	}

	return 0;
}
