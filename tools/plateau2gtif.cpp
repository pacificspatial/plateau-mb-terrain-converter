/***************************************************************************
                         plateau2gtif.cpp  -  description
                                 -------------------

        merge_tilesets application entry point.

        begin                : Mar. 06, 2025
        Copyright            : (c) 2025 MLIT Japan.
                             : (c) 2025 Pacific Spatial Solutions Inc.
        author               : Yamate, N
 ***************************************************************************/

#include "plateau-mb-terrain-converter.h"


void usage()
{
    std::cerr << "plateau2gtif Usage: " << std::endl;
    std::cerr << "plateau2gtif [input file] [output file] [longitude resolution] [latitude resolution]" << std::endl;
    std::exit(1);
}


int main( int argc, char *argv[] )
{
    if ( argc < 5 ) usage();

    std::string strInputCityGML = argv[1];
    std::string strOutputGTiff = argv[2];

    char *pszStop;
    double dResolutionLon = std::strtod( argv[3], &pszStop );
    double dResolutionLat = std::strtod( argv[4], &pszStop );

    if ( dResolutionLon <= 0.0 || dResolutionLat <= 0.0 )
    {
        std::cerr << std::fixed << std::setprecision(10)
            << "Invalid resolution : {"
            << dResolutionLon << " , " << dResolutionLat << "}" << std::endl;
        std::exit( 1 );
    }

    pmtc::terrain2gtif( argv[1], argv[2], dResolutionLon, dResolutionLat,
        [&](MESSAGE_STATUS eStatus, const std::string &strMessage)
        {
            if (eStatus == MESSAGE_ERROR)
            {
                std::cerr << "ERROR : " << strMessage << std::endl;
            }
            else
            {
                std::cout << strMessage << std::endl;
            }
        },
        [](int nProgress)
        {
            std::cout << nProgress << '\r' << std::flush;
        }
    );

    return 0;
}
