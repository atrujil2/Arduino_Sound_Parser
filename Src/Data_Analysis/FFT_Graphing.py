import matplotlib.pyplot as plt
import numpy as np

from Constants import TWELVE_TONE,POS_BIN,NEG_BIN
from scipy import signal
from scipy.fft import fft, ifft, power_spectrum
 




class FFT_Graphing:

    def __init__(self,
                 chunkSize = 255,
                 binWidth = 2**(1/12),
                 tuningA = 440) -> None:
        
        self.chunkSize = chunkSize
        self.binWidth = binWidth
        self.bins: dict = {}
        self.tuningA = tuningA

        pass

    def setChunkSize(self,chunkSize):
        self.chunkSize = chunkSize

    def getChunkSize(self):
        return self.chunkSize
    
    #generating the musical note bins (default A hz value is 440)
    def generateBinList(self):
        0#TODO




