from mg_ent_classes import *
from os import path
#from mg_popup import *
import tkinter as tk

def makeListOfEnemiesFromFile(path) :
    enemies = []
    toPull = 0;

    print("Pulling Enemies from list")
    
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
            elif currentLine[0] == 'MOVE' :
                print(currentLine)
                if not toPull == 0 :
                    toPull.addMovementCommand(int(currentLine[1]), int(currentLine[2]), int(currentLine[3]), int(currentLine[4]),float(currentLine[5]), int(currentLine[6]), int(currentLine[7]), float(currentLine[8]))
                else :
                    print("Movement Command added when there is no enemy selected")
            elif currentLine[0] == 'ENEMY' :
                print(currentLine)
                if toPull == 0 :
                    print("Enemy caught")
                    positionStart = CUS_Point(float(currentLine[5]), float(currentLine[6]))
                    positionVelocity = CUS_Point(float(currentLine[7]), float(currentLine[8]))
                    toPull = EnemyEntity(int( currentLine[1]), currentLine[2], positionStart, positionVelocity, float(currentLine[3]), float(currentLine[4]) )
                else :
                    print("Enemy added when there is already an enemy not pushed")
            elif currentLine[0] == 'BULLETMASTER' :
                print(currentLine)
                toPull.setBulletMaster( int(currentLine[1]), currentLine[2])
            elif currentLine[0] == 'DEATH' :
                print(currentLine)
                toPull.setDeathCycle(int(currentLine[1]))
                enemies.append(toPull)
                toPull = 0
            else :
                print("Error at line " + str(lineNo) + " " + str(len(currentLine)))
        print(lineNo)
        return enemies

def enemySort(enemy) :
    return enemy._spawningCycle
