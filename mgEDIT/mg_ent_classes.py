#classes that store values related to levels
from mg_cus_struct import *
from mg_movement import *
from mg_bullet import *
import sys
import copy
                            
class EnemyEntity(MovementCommander) :
    def __init__(self, spawningCycle, animationName, positionStart, velocityStart, hitbox, hitpoints) :
        #movement commands
        self._hitpoints = hitpoints
        self._hitbox = hitbox
        
        self._bulletMaster = ""
        self._bulletMasterTime = 0

        self._bulletMasterDirect = None
        
        super().__init__(positionStart, velocityStart, spawningCycle)
        
        self._animationName = animationName

    def setDeathCycle(self, deathCycle) :
        self._deathInitialised = True
        self._deathCycle = deathCycle

    def setBulletMaster(self, bulletMasterTime, bulletMaster) :
        self._bulletMaster = bulletMaster
        self._bulletMasterTime = bulletMasterTime

    def calculateBulletMaster(self, bulletTemplates, master) :
        if self._bulletMaster is not "" :
            newPositionList = copy.deepcopy(self._positionList)
            newPositionList = newPositionList[self._bulletMasterTime:]
            self._bulletMasterDirect = BulletMaster(
                bulletTemplates[self._bulletMaster],
                newPositionList,
                master,
                self,
                self._spawningCycle + self._bulletMasterTime)
            return self._bulletMasterDirect.calculateBullets()
        else :
            return []

def enemySort(obj) :
    return obj._spawningCycle


