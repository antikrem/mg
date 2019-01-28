from mg_cus_struct import *
import tkinter as tk

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

    def addMovementCommand(self, cycle, ignoreAngle, forceAngle, relativeToPlayer, angle, ignoreSpeed, forceSpeed, speed) :
        self._movementCommands[cycle] = MovementCommand(ignoreAngle, forceAngle, relativeToPlayer, angle, ignoreSpeed, forceSpeed, speed)

    def calculatePositions(self, lastCycle, playerPosition) :
        position  =  self._initialPosition
        velocity  =  self._initialVelocity
        self._currentMovementCommand = MovementCommand(True, False, False, 0, True, False, 0)
        temporaryVelocty = CUS_Polar(0,0)
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

            temporaryVelocty = toPolar(velocity)
            if not self._currentMovementCommand._ignoreSpeed :
                if self._currentMovementCommand._forceSpeed :
                    temporaryVelocty._magnitude = self._currentMovementCommand._speed
                else :
                    temporaryVelocty._magnitude += self._currentMovementCommand._speed
            if not self._currentMovementCommand._ignoreAngle :
                if not self._currentMovementCommand._relativeToPlayer :
                    if self._currentMovementCommand._forceAngle :
                        temporaryVelocty._angle = self._currentMovementCommand._angle
                    else :
                        temporaryVelocty._angle += self._currentMovementCommand._angle
                else :
                    if (self._currentMovementCommand._forceAngle) :
                        temporaryVelocity.angle = position.getAngleToPoint(playerPosition) + self._currentMovementCommand._angle
                    else :
                        targetAngle = position.getAngleToPoint(playerPosition)
                        print("pls implement me")
                        #todo
            self._positionList.append(position)
            velocity = toPoint(temporaryVelocty)
            position = position.add(velocity)

        if MOVEMENT_DEBUG :
            for i in self._positionList :
                print(i)
            print("done")

    def pullPositionAtCycle(self, cycle) :
        return self._positionList[cycle-self._spawningCycle]

#represents a movement command that will be displayed in a list in MovementCommanderFrame
class MovementCommandFrame(tk.Frame) :
    def __init__(self, master, rt, cycle, command) :
        tk.Frame.__init__(self, rt)
        self._master = master
        self._command = command

        self._cycle = cycle
        self._cycleLabel =  tk.Label(self, text = "Cycle: %d"%cycle)
        self._cycleLabel.grid(column = 0, row = 0)
        
#Whats usually on the right side of the screen that describes current movement commander
#Gets called everytime a new enemy comes into focus
class MovementCommanderFrame(tk.Frame) :
    def __init__(self, master, commander) :
        self._master = master
        tk.Frame.__init__(self, master._windows["commander"],bg="red")
        
        self._commander = commander
        self._MovementCommandFrames = []

        if commander is None :
            tk.Label(self, text = "No Commander Selected").grid(column = 0, row = 0)
        else :
            print(len(commander._movementCommands))
            j = -1
            for i in commander._movementCommands :
                j = j + 1
                commandFrame = MovementCommandFrame(master, self, i, commander._movementCommands[i])
                commandFrame.grid(column = 0, row = j)
