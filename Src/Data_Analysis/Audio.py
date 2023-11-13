#class for implementing audio input as a class
import matplotlib.pyplot as plt
from matplotlib.widgets import RadioButtons
import struct
import wave
from tkinter.filedialog import askopenfilename

class Audio():

    def __init__(self):
        #raw audio data
        self.rawAudio = None
        #number of channels
        self.channelCount = None
        #number of bytes per sample
        self.sampleWidth = None
        #frame/sample rate of audio file
        self.frameRate = None
        #number of frames in the audio file
        self.frameCount = None
        #audio after unpacking the struct
        self.audio = None
        #separate channels
        self.audioByChannel: dict = {}

    def unpackAudio(self):
        format:str

        if self.sampleWidth == 1:
            format = 'B'
        elif self.sampleWidth == 2:
            format = 'h'
        else:
            raise ValueError('Only supports 8-bit and 16-bit audio')

        samples = struct.unpack(format * self.channelCount * self.frameCount, self.rawAudio)

        if self.channelCount == 2:
            samples = [(samples[i] + samples[i + 1]) / 2 for i in range(0, len(samples), 2)]
        
        self.audio = samples

        if self.channelCount == 2:
            self.audioByChannel[1] = samples[::2]
            self.audioByChannel[2] = samples[1::2]
        elif self.channelCount == 1:
            self.audioByChannel[1] = samples


    def openWavFile(self):
        file = askopenfilename(filetypes = [('.wav file type','*.wav')])
        with wave.open(file, 'rb') as wav_file:
            # Get some properties of the WAV file
            self.channelCount = wav_file.getnchannels()
            self.sampleWidth = wav_file.getsampwidth()
            self.frameRate = wav_file.getframerate()
            self.frameCount = wav_file.getnframes()
            
            # Read audio frames
            self.rawAudio = wav_file.readframes(self.frameCount)

        self.unpackAudio()

    #plotting -------------------------------------------------------------------------------

    def onClose(self,event) -> None:
        plt.close()
        print('Plot Closed.')

    def plotWaveform(self) -> None:
        fig,ax = plt.subplots()
        ax.plot(self.audio)
        fig.canvas.mpl_connect('close_event',self.onClose)
        plt.show()

    def plotWaveformByChannel(self) -> None:
        if self.channelCount == 2:
            channels = ['Channel 1',
                        'Channel 2',
                        'Both Channels']
            fig, ax = plt.subplots()
            fig.subplots_adjust(left = 0.25)
            radioAx = fig.add_axes([0,0.5,0.2,0.1])
            radio = RadioButtons(radioAx,
                                labels = channels)
            
            channel1, = ax.plot(self.audioByChannel[1], color = 'blue', alpha = 0.6, label = 'Channel 1')
            channel2, = ax.plot(self.audioByChannel[2], color = 'red', alpha = 0.6, label = 'Channel 2')
            channel1.set_visible(False)
            channel2.set_visible(False)
            
            channelLines = [channel1,channel2]

            def radioChannelClick(value,fig = fig, ax = ax, self = self, channelLines = channelLines) -> None:
                if value == 'Channel 1':
                    channelLines[0].set_visible(True)
                    channelLines[1].set_visible(False)
                elif value == 'Channel 2':
                    channelLines[0].set_visible(False)
                    channelLines[1].set_visible(True)
                elif value == 'Both Channels':
                    channelLines[0].set_visible(True)
                    channelLines[1].set_visible(True)
                fig.canvas.draw()
            

            radio.on_clicked(radioChannelClick)

            plt.show()

        #if it's only one channel then it would be the same as plotting the waveform
        else:
            self.plotWaveform()
        

def waveformTest():
    audio = Audio()
    audio.openWavFile()
    print(f'{audio.channelCount}\n{audio.frameRate}\n{audio.frameCount}\n{audio.sampleWidth}')

    print(len(audio.audio),len(audio.rawAudio))

    audio.plotWaveformByChannel()

