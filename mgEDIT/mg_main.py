import tkinter as tk
from mg_cus_struct import *
from mg_popup import *
from mg_methods import *
from mg_animation import *

#Main class, stores level state
class main :
    def initialise(self) :
        self._lastClickPosition = CUS_Point(0,0)
        self._campaign = None
        self._level = None
        self._playerPosition = CUS_Point(0,0)

        #Animation set reference
        self._animations = dict()
        self._currentEnemies = []
        #reference to frame canvas elements, for easy clear from clearCanvas()
        self._frameCanvasElements = []

        self._currentCycle = 0
        self._maxCycles = 10

        self._root = None
        self._pathToMaster = "..//mg//"
        self._windows = dict()
        self._scale = 1.0
        self._gameScale = 1.0
        
        with open('settings.ini') as settings :
            lines = settings.readlines()
        for i in lines :
            if i.split()[0] == "resolution_width" :
                resoX = i.split()[1]
                print(resoX)
            elif i.split()[0] == "resolution_height" :
                resoY = i.split()[1]
                print(resoY)
        if resoX == 0 or resoY == 0:
            messagebox.showerror("Error", "Resolution not defined in settings.ini. Resolution set to 1920 by 1080")
        self._scale = int(resoY) / 1080.0
        self._gameScale = self._scale*0.7111
        self._root = tk.Tk()
        self._root.title("Level View")
        self._root.resizable(width = False, height = False)
        self._root.geometry("%dx%d+0+0" % (1222 * self._scale, 910 * self._scale))

        #create the main view
        self._windows["level_view"] = tk.Canvas(self._root, bg = 'grey', height = 910 * self._scale, width = 1222 * self._scale)
        self._windows["level_view"].marker = self._windows["level_view"].create_oval(0, 0, 2, 2, width = 0, fill = 'white')
        self._windows["level_view"].create_line(100 * self._gameScale, 100 * self._gameScale,
                          100 * self._gameScale, 1180 * self._gameScale,
                          1620 * self._gameScale, 1180 * self._gameScale,
                          1620 * self._gameScale, 100 * self._gameScale,
                          100 * self._gameScale, 100 * self._gameScale)
        self._windows["level_view"].markerLabel = self._windows["level_view"].create_text((self._lastClickPosition._x + 100) * self._gameScale,
                                                                                          (self._lastClickPosition._y + 100) * self._gameScale,
                                                                                          text = "{0,0)", anchor = tk.SW)
        self._windows["level_view"].pack()
        
        #create a window for slider
        self._windows["slider_bar"] = tk.Toplevel(self._root)
        self._windows["slider_bar"].title("Cycle Controller")
        self._windows["slider_bar"].geometry("%dx%d+0+%d" % (1910 * self._scale, 50 * self._scale, 912 * self._scale + 30))
        self._windows["slider_bar"].slider = tk.Scale(self._windows["slider_bar"], from_=0, to=self._maxCycles, orient=tk.HORIZONTAL, length = self._scale * 1750, resolution=(0.00333333333), command=self.sliderUpdateHandle)
        self._windows["slider_bar"].slider.pack(side = tk.LEFT)
        #Buttons on the slider window
        self._windows["slider_bar"].SliderPlayButtonFrame = tk.Button(self._windows["slider_bar"], text = "Play Level", command = None, height = (int)(self._scale * 60), width = (int)(self._scale * 100))
        self._windows["slider_bar"].SliderPlayButtonFrame.pack(side = tk.RIGHT)

        #level load window
        self._windows["level_load"] = tk.Toplevel(self._root)
        self._windows["level_load"].title("Enemy List")
        self._windows["level_load"].geometry("%dx%d+%d+0" % (686 * self._scale, 250 * self._scale, 1222 * self._scale))
        self._windows["level_load"].columnconfigure(0, weight=20)
        self._windows["level_load"].columnconfigure(1, weight=1)
        self._windows["level_load"].columnconfigure(2, weight=1)
        self._windows["level_load"].columnconfigure(3, weight=1)
        #List of enemies
        self._windows["level_load"].listBox = tk.Listbox(self._windows["level_load"])
        self._windows["level_load"].listBox.grid( column = 0, row = 1, rowspan = 3, sticky = tk.N+tk.E+tk.W+tk.S)

        self._windows["level_load"].loadButton = tk.Button(self._windows["level_load"], text = "Load Level", command=self.loadHandle)
        self._windows["level_load"].loadButton.grid( column = 1, row = 1, sticky = tk.N+tk.E+tk.W+tk.S)
        self._windows["level_load"].saveButton = tk.Button(self._windows["level_load"], text = "Save Level")
        self._windows["level_load"].saveButton.grid( column = 2, row = 1, sticky = tk.N+tk.E+tk.W+tk.S)
        self._windows["level_load"].newButton = tk.Button(self._windows["level_load"], text = "New Level")
        self._windows["level_load"].newButton.grid( column = 3, row = 1, sticky = tk.N+tk.E+tk.W+tk.S)
        self._windows["level_load"].spawnEnemyButton = tk.Button(self._windows["level_load"], text = "Spawn at \n Position", command=self.spawnHandle)
        self._windows["level_load"].spawnEnemyButton.grid( column = 1, row = 2, sticky = tk.N+tk.E+tk.W+tk.S)
        #Information frame on level load that stores information
        self._windows["level_load"].infoFrame = tk.Frame(self._windows["level_load"], height = 83 * self._scale, bg = "blue")  
        self._windows["level_load"].infoFrame.grid(column = 1, row = 3, columnspan = 3, sticky=tk.N+tk.E+tk.S+tk.W)
        #info frame variables
        self._windows["level_load"].infoFrame.infoPositionValueLabel = tk.Label(self._windows["level_load"].infoFrame, text = "Position: { NULL, NULL }")
        self._windows["level_load"].infoFrame.infoPositionValueLabel.grid(column = 0, row = 1)
        self._windows["level_load"].infoFrame.infoCampaignValueLabel = tk.Label(self._windows["level_load"].infoFrame, text = "Campaign: NULL")
        self._windows["level_load"].infoFrame.infoCampaignValueLabel.grid(column = 0, row = 0)
        self._windows["level_load"].infoFrame.infoLevelValueLabel = tk.Label(self._windows["level_load"].infoFrame, text = "Level: NULL")
        self._windows["level_load"].infoFrame.infoLevelValueLabel.grid(column = 1, row = 0)
        
    def clickHandler(self, event) :
        self._lastClickPosition._x = event.x / self._gameScale - 100
        self._lastClickPosition._y = event.y / self._gameScale - 100
        self._windows["level_view"].coords(self._windows["level_view"].marker, event.x-1, event.y-1, event.x +1, event.y + 1)
        self._windows["level_load"].infoFrame.infoPositionValueLabel["text"] = "Position: { %d, %d }" % (self._lastClickPosition._x ,self._lastClickPosition._y)
        self._windows["level_view"].itemconfig(self._windows["level_view"].markerLabel, text = ( "{%d, %d)" % (self._lastClickPosition._x, self._lastClickPosition._y) ))
        self._windows["level_view"].coords(self._windows["level_view"].markerLabel,(self._lastClickPosition._x + 100)*self._gameScale + 3, (self._lastClickPosition._y + 100)*self._gameScale - 2)
        self._windows["level_view"].tag_raise(self._windows["level_view"].markerLabel)
        self._windows["level_view"].tag_raise(self._windows["level_view"].marker)
        
    def loadHandle(self) :
        pop = PopLevel(self)
        self._root.wait_window(pop._window)
        path = self._pathToMaster + "//campaigns//" + self._campaign + "//" + str(self._level) + "//enemy_table.txt"
        self._currentEnemies = makeListOfEnemiesFromFile(path)
        for ent in self._currentEnemies :
            ent.calculatePositions(ent._deathCycle, self._playerPosition)
        self._animations.update(loadAnimations(self._pathToMaster + "campaigns\\" + self._campaign + "\\" + str(self._level) + "\\", "local_load.txt", ImageLevel.LOCAL, self))
        self.updateCampaignLevelLabel()
        self.enemyListUpdate()
        print("done loading")

    def drawCanvasForFrame(self) :
        currentCycle = self._currentCycle
        for i in self._frameCanvasElements :
            self._windows["level_view"].delete(i)
        self._frameCanvasElements.clear()
        for ent in self._currentEnemies :
            if ent._spawningCycle <= currentCycle and (ent._deathCycle + ent._spawningCycle) > currentCycle :
                if ent._animationName in self._animations :
                    x = ent.pullPositionAtCycle(currentCycle)._x
                    y = ent.pullPositionAtCycle(currentCycle)._y
                    frameList = self._animations[ent._animationName].getAnimation("idle")._drawFrames
                    image = frameList[(self._currentCycle-ent._spawningCycle)%len(frameList)]
                    toPush = self._windows["level_view"].create_image( (x + 100)*self._gameScale, (y + 100)*self._gameScale, image = image)
                    self._frameCanvasElements.append(toPush)

    def spawnHandle(self) :
        pop = PopEnemy(self)
        self._root.wait_window(pop._window)
        if pop.valid :
            enemy = pop.value
            print(self._maxCycles - enemy._spawningCycle)
            enemy.calculatePositions(self._maxCycles *300 + 10 - enemy._spawningCycle, self._playerPosition)
            self._currentEnemies.append(enemy)
            self.enemyListUpdate()
        else :
            messagebox.showerror("Error", "Invalid Enemy, no changes made")

    def sliderUpdateHandle(self, throw) :
        self._currentCycle = int(300* self._windows["slider_bar"].slider.get())
        self.drawCanvasForFrame()
        print(self._currentCycle)
        
    def updateCampaignLevelLabel(self) :
        if self._campaign is not None :
            self._windows["level_load"].infoFrame.infoCampaignValueLabel['text'] ="Campaign: %s" % (self._campaign)
        if self._level is not None :
            self._windows["level_load"].infoFrame.infoLevelValueLabel['text'] ="Level: %d" % (self._level)
    def enemyListUpdate(self) :
        self._currentEnemies.sort(key=enemySort)
        self._windows["level_load"].listBox.delete(0, tk.END)
        counter = 0
        for enemy in self._currentEnemies :
            counter += 1
            deadFail = ""
            if not enemy._deathInitialised :
                deadFail = " (No Death Cycle)"
            self._windows["level_load"].listBox.insert(tk.END, str(counter) + " @ " + str(enemy._spawningCycle/300) + " w\ " + enemy._animationName + deadFail)
        
    def bindEvents(self) :
        self._windows["level_view"].bind("<Button-1>", self.clickHandler)
        self._windows["level_view"].bind("<B1-Motion>", self.clickHandler)
        
    def mainloop(self) :
        self._animations.update(loadAnimations(self._pathToMaster + "assets\\", "load_list.txt", ImageLevel.GLOBAL, self))
        cont = True
        while (cont) :
            pop = PopCampaign(self)
            self._root.wait_window(pop._window)
            cont = not pop.valid
        self.updateCampaignLevelLabel()
        self._root.mainloop()
        
if __name__ == "__main__":
    m = main()
    m.initialise()
    m.bindEvents()
    m.mainloop()
    print("nice")
    
