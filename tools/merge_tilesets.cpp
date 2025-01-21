
#include "plateau-mb-terrain-converter.h"
#include <cstring>
#include <vector>


void usage()
{
	std::cerr << "merge_tilesets Usage: " << std::endl;
	std::cerr << "merge_tilesets [(options)] [input directory [input directory] ...]" << std::endl;
	std::cerr << "options :" << std::endl;
	std::cerr << "    --outdir [directory] : output directory" << std::endl;
	std::cerr << "    --overwrite : overwrite existing tiles" << std::endl;
	std::exit( 1 );
}


int main( int argc, char* argv[] )
{
	bool bOverwrite = false;
	std::vector<std::string> vstrInputDirs;
	std::string strOutDir;

	if ( argc < 3 ) usage();

	for ( int i = 1; i < argc; i++ )
	{
		if ( std::strcmp( argv[i], "--overwrite" ) == 0 )
		{
			bOverwrite = true;
		}
		else if ( std::strcmp( argv[i], "--outdir" ) == 0 )
		{
			strOutDir = argv[++i];
		}
		else
		{
			vstrInputDirs.push_back( argv[i] );
		}
	}

	if ( strOutDir.empty() )
	{
		std::cerr << "output directory is not specified." << std::endl;
		usage();
	}

	try
	{
		pmtc::mergeTilesets(
			vstrInputDirs, strOutDir, bOverwrite,
			[&]( MESSAGE_STATUS eStatus, const std::string& strMessage ) {
				if ( eStatus == MESSAGE_ERROR )
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