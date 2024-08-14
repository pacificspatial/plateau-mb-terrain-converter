
#include <plateau-mb-terrain-converter.h>


int main( int argc, char* argv[] )
{
	std::unique_ptr<PlateauMapboxTerrainConverter> converter = std::make_unique<PlateauMapboxTerrainConverter>( argv[1], argv[2], 10, 15 );
	converter->createTileset();

	return 0;
}
