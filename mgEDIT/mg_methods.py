from mg_ent_classes import *
from os import path
#from mg_popup import *
import tkinter as tk

def makeListOfEnemiesFromFile(path) :
    enemies = []
    toPull = 0;

    print("Pulling Enemies from list")
    
    with open(path, "r+") as file :
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
                    positionVelocity = CUS_Polar(float(currentLine[7]), float(currentLine[8]))
                    toPull = EnemyEntity(int( currentLine[1]), currentLine[2], positionStart, positionVelocity, float(currentLine[3]), int(currentLine[4]) )
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

def saveEnemiesToFile(master, enemyList) :
    path = master._pathToMaster + "//campaigns//" + master._campaign + "//" + str(master._level) + "//enemy_table.txt"

    with open(path, "w+") as file :
        print(len(enemyList))
        for i in enemyList :
            file.write("ENEMY %d %s %f %d %f %f %f %f\n"%(i._spawningCycle, i._animationName,
                                                   i._hitbox, i._hitpoints,
                                                   i._initialPosition._x, i._initialPosition._y,
                                                   i._initialVelocity._magnitude, i._initialVelocity._angle)
                  )
            if i._bulletMaster is not "" :
                file.write("BULLETMASTER %d %s\n"%(i._bulletMasterTime, i._bulletMaster))
                
            for j in i._movementCommands :
                command = i._movementCommands[j]
                file.write("MOVE %d %d %d %d %f %d %d %f\n"%(j,
                                                      int(command._ignoreAngle), int(command._forceAngle), int(command._relativeToPlayer),
                                                      float(command._angle),
                                                      int(command._ignoreSpeed), int(command._forceSpeed), float(command._speed))
                      )
        file.write("DEATH %d\n\n"%(i._deathCycle))

def enemySort(enemy) :
    return enemy._spawningCycle
