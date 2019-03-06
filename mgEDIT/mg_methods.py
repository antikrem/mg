from mg_ent_classes import *
from mg_bullet import *
from mg_movement import *
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
            file.write("DEATH %d\n"%(i._deathCycle))
            file.write("\n")

def enemySort(enemy) :
    return enemy._spawningCycle

def pullBulletMasterTemplatesFromFile(master) :
    path = master._pathToMaster + "//campaigns//" + master._campaign + "//" + str(master._level) + "//bullet_table.txt"

    bmtToPull = None
    bstToPull = None
    btToPull = None
    mode = "nobt"

    masterDict = dict()
    
    with open(path, "r") as file :
        lines = file.readlines()
        lines = [line.strip() for line in lines]

        lineNo = 0
        for line in lines :
            lineNo+=1
            currentLine = line.split(' ')
            print(currentLine)
            if currentLine[0] == "BULLETMASTER" :
                if bmtToPull is not None :
                    masterDict[bmtToPull._name] = bmtToPull
                    bstToPull = None
                    btToPull = None
                mode = "bmt"
                bmtToPull = BulletMasterTemplate(currentLine[1])
                
            elif currentLine[0] == "SPAWNER" :
                if (bmtToPull is not None) :
                    position = CUS_Point(float(currentLine[1]), float(currentLine[2]))
                    velocity = CUS_Polar(float(currentLine[3]), float(currentLine[4]))
                    bstToPull = BulletSpawnerTemplate(position, velocity)
                    if btToPull is not None :
                        bstToPull.addBulletTemplate(btToPull)
                    bmtToPull.addBulletSpawnerTemplates(bstToPull)
                    mode = "bst"
                    
            elif currentLine[0] == "VOLLEYTIMER" :
                if mode == "bst" :
                    sprayTimer = []
                    for i in range(1, len(currentLine)) :
                        sprayTimer.append(int(currentLine[i]))
                    bstToPull.addSprayTimer(sprayTimer)
                    
            elif currentLine[0] == "BETWEENVOLLYPAUSE" :
                if mode == "bst" :
                    bstToPull.setInBetweenTimer(int(currentLine[1]))

            elif currentLine[0] == "ROUNDS" :
                if mode == "bst" :
                    bstToPull.setRounds(int(currentLine[1]))
                
            elif currentLine[0] == "EXIT" :
                if mode == "bst" :
                    sprayTimer = []
                    for i in range(1, len(currentLine)) :
                        bstToPull.addExitLocation(float(currentLine[i]))

            elif currentLine[0] == "SPRITEMASK" :
                if mode == "bst" :
                    bstToPull.addMask(currentLine[1], int(currentLine[2]))
                
            elif currentLine[0] == "BULLET" :
                mode = "bt"
                velocity = CUS_Polar(float(currentLine[2]), float(currentLine[3]))
                btToPull = BulletTemplate(currentLine[1], velocity, float(currentLine[4]))

            elif currentLine[0] == "MOVE" :
                currentUpdate = MovementCommand(
                    int(currentLine[2]),
                    int(currentLine[3]),
                    int(currentLine[4]),
                    float(currentLine[5]),
                    int(currentLine[6]),
                    int(currentLine[7]),
                    float(currentLine[8])
                    )

                if mode == "bst" :
                    bstToPull.addMovementCommand(int(currentLine[1]), currentUpdate)
                elif mode == "bt" :
                    btToPull.addMovementCommand(int(currentLine[1]), currentUpdate)
            else :
                print("error")
                print(currentLine)

        masterDict[bmtToPull._name] = bmtToPull

    return masterDict


def saveBulletMastersToFile(master, bulletMasters) :
    path = master._pathToMaster + "//campaigns//" + master._campaign + "//" + str(master._level) + "//bullet_table.txt"

    with open(path, "w+") as file :
        print(len(bulletMasters))
        for i in bulletMasters :
            bulletMaster = bulletMasters[i]
            file.write("BULLETMASTER %s\n"%(bulletMaster._name))
            for spawner in bulletMaster._bulletSpawnerTemplates :
                #bullet template
                bullet = spawner._bulletTemplate
                if bullet is not None :
                    file.write("BULLET %s %f %f %f\n"%(bullet._animationName, bullet._initialVelocity._magnitude, bullet._initialVelocity._angle, bullet._hitbox))
                    for j in bullet._movementList :
                        command = bullet._movementList[j]
                        file.write("MOVE %d %d %d %d %f %d %d %f\n"%(j,
                                                                     int(command._ignoreAngle), int(command._forceAngle), int(command._relativeToPlayer), float(command._angle),
                                                                     int(command._ignoreSpeed), int(command._forceSpeed), float(command._speed))
                                   )
                #bullet spawner
                file.write("SPAWNER %f %f %f %f\n"%(
                    spawner._initialPosition._x, spawner._initialPosition._y,
                    spawner._initialVelocity._magnitude, spawner._initialVelocity._angle
                    )
                )
                if len(spawner._sprayTimer) is not 0 :
                    file.write("VOLLEYTIMER")
                    for k in spawner._sprayTimer :
                        file.write(" %d"%k)
                    file.write('\n')
                if spawner._inBetweenTimer is not 0 :
                    file.write("BETWEENVOLLEYPAUSE %f\n"%spawner._inBetweenTime)
                if spawner._rounds is not -1 :
                    file.write("ROUNDS %d\n"%spawner._rounds)
                if spawner._maskName is not "" :
                    file.write("SPRITEMASK %s %d\n"%(spawner._maskName, spawner._maskLayer))
                if len(spawner._sprayTimer) is not 0 :
                    file.write("EXIT")
                    for k in spawner._exitLocations :
                        file.write(" %f"%k)
                    file.write('\n')
                for j in spawner._movementList :
                    command = spawner._movementList[j]
                    file.write("MOVE %d %d %d %d %f %d %d %f\n"%(j,
                                                                 int(command._ignoreAngle), int(command._forceAngle), int(command._relativeToPlayer),
                                                                 float(command._angle),
                                                                 int(command._ignoreSpeed), int(command._forceSpeed), float(command._speed))
                               )
                file.write("\n")
