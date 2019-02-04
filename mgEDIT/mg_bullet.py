#classes that store values related to levels
from mg_cus_struct import *
from mg_movement import *
import copy

class BulletTemplate(object) :
    def __init__(self, animationName, initialVelocity) :
        self._animationName = animationName
        self._initialVelocity = initialVelocity
        self._movementList = dict()

    def addMovementCommand(self, cycle, movementCommand) :
        self._movementList[cycle] = movementCommand

class BulletSpawnerTemplate(object) :
    def __init__(self, initialPosition, initialVelocity) :
        self._initialPosition = initialPosition
        self._initialVelocity = initialVelocity
        
        self._movementList = dict()
        
        self._displacement = 0
        self._exitLocations = []
        self._rotationSpeed = 0
        
        self._initialDelay = 0
        self._sprayTimer = []
        self._inBetweenTimer = 0
        self._rounds = -1
        
        self._bulletTemplate = None

    def addSprayTimer(self, sprayTimer) :
        self._sprayTimer.extend(sprayTimer)

    def setRounds(self, rounds) :
        self._rounds = rounds

    def setInitialDelay(self, initialDelay) :
        self._initialDelay = initialDelay

    def setInBetweenTimer(self, delay) :
        self._inBetweenTimer = delay

    def addExitLocation(self, location) :
        self._exitLocations.append(location)

    def addBulletTemplate(self, bulletTemplate) :
        self._bulletTemplate = bulletTemplate

    def addMovementCommand(self, cycle, movementCommand) :
        self._movementList[cycle] = movementCommand
        
class BulletMasterTemplate(object) :
    def __init__(self, name) :
        self._name = name
        self._bulletSpawnerTemplates = []
        self._powerUpTable =  {
            "life" : 0,
            "power" : 0,
            "spell" : 0,
            "points" : 0,            
        }

    def addBulletSpawnerTemplates(self, bulletSpawnerTemplate) :
        self._bulletSpawnerTemplates.append(bulletSpawnerTemplate)
        
class Bullet(MovementCommander) :
    def __init__(self, bulletTemplate, position, exitAngle, master, spawningCycle) :
        temp = copy.deepcopy(bulletTemplate._initialVelocity)
        temp._angle = temp._angle + exitAngle

        super().__init__(position, temp, spawningCycle)
        self.addStartingParameters(position, temp)
        print(position)

        self._animationName = bulletTemplate._animationName

        for i in bulletTemplate._movementList :
            self.addMovementCommandDirect(i, bulletTemplate._movementList[i])

        deleteBox = None

        self.calculatePositions(master, master._playerPosition, [-100, -100, 1620, 1180], None)

class BulletSpawner(MovementCommander) :
    def __init__(self, bulletSpawnerTemplate, masterPosition, master, enemy, spawningCycle) :
        self._internalCounter = 0
        self._exitLocations = []
        self._displacement = 0.0

        self._master = master
        
        self._displacement = bulletSpawnerTemplate._displacement 
        for i in bulletSpawnerTemplate._exitLocations :
            self._exitLocations.append(i)
        self._rotationSpeed = bulletSpawnerTemplate._rotationSpeed
        self._bulletTemplate = bulletSpawnerTemplate._bulletTemplate

        self._spawningCycle = enemy._spawningCycle
        self._deathCycle = enemy._deathCycle
        
        self._sprayTimer = bulletSpawnerTemplate._sprayTimer
        self._initialDelay = bulletSpawnerTemplate._initialDelay
        self._lengthOfSpray = max(self._sprayTimer)
        self._inBetweenTimer = bulletSpawnerTemplate._inBetweenTimer
        self._rounds = bulletSpawnerTemplate._rounds

        super().__init__(bulletSpawnerTemplate._initialPosition, bulletSpawnerTemplate._initialVelocity, spawningCycle)
        
        self.calculatePositions(master, master._playerPosition, None, masterPosition)

    def calculateBullets(self) :
        returnList = []
        mode = "initialDelayMode"
        switchCounter = -1
        currentRound = 0
        for i in self._positionList :
            self._internalCounter = self._internalCounter + 1
            switchCounter = switchCounter + 1
            if mode == "initialDelayMode" :
                if switchCounter >= self._initialDelay :
                    mode = "sprayMode"
                    switchCounter = -1
            elif mode == "sprayMode" :
                if switchCounter in self._sprayTimer :
                    for j in self._exitLocations :
                        offset = CUS_Polar(self._displacement, j)
                        pos = CUS_Point(0.0, 0.0)
                        pos.add(toPoint(offset))
                        pos._x = pos._x + i._x
                        pos._y = pos._y + i._y
                        bullet = Bullet(self._bulletTemplate, pos, j, self._master, self._spawningCycle+self._internalCounter)
                        returnList.append(bullet)
                if switchCounter >= self._lengthOfSpray :
                    mode = "inBetweenTimerMode"
                    currentRound = currentRound + 1
                    switchCounter = -1
            elif mode == "inBetweenTimerMode" :
                print(currentRound)
                if switchCounter >= self._inBetweenTimer :
                    mode = "sprayMode"
                    switchCounter = -1
                if currentRound >= self._rounds and self._rounds is not -1 :
                    mode = "sprayOverMode"

        return returnList
                    
                    
            
class BulletMaster(object) :
    def __init__(self, bulletMasterTemplate, masterPositionList, master, enemy, spawningCycle) :
        self._name = bulletMasterTemplate._name

        self._bulletSpawners = []

        for i in bulletMasterTemplate._bulletSpawnerTemplates :
            self._bulletSpawners.append(BulletSpawner(i, masterPositionList, master, enemy, spawningCycle))

    def calculateBullets(self) :
        returnList =  []
        print(len(self._bulletSpawners))
        for i in self._bulletSpawners :
            returnList.extend(i.calculateBullets())

        return returnList
            

        
            
