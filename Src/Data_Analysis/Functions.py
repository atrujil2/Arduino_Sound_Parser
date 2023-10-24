#this file is for small, useful functions that don't need to be in the other classes.
from Constants import POS_BIN,NEG_BIN

#decreases a note by a number of half steps
def decreaseHalfSteps(halfSteps,frequency):
    return (2**(halfSteps/12)) * frequency

#increases a note by a number of half steps
def increaseHalfSteps(halfSteps,frequency):
    return (2**(-halfSteps/12)) * frequency



def incrementPitch(frequency):
    return frequency + POS_BIN

def decrementPitch(frequency):
    return frequency - NEG_BIN


#the octave above or below a given note is double or half of the given frequency
def decrementByOctave(frequency):
    return frequency/2

def incrementByOctave(frequency):
    return frequency*2