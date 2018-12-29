#classes that store values related to levels
from mg_cus_struct import *
import sys

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
        movementCommands[cycle] = MovementCommand(ignoreAngle, forceAngle, relativeToPlayer, angle, ignoreSpeed, forceSpeed, speed)

    def calculatePositions(self, lastCycle, playerPosition) :
        position  =  self._initialPosition
        velocity  =  self._initialVelocity
        self._currentMovementCommand = MovementCommand(True, False, False, 0, True, False, 0)
        temporaryVelocty = CUS_Polar(0,0)
        for i in range(0,lastCycle) :
            if i in self._movementCommands :
                if not self._movementCommands[i]._ignoreAngle :
                    self._currentMovementCommand._ignoreAngle = False
                    self._currentMovementCommand._forceAngle = movementCommands[i]._forceAngle
                    self._currentMovementCommand._relativeToPlayer = movementCommands[i]._relativeToPlayer
                    self._currentMovementCommand._angle = movementCommands[i]._angle
                if not self._movementCommands[i]._ignoreSpeed :
                    self._currentMovementCommand._ignoreSpeed = False
                    self._currentMovementCommand._forceSpeed = movementCommands[i]._forceSpeed
                    self._currentMovementCommand._speed = movementCommands[i]._speed

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

        for i in self._positionList :
            print(i)
        print("done")

    def pullPositionAtCycle(self, cycle) :
        return self._positionList[cycle-self._spawningCycle]
                            
class EnemyEntity(MovementCommander) :
    def __init__(self, spawningCycle, animationName, positionStart, velocityStart, hitbox, hitpoints) :
        #movement commands
        self._deathCycle = sys.maxsize
        self._deathInitialised = False
        self._hitpoints = hitpoints
        self._hitpbox = hitbox
        super().__init__(positionStart, velocityStart, spawningCycle)
        
        self._animationName = animationName

    def setDeathCycle(self, deathCycle) :
        self._deathInitialised = True
        self._deathCycle = deathCycle


def enemySort(obj) :
    return obj._spawningCycle
