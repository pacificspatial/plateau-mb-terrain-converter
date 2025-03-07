#################################################################################
#                         merge_tilesets_python.py
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


def pyConvertGsiDemToMapboxRGB(argv = sys.argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("-o", "--outdir", nargs='?', help="[Required] output directory")
    parser.add_argument("--overwrite", action="store_false", help="overwrite existing tiles")
    parser.add_argument("inputs", nargs='*', help="input directory[s]")

    args = parser.parse_args()
    feedback = myFeedback()
    MergeTilesets(args.inputs, args.outdir, args.overwrite, feedback)

if __name__ == "__main__":
    sys.exit(pyConvertGsiDemToMapboxRGB(sys.argv))
