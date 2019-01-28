#classes that store values related to levels
from mg_cus_struct import *
from mg_movement import *
import sys
                            
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


