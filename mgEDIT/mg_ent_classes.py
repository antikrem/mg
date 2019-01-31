#classes that store values related to levels
from mg_cus_struct import *
from mg_movement import *
import sys
                            
class EnemyEntity(MovementCommander) :
    def __init__(self, spawningCycle, animationName, positionStart, velocityStart, hitbox, hitpoints) :
        #movement commands
        self._hitpoints = hitpoints
        self._hitbox = hitbox
        self._bulletMaster = ""
        self._bulletMasterTime = 0
        super().__init__(positionStart, velocityStart, spawningCycle)
        
        self._animationName = animationName

    def setDeathCycle(self, deathCycle) :
        self._deathInitialised = True
        self._deathCycle = deathCycle

    def setBulletMaster(self, bulletMasterTime, bulletMaster) :
        self._bulletMaster = bulletMaster
        self._bulletMasterTime = bulletMasterTime


def enemySort(obj) :
    return obj._spawningCycle


