import matplotlib.pyplot as plt
import numpy as np

from Constants import TWELVE_TONE,POS_BIN,NEG_BIN,OCTAVE
from Functions import increaseHalfSteps,decreaseHalfSteps,incrementByOctave,decrementByOctave
from scipy import signal
 




class FFT_Graphing:

    def __init__(self,
                 chunkSize = 255,
                 binWidth = POS_BIN,
                 tuningA = 440,
                 frequencyRange = [0,8]) -> None:
        
        self.chunkSize = chunkSize
        self.binWidth = binWidth
        self.tuningA = tuningA
        #frequency range is based on note numbers, default [0,8] means C0 to B8
        self.frequencyRange = frequencyRange
        self.bins: list = [tuningA]

        pass

    def setChunkSize(self,chunkSize):
        self.chunkSize = chunkSize

    def getChunkSize(self):
        return self.chunkSize
    
    #generating the musical note bins (default A hz value is 440)
    def generateBinList(self):
        currentFrequency = self.tuningA

        #generating frequencies from tuningA to the top of the frequency range
        for octave in range(4,8):
            for pitch in range(0,OCTAVE):
                self.bins.append(increaseHalfSteps(pitch,currentFrequency))
            currentFrequency = incrementByOctave(currentFrequency)
        
        currentFrequency = self.tuningA

        for octave in range(4,0,-1):
            for pitch in range(0,OCTAVE):
                self.bins.insert(0,decreaseHalfSteps(pitch,currentFrequency))
            currentFrequency = decrementByOctave(currentFrequency)


            


f = FFT_Graphing()

f.generateBinList()
print(f.bins)


