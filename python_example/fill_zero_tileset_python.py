
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
	parser.add_argument("input", nargs='?', help="input directory")

	args = parser.parse_args()
	feedback = myFeedback()
	Fill_zero(args.input, feedback)

if __name__ == "__main__":
    sys.exit(pyConvertGsiDemToMapboxRGB(sys.argv))
