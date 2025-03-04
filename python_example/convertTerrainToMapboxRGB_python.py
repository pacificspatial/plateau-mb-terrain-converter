#################################################################################
#                        convertTerrainToMapboxRGB_python.py
# 
# Python example of CreatePlateauTileset() function.
#
# (c) 2025 MLIT Japan.
# (c) 2025 Pacific Spatial Solutions Inc.
#
# Author : Yamate, N
# 
#################################################################################

from plateauterrainrgb.pyplateauterrainrgb import *
import sys, argparse


class myFeedback(PMTCFeedback):

    def __init__(self):
        PMTCFeedback.__init__(self)

    def messageFeedback(self, eStatus, strMessage):
        if eStatus == MESSAGE_ERROR:
            print("ERROR : " + strMessage, file=sys.stderr)
        else:
            print(strMessage)

    def progressFeedback(self, nProgress):
        sys.stdout.write(str(nProgress) + '\r')
        sys.stdout.flush()


def pyConvertTerrainToMapboxRGB(argv = sys.argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="input file path.")
    parser.add_argument("outdir", help="output directory")
    parser.add_argument("--min_zoom", nargs='?', default=6, help="minimum zoom level (default 6)")
    parser.add_argument("--max_zoom", nargs='?', default=15, help="maximum zoom level (default = 15)")
    parser.add_argument("--overwrite", action="store_false", help="overwrite existing tiles")

    args = parser.parse_args()
    feedback = myFeedback()
    CreatePlateauTileset(args.input, args.outdir, args.min_zoom, args.max_zoom, args.overwrite, feedback)

if __name__ == "__main__":
    sys.exit(pyConvertTerrainToMapboxRGB(sys.argv))
