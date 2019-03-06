from mg_movement import *
from mg_popup import *

#represents a movement command that will be displayed in a list in MovementCommanderFrame
class MovementCommandFrame(tk.Frame) :
    def updateOnSlide(self, value) :
        try :
            if value >= self._commander._spawningCycle and value <= ( self._commander._spawningCycle + self._commander._deathCycle) :
                self._shiftValue = value-self._commander._spawningCycle
                self._shiftButton["text"] = "Shift to: %d"%self._shiftValue
            elif value > ( self._commander._spawningCycle + self._commander._deathCycle) :
                self._shiftValue = self._commander._deathCycle
                self._shiftButton["text"] = "Shift to: >%d"%self._commander._deathCycle
            elif value < ( self._commander._spawningCycle) :
                self._shiftValue = 0
                self._shiftButton["text"] = "Shift to: <0"
        except AttributeError :
            pass

    def shiftUpdate(self) :
        command = self._command
        #Enemy list view
        try :
            del self._commander._movementCommands[self._cycle]
            self._commander._movementCommands[self._shiftValue] = self._command
            self._rt.drawFrame()
            self._commander.calculatePositions(self._master, self._master._playerPosition, None, None)
            self._master.drawCanvasForFrame()
        #Bullet master view
        except AttributeError :
            del self._commander._movementList[self._cycle]
            self._commander._movementList[self._shiftValue] = self._command
            self._rt.drawFrame()
            self._master.updateAllEnemiesBM()
        
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
        
    def deleteMe(self) :
        #Enemy list view
        try :
            del self._commander._movementCommands[self._cycle]
            self._rt.drawFrame()
            self._commander.calculatePositions(self._master, self._master._playerPosition, None, None)
            self._master.drawCanvasForFrame()
        #Bullet master view
        except AttributeError :
            del self._commander._movementList[self._cycle]
            self._rt.drawFrame()
            self._master.updateAllEnemiesBM()

    def updateValues(self) :
        if self._dontIgnoreAngle.get() :
            print("angle not ignored")
            self._command._ignoreAngle = 0
            self._command._forceAngle = self._forceAngle.get()
            self._command._relativeToPlayer = self._relativeToPlayer.get()
            self._command._angle = float(self._angleValue.get())
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

        command = MovementCommand(self._command._ignoreAngle, self._command._forceAngle,
                                        self._command._relativeToPlayer, self._command._angle,
                                        self._command._ignoreSpeed, self._command._forceSpeed,
                                        self._command._speed)
        #Enemy list view
        try :
            del self._commander._movementCommands[self._cycle]
            self._cycle = int(self._cycleEntry.get())
            self._commander.addMovementCommandDirect(self._cycle,command)
            self._commander.calculatePositions(self._master, self._master._playerPosition, None, None)
            self._rt.drawFrame()
        #Bullet master view
        except AttributeError :
            del self._commander._movementList[self._cycle]
            self._cycle = int(self._cycleEntry.get())
            self._commander.addMovementCommand(self._cycle, command)
            self._master.updateAllEnemiesBM()
            
        
        
        self._master.enemyListUpdate()
        self._master.drawCanvasForFrame()
    
    def __init__(self, master, rt, cycle, command, commander) :
        tk.Frame.__init__(self, rt,
                          highlightbackground = "red", highlightcolor = "red",
                          highlightthickness=1, width = 626 * master._scale, bd = 0, height = 90)
        self._master = master
        self._rt = rt
        self._command = command
        self._commander = commander

        self._cycle = cycle
        self._shiftValue = 0
        
        tk.Label(self, text = "MOVE Cycle: ").grid(row = 0, column = 0)
        self._cycleEntry =  tk.Entry(self)
        self._cycleEntry.grid(row = 0, column = 1)
        self._cycleLabel = tk.Label(self, text = "(GLOBAL:%d)"%(cycle + commander._spawningCycle))
        self._cycleLabel.grid(row = 0, column = 2)
        self._shiftButton = tk.Button(self, text = "Shift to: 0", command = self.shiftUpdate)
        self._shiftButton.grid(row = 0, column = 3)

        self._dontIgnoreAngle = tk.IntVar()
        self._angleCheck = tk.Checkbutton(self, text = "Angle", variable = self._dontIgnoreAngle, command=self.setCheckBoxUpdate)
        self._angleCheck.grid(row = 1, column = 0)

        self._dontIgnoreSpeed = tk.IntVar()
        self._speedCheck = tk.Checkbutton(self, text = "Speed", variable = self._dontIgnoreSpeed, command=self.setCheckBoxUpdate)
        self._speedCheck.grid(row = 2, column = 0)
        
        self._forceAngle = tk.IntVar()
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

        self._updateValue = 0

        tk.Button(self, text = "Update", command = self.updateValues).grid(row=0, column=6, rowspan=2, sticky=tk.N+tk.S+tk.E+tk.W)
        tk.Button(self, text = "Delete", command = self.deleteMe).grid(row=2, column=6, sticky=tk.N+tk.S+tk.E+tk.W)

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

        if self._bulletMasterVar.get() :
            self._enemy._bulletMaster = ""
            self._enemy._bulletMasterTimer = 0
        else :
            self._enemy._bulletMaster = self._bulletMasterName.get()
            self._enemy._bulletMasterTime = int(self._delayEntry.get())

        self._enemy.addStartingParameters(position, velocity)

        self._enemy.calculatePositions(self._master, self._master._playerPosition, None, None)
        self._master.enemyListUpdate()
        self._rt.drawFrame()
        self._master.drawCanvasForFrame()

    def updateOnSlide(self, value) :
        if value >= self._enemy._spawningCycle and value <= ( self._enemy._spawningCycle + self._enemy._deathCycle) :
            self._updateValue = value-self._enemy._spawningCycle
            self._addButton["text"] = "Add MOVE\n%d"%self._updateValue
        elif value > ( self._enemy._spawningCycle + self._enemy._deathCycle) :
            self._updateValue = self._enemy._deathCycle
            self._addButton["text"] = "Add MOVE\n>%d"%self._enemy._deathCycle
        elif value < ( self._enemy._spawningCycle) :
            self._updateValue = 0
            self._addButton["text"] = "Add MOVE\n<0"
        
    def addHandle(self) :
        self._enemy.addMovementCommand(self._updateValue, True, False, False, 0, True, False, 0)
        print(len(self._enemy._movementCommands))
        self._rt.drawFrame()

    def noBulletMaster(self) :
        if self._bulletMasterVar.get() :
            self._bulletMasterName.configure(state = tk.DISABLED)
            self._bulletMasterPicker.configure(state = tk.DISABLED)
            self._delayEntry.configure(state = tk.DISABLED)
        else :
            self._bulletMasterName.configure(state = tk.NORMAL)
            self._bulletMasterPicker.configure(state = tk.NORMAL)
            self._delayEntry.configure(state = tk.NORMAL)

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

        if (self._enemy._bulletMaster == "") :
            self._bulletMasterVar.set(1)
        else :
            self._bulletMasterVar.set(0)
            self._bulletMasterName.delete(0, tk.END)
            self._bulletMasterName.insert(0, self._enemy._bulletMaster)
            self._delayEntry.delete(0, tk.END)
            self._delayEntry.insert(0, self._enemy._bulletMasterTime)
        self.noBulletMaster()
        
    
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

        (tk.Button(self, text="...", command=self.handleAnimation)).grid(row = 0, column = 5, sticky=tk.E+tk.W)

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

        self._bulletMasterVar = tk.IntVar();
        self._bulletMasterButton = tk.Checkbutton(self, text="No BulletMaster", variable=self._bulletMasterVar, onvalue = 1, offvalue = 0, command = self.noBulletMaster)
        self._bulletMasterButton.grid(row = 4, column = 0)
        self._bulletMasterName = tk.Entry(self)
        self._bulletMasterName.grid(row=4, column=1, sticky=tk.E+tk.W)
        self._bulletMasterPicker = tk.Button(self, text="...", command=None)
        self._bulletMasterPicker.grid(row = 4, column = 2, sticky=tk.E+tk.W)
        (tk.Label(self, text = "Delay:")).grid(row=4, column=3)
        self._delayEntry = tk.Entry(self)
        self._delayEntry.grid(row=4, column=4, sticky=tk.E+tk.W)

        self._updateValue = 1
        tk.Button(self, text = "Update", command = self.updateValues).grid(row=0, column=6, rowspan=3, sticky=tk.E+tk.W+tk.N+tk.S)
        self._addButton = tk.Button(self, text = "Add MOVE\n%d"%self._enemy._spawningCycle, command = self.addHandle)
        self._addButton.grid(row=3, column=6, rowspan=2, sticky=tk.E+tk.W+tk.N+tk.S)
        
        self.fillValues()

class DeathFrame(tk.Frame) :
    def updateOnSlide(self, value) :
        pass
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
        self._enemy.calculatePositions(self._master, self._master._playerPosition, None, None)
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
        
#Whats usually on the right side of the screen that describes current movement commander
#Gets called everytime a new enemy comes into focus
class MovementCommanderFrame(tk.Frame) :
    def updateOnSlide(self, value) :
        for i in self._commandFrames :
            i.updateOnSlide(value)
    def drawFrame(self) :
        for i in self._commandFrames :
            i.destroy()
        self._commandFrames.clear()
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

#Describes initial setup for bullet master
class BulletMasterSetupFrame(tk.Frame) :
    def updateOnSlide(self, value) :
        pass
    def updateValues(self) :
        self._enemy._spawningCycle = int(self._cycleNewGlobal.get())

        self._enemy._hitpoints = int(self._hitpointEntry.get())
        self._enemy._hitbox = float(self._hitboxEntry.get())

        position = CUS_Point(float(self.__posX.get()), float(self.__posY.get()))
        velocity = CUS_Polar(float(self.__velX.get()), float(self.__velY.get()))

        self._enemy.addStartingParameters(position, velocity)

        self._enemy.calculatePositions(self._master, self._master._playerPosition, None, None)
        self._master.enemyListUpdate()
        self._rt.drawFrame()
        self._master.drawCanvasForFrame()
    
    def __init__(self, master, rt, template) :
        tk.Frame.__init__(self, rt,
                          highlightbackground = "blue", highlightcolor = "blue",
                          highlightthickness=1, width = 626 * master._scale, bd = 0, height = 93)
        self._master = master
        self._template = template
        self._rt = rt
        self._cycleLabel =  tk.Label(self, text = "BULLET MASTER DESCRIPTION")
        self._cycleLabel.grid(row = 0, column = 0, columnspan = 2)

        (tk.Label(self, text = "BM Name")).grid(row = 1, column = 0)
        self._nameLabel =  tk.Entry(self)
        self._nameLabel.insert(0, template._name)
        self._nameLabel.grid(row = 1, column = 1)

#Each spawner has a display frame
class BulletSpawnerFrame(tk.Frame) :
    def updateOnSlide(self, value) :
        pass
    def noMaskCallback(self) :
        if self._spriteMask.get() :
            self._animationEntry.configure(state='disable')
            self._animationSelectButton.configure(state='disable')
        else :
            self._animationEntry.configure(state='normal')
            self._animationSelectButton.configure(state='normal')
            
    def noLimitCallback(self) :
        if self._roundLimit.get() :
            self._roundLimitEntry.configure(state='disable')
        else :
            self._roundLimitEntry.configure(state='normal')
    
    def handleAnimation(self) :
        pop = popAnimationSelector(self._master)
        self.wait_window(pop._window)
        if pop.valid :
            self._animationEntry.delete(0, tk.END)
            self._animationEntry.insert(0, pop.value)
        else :
            messagebox.showerror("Error", "No animation selected")

    def updateValues(self) :
        template = self._template

        try :
            template._initialDelay = int(self._delayEntry.get())
        except ValueError :
            messagebox.showerror("Error", "Delay is not a whole number")

        try :
            if self._roundLimit.get() :
                template._rounds = -1
            else :
                template._rounds = int(self._roundLimitEntry.get())
        except ValueError :
            messagebox.showerror("Error", "Rounds is not a whole number")

        try :
            template._inBetweenTimer = int(self._delayBetweenRoundsEntry.get())
        except ValueError :
            messagebox.showerror("Error", "Delay between rounds is not a whole number")
            
        try :
            template._displacement = float(self._displacementEntry.get())
        except ValueError :
            messagebox.showerror("Error", "Displacement is not a number")

        try :
            template._sprayTimer.clear()
            for i in self._sprayTimingEntry.get().strip(' ').split(" ") :
                template._sprayTimer.append(int(i))
        except ValueError :
            messagebox.showerror("Error", "Spray Timer is not a series of whole numbers split by spaces")

        try :
            template._exitLocations.clear()
            for i in self._exitsEntry.get().strip(' ').split(" ") :
                template._exitLocations.append(float(i))
        except ValueError :
            messagebox.showerror("Error", "Exit Locations is not a series of numbers split by spaces")

        try :
            template._initialPosition._x = float(self._posX.get())
            template._initialPosition._y = float(self._posY.get())
        except ValueError :
            messagebox.showerror("Error", "Position is not two numbers")

        try :
            template._initialVelocity._magnitude = float(self._velX.get())
            template._initialVelocity._angle = float(self._velY.get())
        except ValueError :
            messagebox.showerror("Error", "Position is not two numbers")

        self._master.updateAllEnemiesBM()

    def fillValues(self) :
        template = self._template

        #sprite mask
        if template._maskName == "" :
            self._spriteMask.set(1)
        else :
            self._spriteMask.set(0)
            self._animationEntry.insert(0, str(template._maskName))
        self.noMaskCallback()
        
        if template._rounds is not -1 :
            self._roundLimitEntry.insert(0, str(template._rounds))
        else :
            self._roundLimit.set(1)
        self.noLimitCallback()

        self._delayEntry.insert(0, str(template._initialDelay))
        self._delayBetweenRoundsEntry.insert(0, str(template._inBetweenTimer))
        self._displacementEntry.insert(0, str(template._displacement))

        for i in template._sprayTimer :
            self._sprayTimingEntry.insert(tk.END, str(i)+" ")

        for i in template._exitLocations :
            self._exitsEntry.insert(tk.END, str(i)+" ") 
        
        self._posX.insert(0, str(template._initialPosition._x))
        self._posY.insert(0, str(template._initialPosition._y))

        self._velX.insert(0, str(template._initialVelocity._magnitude))
        self._velY.insert(0, str(template._initialVelocity._angle))
        
    
    def __init__(self, master, rt, template) :
        tk.Frame.__init__(self, rt,
                          highlightbackground = "blue", highlightcolor = "blue",
                          highlightthickness=1, width = 626 * master._scale, bd = 0, height = 93)
        self._master = master
        self._template = template
        self._rt = rt

        tk.Label(self, text = "Mask With Sprite: ").grid(row = 0, column = 0)
        self._spriteMask = tk.IntVar();
        self._spriteMaskButton = tk.Checkbutton(self, text="No Mask", variable=self._spriteMask, onvalue = 1, offvalue = 0, command = self.noMaskCallback)
        self._spriteMaskButton.grid(row = 0, column = 1)
        tk.Label(self, text = "Sprite Name: ").grid(row = 0, column = 2)
        self._animationEntry = tk.Entry(self)
        self._animationEntry.grid(row = 0, column = 3)
        self._animationSelectButton = tk.Button(self, text="Choose Mask", command=self.handleAnimation)
        self._animationSelectButton.grid(row = 0, column = 4, sticky=tk.E+tk.W)

        tk.Label(self, text = "Initial Delay:").grid(row = 1, column = 0)
        self._delayEntry = tk.Entry(self)
        self._delayEntry.grid(row = 1, column = 1)

        tk.Label(self, text = "Limit Rounds: ").grid(row = 1, column = 2)
        self._roundLimit = tk.IntVar();
        self._roundLimitButton = tk.Checkbutton(self, text="No Limit", variable=self._roundLimit, onvalue = 1, offvalue = 0, command = self.noLimitCallback)
        self._roundLimitButton.grid(row=1, column=3)
        self._roundLimitEntry = tk.Entry(self)
        self._roundLimitEntry.grid(row=1, column=4)

        tk.Label(self, text = "Delay Between Rounds: ").grid(row = 2, column = 0)
        self._delayBetweenRoundsEntry = tk.Entry(self)
        self._delayBetweenRoundsEntry.grid(row=2, column=1)

        tk.Label(self, text = "Displacement: ").grid(row = 2, column = 2)
        self._displacementEntry = tk.Entry(self)
        self._displacementEntry.grid(row=2, column=3)

        tk.Label(self, text = "Spray Timing: ").grid(row = 3, column = 0)
        self._sprayTimingEntry = tk.Entry(self)
        self._sprayTimingEntry.grid(row=3, column=1, columnspan = 4,  sticky=tk.W+tk.E)

        tk.Label(self, text = "Exit Locations: ").grid(row = 4, column = 0)
        self._exitsEntry = tk.Entry(self)
        self._exitsEntry.grid(row=4, column=1, columnspan = 4,  sticky=tk.W+tk.E)
        
        tk.Label(self, text = "Initial Position(relative): ").grid(row = 5, column = 0)
        self._posX = tk.Entry(self)
        self._posX.grid(row=5, column=1)
        tk.Label(self, text = "X").grid(row = 5, column = 2)
        self._posY = tk.Entry(self)
        self._posY.grid(row=5, column=3)
        tk.Label(self, text = "Y").grid(row = 5, column = 4)

        tk.Label(self, text = "Initial Velocity: ").grid(row =6, column = 0)
        self._velX = tk.Entry(self)
        self._velX.grid(row=6, column=1)
        tk.Label(self, text = "mag").grid(row = 6, column = 2)
        self._velY = tk.Entry(self)
        self._velY.grid(row=6, column=3)
        tk.Label(self, text = "ang").grid(row = 6, column = 4)

        tk.Button(self, text = "Update", command = self.updateValues).grid(row=0, column=5, rowspan = 4, sticky=tk.W+tk.E+tk.N+tk.S)

        self.fillValues()

        
#Represents Bullet Template
class BulletTemplateFrame(tk.Frame) :
    def updateOnSlide(self, value) :
        pass
    def handleAnimation(self) :
        pop = popAnimationSelector(self._master)
        self.wait_window(pop._window)
        if pop.valid :
            self._animationEntry.delete(0, tk.END)
            self._animationEntry.insert(0, pop.value)
        else :
            messagebox.showerror("Error", "No animation selected")

    def updateValues(self) :
        pass

    def fillValues(self) :
        template = self._template
        
        if template is None :
            self._noBullet.set(1)
        else :
            self._noBullet.set(0)
            self._animationEntry.insert(0, str(template._animationName))
            self._hitboxEntry.insert(0, str(template._animationName))
            
            
            
        self.handleNoBullet()

    def setHitboxToSpriteSize(self) :
        animationName = self._animationEntry.get()
        if animationName in self._master._animations :
            animation = self._master._animations[animationName]
            self._hitboxEntry.delete(0, tk.END)
            length = min(animation.getAnimation("idle")._w, animation.getAnimation("idle")._h)
            self._hitboxEntry.insert(0, str(length))

    def handleNoBullet(self) :
        if self._noBullet.get() :
            self._animationEntry.configure(state='disable')
            self._animationSelectButton.configure(state='disable')
            self._hitboxEntry.configure(state='disable')
            self._velMag.configure(state='disable')
            self._velAng.configure(state='disable')
        else :
            self._animationEntry.configure(state='normal')
            self._animationSelectButton.configure(state='normal')
            self._hitboxEntry.configure(state='normal')
            self._velMag.configure(state='normal')
            self._velAng.configure(state='normal')
    
    def __init__(self, master, rt, template, spawner) :
        tk.Frame.__init__(self, rt,
                          highlightbackground = "blue", highlightcolor = "blue",
                          highlightthickness=1, width = 626 * master._scale, bd = 0, height = 93)
        self._master = master
        self._template = template
        self._spawner = spawner
        self._rt = rt
        
        self._noBullet = tk.IntVar();
        self._noBulletButton = tk.Checkbutton(self, text="No Bullet", variable=self._noBullet, onvalue = 1, offvalue = 0, command = self.handleNoBullet)
        self._noBulletButton.grid(row = 0, column = 0)
        
        tk.Label(self, text = "Sprite Name: ").grid(row = 0, column = 1)
        self._animationEntry = tk.Entry(self)
        self._animationEntry.grid(row = 0, column = 2)
        self._animationSelectButton = tk.Button(self, text="...", command=self.handleAnimation)
        self._animationSelectButton.grid(row = 0, column = 3)

        tk.Label(self, text = "Hitbox:").grid(row = 1, column = 0)
        self._hitboxEntry = tk.Entry(self)
        self._hitboxEntry.grid(row = 1, column = 1)

        tk.Label(self, text = "Initial Velocity:").grid(row = 2, column = 0)
        self._velMag = tk.Entry(self)
        self._velMag.grid(row = 2, column = 1)
        self._velAng = tk.Entry(self)
        self._velAng.grid(row = 3, column = 1)
        tk.Label(self, text = "Magnitude").grid(row = 2, column = 2)
        tk.Label(self, text = "Angle").grid(row = 3, column = 2)
        
        tk.Button(self, text="Hitbox to Short Side", command=self.setHitboxToSpriteSize).grid(row = 1, column = 2)
        
        tk.Button(self, text="Update", command=self.handleAnimation).grid(row = 0, column = 4, rowspan = 2, sticky=tk.N+tk.S+tk.E+tk.W)

        self.fillValues()


#what else could be on the right of the screen that describes a bullet master
#Gets called everytime a new 
class BulletMasterFrame(tk.Frame) :
    def updateOnSlide(self, value) :
        for i in self._commandFrames :
            print(type(i))
            i.updateOnSlide(value)
        
    def drawFrame(self) :
        for i in self._commandFrames :
            i.destroy()
        self._commandFrames.clear()
        if self._template is None :
            tk.Label(self, text = "No BulletMaster Selected").grid(column = 0, row = 0)
        else :
            commandFrame = BulletMasterSetupFrame(self._master, self, self._template)
            commandFrame.grid(column = 0, row = 0)
            self._commandFrames.append(commandFrame)

            j = 0

            #draw spawners and movement frames
            for spawner in self._template._bulletSpawnerTemplates :
                j = j + 1
                commandFrame = BulletSpawnerFrame(self._master, self, spawner)
                commandFrame.grid(column = 0, row = j, sticky=tk.W)
                self._commandFrames.append(commandFrame)
                for command in spawner._movementList :
                    j = j + 1
                    commandFrame = MovementCommandFrame(self._master, self, command,
                                                        spawner._movementList[command], spawner)
                    commandFrame.grid(column = 0, row = j, sticky=tk.E)
                    self._commandFrames.append(commandFrame)
                #draw bullet template for each spawner
                j = j + 1
                commandFrame = BulletTemplateFrame(self._master, self, spawner._bulletTemplate, spawner)
                commandFrame.grid(column = 0, row = j, sticky=tk.W)
                self._commandFrames.append(commandFrame)
                for command in spawner._bulletTemplate._movementList :
                    j = j + 1
                    commandFrame = MovementCommandFrame(self._master, self, command,
                                                        spawner._bulletTemplate._movementList[command], spawner._bulletTemplate)
                    commandFrame.grid(column = 0, row = j, sticky=tk.E)
                    self._commandFrames.append(commandFrame)    
    
    def __init__(self, master, template) :
        self._master = master
        self._template = template
        tk.Frame.__init__(self, master._windows["commander"],
                          highlightbackground = "green", highlightcolor = "green",
                          highlightthickness = 5, width = 626 * master._scale, bd = 0)
        
        
        self._template = template
        self._commandFrames = []
        self.drawFrame()
