
#include <plateau-mb-terrain-converter.h>
#include <iostream>


int main( int argc, char* argv[] )
{
	std::unique_ptr<PlateauMapboxTerrainConverter> converter = 
		std::make_unique<PlateauMapboxTerrainConverter>( 
			argv[1], argv[2], 10, 15, 
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
