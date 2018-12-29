from enum import Enum

from PIL import Image, ImageTk
from tkinter import messagebox
import tkinter as tk

class ImageLevel(Enum):
    GLOBAL = 1
    CAMPAIGN = 2
    LOCAL = 3

class Animation(object) :
    def __init__(self, path, width, frameSkip, master) :
        self._scale = 1.0
        
        spritesheet = Image.open(path)
        w, h = spritesheet.size
        self._frames = []
        self._drawFrames = []
        width = int(width)
        noOfFrames = int(w/width)
        frameWidth = int(w/noOfFrames)
        self._radius = min(width, h)/2
        
        for i in range(0, noOfFrames) :
            cpy = spritesheet.copy()
            newcpy = cpy.crop((i*frameWidth, 0, i*frameWidth+frameWidth, h))
            newcpy.load()
            photo = ImageTk.PhotoImage(newcpy)
            self._frames.append(photo)
            drawcpy = cpy.crop((i*frameWidth, 0, i*frameWidth+frameWidth, h))
            drawcpy.resize(( int(frameWidth*master._gameScale),int(h*master._gameScale) ), Image.LANCZOS)
            drawcpy.load()
            drawPhoto = ImageTk.PhotoImage(drawcpy)
            self._drawFrames.append(drawPhoto)

        self._frameSkip = int(frameSkip)
        self._w=frameWidth
        self._h=h
        self._noOfFrames=noOfFrames

    def setScale(self, scale) :
        self._scale = scale

class AnimationSet(object) :
    def __init__(self, name, imageLevel) :
        self._name = name
        self._imageLevel = imageLevel
        self._animations = dict()

    def addAnimation(self, animationType, animation) :
        self._animations.update( {animationType : animation} )

    def getAnimation(self, animationType) :
        if animationType in self._animations :
            return self._animations[animationType]
        else :
            print("Animation type " + animationType + " does not exist in " + name)
            return None

    def __str__(self):
        output = self._name + '\n'
        for i in self._animations :
            output += i
            output += ": "
            output += str(self._animations[i]._noOfFrames)
            output += " frames of size %d by %d with %d refresh rate"%(self._animations[i]._w, self._animations[i]._h, self._animations[i]._frameSkip )
            output += '\n'
            
        return output

def loadAnimations(folder, listName,  imageLevel, master) :
    animationSets = dict()
    index = ""
    
    with open(folder + listName, "r+") as file:
        lines = file.readlines()
        lines = [line.strip() for line in lines]

        lineNo = 0;
        for line in lines :
            lineNo+=1
            currentLine = line.split(' ')
            print(currentLine)
            if currentLine[0] == '' :
                print("---")
            elif currentLine[0][0] == '/' and currentLine[0][1] == '/' :
                print("---")
            elif len(currentLine) == 1 :
                index =  currentLine[0]
                currentAnimationSet = AnimationSet(currentLine[0], imageLevel)
                animationSets.update({ index : currentAnimationSet})
            elif len(currentLine) == 4 :
                if index != "" :
                    animationSets[index].addAnimation(currentLine[1], Animation(folder + currentLine[0], currentLine[2], currentLine[3], master))
            elif len(currentLine) == 5 :
                if index != "" :
                    animationSets[index].addAnimation(currentLine[1], Animation(folder + currentLine[0], currentLine[2], currentLine[3], master))
                    animationSets[index].getAnimation(currentLine[1]).setScale(float(currentLine[4]))
            else :
                print("Error at line " + str(lineNo) + " " + str(len(currentLine)))

    return animationSets
