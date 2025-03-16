#################################################################################
#                         plateau2gtif_python.py
# 
# Python example of MergeTilesets() function.
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


def pyPlateau2gtif(argv = sys.argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="input file path.")
    parser.add_argument("output", help="output file path.")
    parser.add_argument("resolution_lon", type=float, help="resolution of longitude.")
    parser.add_argument("resolution_lat", type=float, help="resolutino of latitude.")

    args = parser.parse_args()
    feedback = myFeedback()
    Terrain2GTif(args.input, args.output, args.resolution_lon, args.resolution_lat, feedback)

if __name__ == "__main__":
    sys.exit(pyPlateau2gtif(sys.argv))
