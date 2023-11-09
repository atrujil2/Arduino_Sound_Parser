from Audio import Audio
from scipy import signal, fft
import matplotlib.pyplot as plt

class FFT():

    def __init__(self,
                 audio:Audio,               #instance of the Audio class
                 chunkSize = 255,           #the amount of datapoints in a chunk for simulated continuous input
                 
                 ) -> None:
        self.audio = audio
        self.chunkSize = chunkSize
        self.frequencies = None
        self.spectrum = None

    def getFFT(self):
        self.spectrum = fft.fft(self.audio.audio)
        self.frequencies = fft.fftfreq(len(self.audio.audio), 1 / self.audio.frameRate)
        
    def sliceIntoChunks(self):
        pass

aud = Audio()
aud.openWavFile()
transform = FFT(aud)
transform.getFFT()
plt.plot(transform.frequencies,abs(transform.spectrum))

plt.show()