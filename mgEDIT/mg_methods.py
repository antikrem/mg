from mg_ent_classes import *
from os import path
from mg_ref import *
from mg_popup import *
import tkinter as tk

def makeListOfEnemiesFromFile(path) :
    enemies = []
    toPull = 0;
    with open(path, "r+") as file:
        lines = file.readlines()
        lines = [line.strip() for line in lines]

        lineNo = 0;
        for line in lines :
            lineNo+=1
            currentLine = line.split(' ')

            if currentLine[0] == '' :
                print("---")
            elif currentLine[0][0] == '/' and currentLine[0][1] == '/' :
                print("---")
            elif len(currentLine) == 7 :
                print(currentLine)
                if not toPull == 0 :
                    toPull.addMovementCommand(int(currentLine[0]), int(currentLine[1]), int(currentLine[2]), float(currentLine[3]), int(currentLine[4]), int(currentLine[5]), float(currentLine[6]))
                else :
                    print("Movement Command added when there is no enemy selected")
            elif len(currentLine) == 8 :
                print(currentLine)
                if toPull == 0 :
                    print("Caught")
                    positionStart = CUS_Point(float(currentLine[4]), float(currentLine[5]))
                    positionVelocity = CUS_Point(float(currentLine[6]), float(currentLine[7]))
                    toPull = EnemyEntity(int(currentLine[0]), currentLine[1], positionStart, positionVelocity)
                else :
                    print("Enemy added when there is already an enemy not pushed")
            elif len(currentLine) == 1 :
                print(currentLine)
                toPull.setDeathCycle(int(currentLine[0]))
                enemies.append(toPull)
                toPull = 0
            else :
                print("Error at line " + str(lineNo) + " " + str(len(currentLine)))
        print(lineNo)
        return enemies

def populateLevelCanvas(currentCycle, animations, levelView) :
    
