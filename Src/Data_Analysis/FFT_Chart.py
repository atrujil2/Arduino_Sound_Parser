import numpy as np
import scipy as sci
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter.filedialog import askopenfilename
from scipy.fft import fft, ifft

from scipy.io import wavfile

print("Select an audio file for analysis")



def getFile():
    filename = askopenfilename() # show an "Open" dialog box and return the path to the selected file
    print(filename)
    data = wavfile.read(filename)
    return data

def plotFFTData(data,domain,c):
    fig,ax = plt.subplots(2)
    ax[0].plot(data.T[0])
    ax[1].plot(domain[5:25000]*2,c[5:25000])
    #ax[1].plot(dataFrame['Frequency'],dataFrame['Power'])
    ax[0].set_title("Raw Audio (Channel 1)")
    ax[1].set_title("FFT of WAV audio file input")
    ax[1].set_xlabel("Sound Frequency (Hz)")
    ax[1].set_ylabel("Power at Frequency")
    fig.tight_layout()
    plt.show()

def transformData(data):
    a = data.T[0] # this is a two channel soundtrack, I get the first track
    b=[(ele/2**16.)*2-1 for ele in a] # this is 8-bit track, b is now normalized on [-1,1)
    c = list(abs(fft(b))) # calculate fourier transform (complex numbers list)
    d = len(c)/2  # you only need half of the fft list (real signal symmetry)
    c = c[0:int(len(c)/2)]
    domain = np.arange(0,len(c)/2/10,0.05)
    return domain,c

def testForSimilarVectorLength(vector1,vector2):
    if len(vector1) == len(vector2):
        print("lists are same length")
    else:
        print("different list lengths")
        print(len(vector1))
        print(len(vector2))

def outputDataFrame(domain,range):
    dataDict = {'Frequency': domain,
                'Power': range}
    dataFrame = pd.DataFrame(dataDict)
    return dataFrame

def getLogPower(power):
    logPower = np.log10(power)
    return logPower

def FFT_Chart():
    fs, data = getFile() # load the data
    domain,range = transformData(data)
    range = getLogPower(range)
    testForSimilarVectorLength(range,domain)
    dataFrame = outputDataFrame(domain,range)
    plotFFTData(data,domain,range)