
#include <plateau-mb-terrain-converter.h>
#include <iostream>


int main( int argc, char* argv[] )
{
	std::unique_ptr<PlateauMapboxTerrainConverter> converter = 
		std::make_unique<PlateauMapboxTerrainConverter>( 
			argv[1], argv[2], 10, 15,
			[](std::string strErr ){
				std::cerr << strErr << std::endl;
			},
			[](int nProgress){
				std::cout << nProgress << "\r" << std::flush;
			}
	);
	if ( converter->isValid() )
	{
		converter->createTileset();
	}

	return 0;
}
