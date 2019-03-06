from mg_cus_struct import *
import tkinter as tk
import sys
import copy

MOVEMENT_DEBUG = False

#stores a command that on starting frame will be applied
class MovementCommand(object) :
    def __init__(self, ignoreAngle, forceAngle, relativeToPlayer, angle, ignoreSpeed, forceSpeed, speed) :
        #speed variables
        self._ignoreAngle = ignoreAngle
        self._forceAngle = forceAngle
        self._relativeToPlayer = relativeToPlayer
        self._angle = angle
        #speed variables
        self._ignoreSpeed = ignoreSpeed
        self._forceSpeed = forceSpeed
        self._speed = speed

class MovementCommander(object) :
    def __init__(self, initialPosition, initialVelocity, spawningCycle) :
        self._spawningCycle = spawningCycle
        self._initialPosition = initialPosition
        self._initialVelocity = initialVelocity
        self._positionList = []
        self._movementCommands = dict()
        self._currentMovementCommand = MovementCommand(True, False, False, 0, True, False, 0)

        self._deathCycle = sys.maxsize
        self._deathInitialised = False

    def addStartingParameters(self, initialPosition, initialVelocity) :
        self._initialPosition = initialPosition
        self._initialVelocity = initialVelocity

    def addMovementCommand(self, cycle, ignoreAngle, forceAngle, relativeToPlayer, angle, ignoreSpeed, forceSpeed, speed) :
        self._movementCommands[cycle] = MovementCommand(ignoreAngle, forceAngle, relativeToPlayer, angle, ignoreSpeed, forceSpeed, speed)

    def addMovementCommandDirect(self, cycle, command) :
        self._movementCommands[cycle] = command
    
    #last cycle is the maximum possible cycle, usually    
    def calculatePositions(self, master, playerPosition, deleteBox, masterPositions) :
        self._positionList.clear()
        position  =  copy.deepcopy(self._initialPosition)
        velocity  =  copy.deepcopy(self._initialVelocity)
        self._currentMovementCommand = MovementCommand(True, False, False, 0, True, False, 0)

        masterLen = sys.maxsize
        if masterPositions is not None :
            masterLen = len(masterPositions)
            self._deathCycle = len(masterPositions) + self._spawningCycle            

        lastCycle = min(master._maxCycles *300 + 10 - self._spawningCycle, self._deathCycle, masterLen)
        
        for i in range(0,lastCycle) :
            if i in self._movementCommands :
                if not self._movementCommands[i]._ignoreAngle :
                    self._currentMovementCommand._ignoreAngle = False
                    self._currentMovementCommand._forceAngle = self._movementCommands[i]._forceAngle
                    self._currentMovementCommand._relativeToPlayer = self._movementCommands[i]._relativeToPlayer
                    self._currentMovementCommand._angle = self._movementCommands[i]._angle
                if not self._movementCommands[i]._ignoreSpeed :
                    self._currentMovementCommand._ignoreSpeed = False
                    self._currentMovementCommand._forceSpeed = self._movementCommands[i]._forceSpeed
                    self._currentMovementCommand._speed = self._movementCommands[i]._speed

            if not self._currentMovementCommand._ignoreSpeed :
                if self._currentMovementCommand._forceSpeed :
                    velocity._magnitude = self._currentMovementCommand._speed
                else :
                    velocity._magnitude += self._currentMovementCommand._speed
            if not self._currentMovementCommand._ignoreAngle :
                if not self._currentMovementCommand._relativeToPlayer :
                    if self._currentMovementCommand._forceAngle :
                       velocity._angle = self._currentMovementCommand._angle
                    else :
                        velocity._angle += self._currentMovementCommand._angle
                else :
                    if (self._currentMovementCommand._forceAngle) :
                        velocity.angle = position.getAngleToPoint(playerPosition) + self._currentMovementCommand._angle
                    else :
                        targetAngle = position.getAngleToPoint(playerPosition)
                        print("pls implement me")
                        #todo
            if masterPositions is not None :
                newPosition = CUS_Point(position._x, position._y)
                newPosition._x = newPosition._x + masterPositions[i]._x
                newPosition._y = newPosition._y + masterPositions[i]._y
                self._positionList.append(newPosition)
            else :   
                self._positionList.append(position)
                
            position = position.add(toPoint(velocity))

            if deleteBox is not None :
                if position._x < deleteBox[0] or position._x > deleteBox[2] or position._y < deleteBox[1] or position._y > deleteBox[3] :
                    self._deathCycle = i
                    break
                    

        if MOVEMENT_DEBUG :
            for i in self._positionList :
                print(i)
            print("done")

    def pullPositionAtCycle(self, cycle) :
        return self._positionList[cycle-self._spawningCycle]
