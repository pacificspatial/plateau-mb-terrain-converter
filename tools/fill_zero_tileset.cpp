
#include "plateau-mb-terrain-converter.h"


void usage()
{
	std::cerr << "fill_zero_tileset Usage: " << std::endl;
	std::cerr << "fill_zero_tileset [input directory] [(option)output directory]" << std::endl;
	std::exit( 1 );
}


int main( int argc, char* argv[] )
{
	std::filesystem::path pathSrc, pathOutDir;

	if ( argc < 2 ) usage();

	try
	{
		pathSrc.assign( argv[1] );
		if ( argc == 3 )
		{
			pathOutDir.assign( argv[2] );
			std::filesystem::copy( pathSrc, pathOutDir, std::filesystem::copy_options::recursive );
		}
		else
		{
			pathOutDir.assign( argv[1] );
		}

		PlateauMapboxTerrainConverter::fill_zero( pathOutDir.u8string(),
			[&]( PlateauMapboxTerrainConverter::MESSAGE_STATUS eStatus, const std::string& strMessage ) {
				if ( eStatus == PlateauMapboxTerrainConverter::MESSAGE_ERROR )
				{
					std::cerr << "ERROR : " << strMessage << std::endl;
				}
				else
				{
					std::cout << strMessage << std::endl;
				}
			},
			[]( int nProgress ) {
				std::cout << nProgress << '\r' << std::flush;
			}
		);
	}
	catch ( std::filesystem::filesystem_error &e )
	{
		std::cerr << e.what();
	}

	return 0;
}
