from mg_cus_struct import *
from mg_popup import *
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
        
    #last cycle is the maximum possible cycle, usually    
    def calculatePositions(self, master, playerPosition) :
        self._positionList.clear()
        position  =  copy.deepcopy(self._initialPosition)
        velocity  =  copy.deepcopy(self._initialVelocity)
        self._currentMovementCommand = MovementCommand(True, False, False, 0, True, False, 0)

        lastCycle = min(master._maxCycles *300 + 10 - self._spawningCycle, self._deathCycle)
        
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

            temporaryVelocty = velocity
            if not self._currentMovementCommand._ignoreSpeed :
                if self._currentMovementCommand._forceSpeed :
                    velocity._magnitude = self._currentMovementCommand._speed
                else :
                    velocity._magnitude += self._currentMovementCommand._speed
            if not self._currentMovementCommand._ignoreAngle :
                if not self._currentMovementCommand._relativeToPlayer :
                    if self._currentMovementCommand._forceAngle :
                       velocty._angle = self._currentMovementCommand._angle
                    else :
                        velocity._angle += self._currentMovementCommand._angle
                else :
                    if (self._currentMovementCommand._forceAngle) :
                        velocity.angle = position.getAngleToPoint(playerPosition) + self._currentMovementCommand._angle
                    else :
                        targetAngle = position.getAngleToPoint(playerPosition)
                        print("pls implement me")
                        #todo
            self._positionList.append(position)
            position = position.add(toPoint(velocity))

        if MOVEMENT_DEBUG :
            for i in self._positionList :
                print(i)
            print("done")

    def pullPositionAtCycle(self, cycle) :
        return self._positionList[cycle-self._spawningCycle]

#represents a movement command that will be displayed in a list in MovementCommanderFrame
class MovementCommandFrame(tk.Frame) :
    def setCheckBoxUpdate(self) :
        if self._dontIgnoreAngle.get() :
            self._forceAngleRadio.configure(state = tk.NORMAL)
            self._addAngleRadio.configure(state = tk.NORMAL)
            self._relativeToPlayerButton.configure(state = tk.NORMAL)
            self._angleValue.configure(state = tk.NORMAL)
        else :
            self._forceAngleRadio.configure(state = tk.DISABLED)
            self._addAngleRadio.configure(state = tk.DISABLED)
            self._relativeToPlayerButton.configure(state = tk.DISABLED)
            self._angleValue.configure(state = tk.DISABLED)

        if self._dontIgnoreSpeed.get() :
            self._forceSpeedRadio.configure(state = tk.NORMAL)
            self._addSpeedRadio.configure(state = tk.NORMAL)
            self._speedValue.configure(state = tk.NORMAL)
        else :
            self._forceSpeedRadio.configure(state = tk.DISABLED)
            self._addSpeedRadio.configure(state = tk.DISABLED)
            self._speedValue.configure(state = tk.DISABLED)

    def setValues(self) :
        self._cycleEntry.delete(0, tk.END)
        self._cycleEntry.insert(0, str(self._cycle))

        if not self._command._ignoreAngle :
            self._dontIgnoreAngle.set(1)
        else :
            self._dontIgnoreAngle.set(0)
            
        if not self._command._ignoreSpeed :
            self._dontIgnoreSpeed.set(1)
        else :
            self._dontIgnoreSpeed.set(0)
            
        self.setCheckBoxUpdate()

    def updateValues(self) :
        if self._dontIgnoreAngle.get() :
            print("angle not ignored")
            self._command._ignoreAngle = 0
            self._command._forceAngle = self._forceAngle.get()
            self._command._relativeToPlayer = self._relativeToPlayer.get()
            self._command._angle = float(self._angleValue.get())
            print(self._angleValue.get())
        else :
            print("angle ignored")
            self._command._ignoreAngle = 1
            self._command._forceAngle = 0
            self._command._relativeToPlayer = 0
            self._command._angle = 0

        if self._dontIgnoreSpeed.get() :
            print("speed not ignored")
            self._command._ignoreSpeed = 0
            self._command._forceSpeed = self._forceSpeed.get()
            self._command._speed = float(self._speedValue.get())
        else :
            print("speed ignored")
            self._command._ignoreSpeed = 1
            self._command._forceSpeed = 0
            self._command._speed = 0

        del self._commander._movementCommands[self._cycle]
        self._cycle = int(self._cycleEntry.get())
        self._commander.addMovementCommand(self._cycle,
                                           self._command._ignoreAngle, self._command._forceAngle,
                                           self._command._relativeToPlayer, self._command._angle,
                                           self._command._ignoreSpeed, self._command._forceSpeed,
                                           self._command._speed)
        
        self._commander.calculatePositions(self._master, self._master._playerPosition)
        self._master.enemyListUpdate()
        self._master.drawCanvasForFrame();
    
    def __init__(self, master, rt, cycle, command, commander) :
        tk.Frame.__init__(self, rt,
                          highlightbackground = "red", highlightcolor = "red",
                          highlightthickness=1, width = 626 * master._scale, bd = 0, height = 90)
        self._master = master
        self._command = command
        self._commander = commander

        self._cycle = cycle
        tk.Label(self, text = "MOVE Cycle: ").grid(row = 0, column = 0)
        self._cycleEntry =  tk.Entry(self)
        self._cycleEntry.grid(row = 0, column = 1)
        self._cycleLabel =  tk.Label(self, text = "(GLOBAL:%d)"%(cycle + commander._spawningCycle))
        self._cycleLabel.grid(row = 0, column = 2)

        self._dontIgnoreAngle = tk.IntVar()
        self._angleCheck = tk.Checkbutton(self, text = "Angle", variable = self._dontIgnoreAngle, command=self.setCheckBoxUpdate)
        self._angleCheck.grid(row = 1, column = 0)

        self._dontIgnoreSpeed = tk.IntVar();
        self._speedCheck = tk.Checkbutton(self, text = "Speed", variable = self._dontIgnoreSpeed, command=self.setCheckBoxUpdate)
        self._speedCheck.grid(row = 2, column = 0)
        
        self._forceAngle = tk.IntVar();
        self._forceAngle.set(command._forceAngle)
        self._forceAngleRadio = tk.Radiobutton(self, text="Force Angle", variable=self._forceAngle, value=1)
        self._forceAngleRadio.grid(row = 1, column = 1)
        self._addAngleRadio = tk.Radiobutton(self, text="Add Angle", variable=self._forceAngle, value=0)
        self._addAngleRadio.grid(row = 1, column = 2)

        self._forceSpeed = tk.IntVar();
        self._forceSpeed.set(command._forceSpeed)
        self._forceSpeedRadio = tk.Radiobutton(self, text="Force Speed", variable=self._forceSpeed, value=1)
        self._forceSpeedRadio.grid(row = 2, column = 1)
        self._addSpeedRadio = tk.Radiobutton(self, text="Add Speed", variable=self._forceSpeed, value=0)
        self._addSpeedRadio.grid(row = 2, column = 2)

        self._relativeToPlayer = tk.IntVar();
        self._relativeToPlayer.set(command._relativeToPlayer)
        self._relativeToPlayerButton = tk.Checkbutton(self, text = "Relative To Player", variable = self._relativeToPlayer)
        self._relativeToPlayerButton.grid(row = 1, column = 3)

        tk.Label(self, text = "Value: ").grid(row = 1, column = 4)
        self._angleValue = tk.Entry(self)
        self._angleValue.insert(0, str(command._angle))
        self._angleValue.grid(row = 1, column = 5)

        tk.Label(self, text = "Value: ").grid(row = 2, column = 4)
        self._speedValue = tk.Entry(self)
        self._speedValue.insert(0, str(command._speed))
        self._speedValue.grid(row = 2, column = 5)

        tk.Button(self, text = "Update", command = self.updateValues).grid(row=1, column=6)        

        self.setValues()
    

#Describes initial setup for enemy
class EnemyFrame(tk.Frame) :
    def handleAnimation(self) :
        pop = popAnimationSelector(self._master)
        self.wait_window(pop._window)
        if pop.valid :
            self._animationEntry.delete(0, tk.END)
            self._animationEntry.insert(0, pop.value)
        else :
            messagebox.showerror("Error", "No animation selected")

    def updateValues(self) :
        self._enemy._spawningCycle = int(self._cycleNewGlobal.get())

        self._enemy._hitpoints = int(self._hitpointEntry.get())
        self._enemy._hitbox = float(self._hitboxEntry.get())

        position = CUS_Point(float(self.__posX.get()), float(self.__posY.get()))
        velocity = CUS_Polar(float(self.__velX.get()), float(self.__velY.get()))

        self._enemy.addStartingParameters(position, velocity)

        self._enemy.calculatePositions(self._master, self._master._playerPosition)
        self._master.enemyListUpdate()
        self._rt.drawFrame()
        self._master.drawCanvasForFrame()

    def fillValues(self) :
        self._cycleNewGlobal.delete(0, tk.END)
        self._cycleNewGlobal.insert(0, str(self._enemy._spawningCycle))
        
        self._animationEntry.delete(0, tk.END)
        self._animationEntry.insert(0, str(self._enemy._animationName))

        self._hitpointEntry.delete(0, tk.END)
        self._hitpointEntry.insert(0, str(self._enemy._hitpoints))

        self._hitboxEntry.delete(0, tk.END)
        self._hitboxEntry.insert(0, str(self._enemy._hitbox))

        self.__posX.delete(0, tk.END)
        self.__posX.insert(0, self._enemy._initialPosition._x)

        self.__posY.delete(0, tk.END)
        self.__posY.insert(0, self._enemy._initialPosition._y)

        self.__velX.delete(0, tk.END)
        self.__velX.insert(0, self._enemy._initialVelocity._magnitude)
        self.__velY.delete(0, tk.END)
        self.__velY.insert(0, self._enemy._initialVelocity._angle)
        
    
    def __init__(self, master, rt, enemy) :
        tk.Frame.__init__(self, rt,
                          highlightbackground = "blue", highlightcolor = "blue",
                          highlightthickness=1, width = 626 * master._scale, bd = 0, height = 93)
        self._master = master
        self._enemy = enemy
        self._rt = rt
        self._cycleLabel =  tk.Label(self, text = "SPAWN Cycle: 0")
        self._cycleLabel.grid(row = 0, column = 0)

        tk.Label(self, text = "GLOBAL:").grid(row = 0, column = 2)
        
        self._cycleNewGlobal = tk.Entry(self)
        self._cycleNewGlobal.grid(row = 0, column = 1)
        

        tk.Label(self, text = " Using animation: ").grid(row = 0, column = 3)
        self._animationEntry = tk.Entry(self)
        self._animationEntry.grid(row = 0, column = 4)

        (tk.Button(self, text="...", command=self.handleAnimation)).grid(row = 0, column = 5)

        tk.Label(self, text = "HitPoints: ").grid(row = 1, column = 0)
        self._hitpointEntry = tk.Entry(self)
        self._hitpointEntry.grid(row=1, column=1)
        
        tk.Label(self, text = "HitBox: ").grid(row = 1, column = 3)
        self._hitboxEntry = tk.Entry(self)
        self._hitboxEntry.grid(row=1, column=4)

        (tk.Label(self, text = "Starting Pos:")).grid(row=2, column=0)
        self.__posX = tk.Entry(self)
        self.__posX.grid(row=2, column=1)
        (tk.Label(self, text = "X")).grid(row=2, column=2)
        self.__posY = tk.Entry(self)
        self.__posY.grid(row=3, column=1)
        (tk.Label(self, text = "Y")).grid(row=3, column=2)

        (tk.Label(self, text = "Starting Vel:")).grid(row=2, column=3)
        self.__velX = tk.Entry(self)
        self.__velX.grid(row=2, column=4)
        (tk.Label(self, text = "Magnitude")).grid(row=2, column=5)
        self.__velY = tk.Entry(self)
        self.__velY.grid(row=3, column=4)
        tk.Label(self, text = "Angle").grid(row=3, column=5)

        tk.Button(self, text = "Update", command = self.updateValues).grid(row=0, column=6)

        self.fillValues()

class DeathFrame(tk.Frame) :
    def setValues(self) :
        if self._enemy._deathCycle == sys.maxsize :
            self._cycleDeathEntry.delete(0, tk.END)
            self._localDeathCyclefloat["text"] = "NOT SET"
            self._globalDeath["text"] = "NOT SET"
            self._globalDeathFloat["text"] = "NOT SET"
        else :
            self._cycleDeathEntry.delete(0, tk.END)
            self._cycleDeathEntry.insert(0, str(self._enemy._deathCycle))
            self._localDeathCyclefloat["text"] = "(%f)"%(self._enemy._deathCycle/300)
            self._globalDeath["text"] = "%d"%(self._enemy._deathCycle + self._enemy._spawningCycle)
            self._globalDeathFloat["text"] = "(%f)"%((self._enemy._deathCycle + self._enemy._spawningCycle)/300)

    def update(self) :
        self._enemy._deathCycle = int(self._cycleDeathEntry.get())
        self._enemy._deathInitialised = True
        self._enemy.calculatePositions(self._master, self._master._playerPosition)
        self.setValues()
        self._master.enemyListUpdate()
        self._master.drawCanvasForFrame()
        self._rt.drawFrame()
        
    
    def __init__(self, master, rt, enemy) :
        tk.Frame.__init__(self, rt,
                          highlightbackground = "black", highlightcolor = "black",
                          highlightthickness=1, width = 626 * master._scale, bd = 0, height = 30)
        self._master = master
        self._enemy = enemy
        self._rt = rt
        
        tk.Label(self, text = "DEATH Cycle: ").grid(row = 0, column = 0)

        self._cycleDeathEntry = tk.Entry(self)
        self._cycleDeathEntry.grid(row = 0, column = 1)

        self._localDeathCyclefloat = tk.Label(self, text = "(%f)")
        self._localDeathCyclefloat.grid(row = 0, column = 2)
        tk.Label(self, text = "GLOBAL: ").grid(row = 0, column = 3)
        self._globalDeath = tk.Label(self, text = "GLOBAL: ")
        self._globalDeath.grid(row = 0, column = 4)
        self._globalDeathFloat = tk.Label(self, text = "GLOBAL: ")
        self._globalDeathFloat.grid(row = 0, column = 5)

        tk.Button(self, text="Update", command=self.update).grid(row = 0, column = 6)

        self.setValues()

class SpawnNewFrame(tk.Frame) :
    def newCommand(self) :
        pass
    
    def __init__(self, rt, enemy) :
        self._enemy = enemy
        self._rt = rt

        tk.Label(self, text = "No Commander Selected").grid(row = 0, column = 0)
        tk.Label(self, text = "No Commander Selected").grid(row = 0, column = 1)
        tk.Button(self, text="Update", command=self.newCommand).grid(row = 0, column = 2)
        
        
#Whats usually on the right side of the screen that describes current movement commander
#Gets called everytime a new enemy comes into focus
class MovementCommanderFrame(tk.Frame) :    
    def drawFrame(self) :
        for i in self._commandFrames :
            i.destroy()
        if self._commander is None :
            tk.Label(self, text = "No Commander Selected").grid(column = 0, row = 0)
        else :
            commandFrame = EnemyFrame(self._master, self, self._commander)
            commandFrame.grid(column = 0, row = 0)
            self._commandFrames.append(commandFrame)
            j = 1
            for i in sorted(self._commander._movementCommands.keys()) :
                commandFrame = MovementCommandFrame(self._master, self, i,
                                                    self._commander._movementCommands[i], self._commander)
                commandFrame.grid(column = 0, row = j)
                self._commandFrames.append(commandFrame)
                j = j + 1
            commandFrame = DeathFrame(self._master, self, self._commander)
            commandFrame.grid(column = 0, row = j)
            self._commandFrames.append(commandFrame)
            j = j + 1

                    
    def __init__(self, master, commander) :
        self._master = master
        tk.Frame.__init__(self, master._windows["commander"],
                          highlightbackground = "green", highlightcolor = "green",
                          highlightthickness = 5, width = 626 * master._scale, bd = 0)
        
        
        self._commander = commander
        self._commandFrames = []
        self.drawFrame()
        

        
