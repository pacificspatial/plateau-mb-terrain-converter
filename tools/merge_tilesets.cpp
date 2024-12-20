
#include "plateau-mb-terrain-converter.h"


int main( int argc, char* argv[] )
{
	PlateauMapboxTerrainConverter::mergeTilesets(
		argv[1], argv[2], argv[3], false,
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

	return 0;
}