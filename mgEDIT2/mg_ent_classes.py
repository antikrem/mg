#classes that store values related to levels
from mg_cus_struct import *

#stores a command that on starting frame will be applied
class MovementCommand(object) :
    #speed variables
    _ignoreAngle = False
    _forceAngle = False
    _angle = 0
    #speed variables
    _ignoreSpeed = False
    _forceSpeed = False
    _speed = 0

    def __init__(self, ignoreAngle, forceAngle, angle, ignoreSpeed, forceSpeed, speed) :
        self._ignoreAngle = ignoreAngle
        self._forceAngle = forceAngle
        self._angle = angle
        self._ignoreSpeed = ignoreSpeed
        self._forceSpeed = forceSpeed
        self._speed = speed

class EnemyEntity(object) :
    def __init__(self, spawningCycle, animationName, positionStart, velocityStart) :
        #movement commands
        self.__movementCommandsToUse = dict()
        self.__movementCommandInUse = MovementCommand(False, False, 0, False, False, 0)
        self._spawningCycle = spawningCycle
        self._deathCycle = 0
        
        self._animationName = animationName
        self._positions = []
        self._velocity = []
        self._positions.append(positionStart)
        self._velocity.append(velocityStart)

    def setDeathCycle(self, deathCycle) :
        self._deathCycle = deathCycle
    
    def addMovementCommand(self, startingFrame, ignoreAngle, forceAngle, angle, ignoreSpeed, forceSpeed, speed) :
        self.__movementCommandsToUse[startingFrame] = MovementCommand(ignoreAngle, forceAngle, angle, ignoreSpeed, forceSpeed, speed)

    def populatePosition(self) :
        counter = -1

        while (counter <= self._deathCycle) :
            counter+=1
            
            if counter in self.__movementCommandsToUse:
                if not self.__movementCommandsToUse[counter]._ignoreAngle:
                    self.__movementCommandInUse._forceAngle = self.__movementCommandsToUse[counter]._forceAngle
                    self.__movementCommandInUse._angle = self.__movementCommandsToUse[counter]._angle
                if not self.__movementCommandsToUse[counter]._ignoreSpeed:
                    self.__movementCommandInUse._forceSpeed = self.__movementCommandsToUse[counter]._forceSpeed
                    self.__movementCommandInUse._speed = self.__movementCommandsToUse[counter]._speed

            tempVelocity = toPolar(self._velocity[-1])
            if self.__movementCommandInUse._forceAngle :
                tempVelocity._angle = self.__movementCommandInUse._angle
            else :
                tempVelocity._angle += self.__movementCommandInUse._angle
                    
            if self.__movementCommandInUse._forceSpeed :
                tempVelocity._magnitude = self.__movementCommandInUse._speed
            else :
                tempVelocity._magnitude += self.__movementCommandInUse._speed

            self._velocity.append(toPoint(tempVelocity))
            self._positions.append(self._positions[-1].add(self._velocity[-1]))
