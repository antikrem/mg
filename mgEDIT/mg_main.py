from mg_ent_classes import *
from os import path
from mg_ref import *
from mg_popup import *
from mg_methods import *
from mg_image import *
from tkinter import messagebox
import tkinter as tk
import threading

def levelFilePath(campaign, level) :
    return ("..\\mg\\campaigns\\" + campaign + "\\" + str(level) + "\\enemy_table.txt")

def main() :
    #Click Variables
    lastClickPosition = CUS_Point(0,0)
    
    #level variables
    campaign = Ref(None)
    level = Ref(None)
    animations = dict()

    #current list of enemies
    currentEnemies = Ref([])
    
    #the current cycle being edited
    currentCycle = Ref(0)
    maxCycle = Ref(currentCycle.get() + 10)

    #variables regarding resolutions and scaling values
    resoX = 0
    resoY = 0
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
    scale = int(resoY) / 1080.0
    gameScale = scale*0.7111
    
    ##shows representaion of current level at given time
    root = tk.Tk()
    root.title("Level View")
    root.resizable(width=False, height=False)
    root.geometry("%dx%d+0+0" % (1222 * scale, 910 * scale))
    
    #main
    levelView = tk.Canvas(root, bg = 'grey', height = 910 * scale, width =1222 * scale)
    pointer = levelView.create_oval(0, 0, 2, 2,
                           width = 0, fill = 'white')
    
    levelView.create_line(100 * gameScale, 100 * gameScale,
                          100 * gameScale, 1180 * gameScale,
                          1620 * gameScale, 1180 * gameScale,
                          1620 * gameScale, 100 * gameScale,
                          100 * gameScale, 100 * gameScale)
    positionLabelOnView = levelView.create_text((lastClickPosition._x + 100) *gameScale, (lastClickPosition._y + 100) *gameScale, text = "{0,0)", anchor = tk.SW)
    levelView.pack()

    ##Seperate window for containing slider
    sliderRoot = tk.Toplevel(root)
    sliderRoot.title("Cycle Controller")
    sliderRoot.geometry("%dx%d+0+%d" % (1910 * scale, 60 * scale, 910 * scale + 30))
    slider = tk.Scale(sliderRoot, from_=0, to=10, orient=tk.HORIZONTAL, length = scale * 1750)
    slider.pack(side = tk.LEFT)
    #Buttons on the slider window
    SliderPlayButtonFrame = tk.Frame(sliderRoot, width = 80 * scale, height = 60)
    SliderPlayButtonFrame.pack(side = tk.RIGHT)
    
    def zeroCounterCallBack() :
        slider.set(0)
        print(unHandledClick.get())
        print(lastClickPosition._x)
    
    sliderZeroButton = tk.Button(SliderPlayButtonFrame, text = "Play Level", command = zeroCounterCallBack, height = 100, width = 200)
    sliderZeroButton.pack()
    
    ##seperate enemy load window
    levelLoadRoot = tk.Toplevel(root)
    levelLoadRoot.title("Enemy List")
    levelLoadRoot.geometry("%dx%d+%d+0" % (686 * scale, 250 * scale, 1222 * scale))
    levelLoadRoot.columnconfigure(0, weight=20)
    levelLoadRoot.columnconfigure(1, weight=1)
    levelLoadRoot.columnconfigure(2, weight=1)
    levelLoadRoot.columnconfigure(3, weight=1)

    #Enemy List load
    listBox = tk.Listbox(levelLoadRoot)
    listBox.grid( column = 0, row = 1, rowspan = 3, sticky = tk.N+tk.E+tk.W+tk.S)
    
    loadButton = tk.Button(levelLoadRoot, text = "Load Level", command=None)
    loadButton.grid( column = 1, row = 1, sticky = tk.N+tk.E+tk.W+tk.S)
    saveButton = tk.Button(levelLoadRoot, text = "Save Level")
    saveButton.grid( column = 2, row = 1, sticky = tk.N+tk.E+tk.W+tk.S)
    newButton = tk.Button(levelLoadRoot, text = "New Level")
    newButton.grid( column = 3, row = 1, sticky = tk.N+tk.E+tk.W+tk.S)
    spawnEnemyButton = tk.Button(levelLoadRoot, text = "Spawn at \n Position")
    spawnEnemyButton.grid( column = 1, row = 2, sticky = tk.N+tk.E+tk.W+tk.S)

    infoFrame = tk.Frame(levelLoadRoot, height = 83 * scale, bg = "blue")  
    infoFrame.grid(column = 1, row = 3, columnspan = 3, sticky=tk.N+tk.E+tk.S+tk.W)
    
    infoPositionValueLabel = tk.Label(infoFrame, text = "Position: { NULL, NULL }")
    infoPositionValueLabel.grid(column = 0, row = 1)
    infoCampaignValueLabel = tk.Label(infoFrame, text = "Campaign: NULL")
    infoCampaignValueLabel.grid(column = 0, row = 0)
    infoLevelValueLabel = tk.Label(infoFrame, text = "Level: NULL")
    infoLevelValueLabel.grid(column = 1, row = 0)
    def disableEverything() :
        root.grab_set()
        sliderRoot.grab_set()
        levelLoadRoot.grab_set()

    def enableEverything() :
        root.grab_release()
        sliderRoot.grab_release()
        levelLoadRoot.grab_release()

    def updateListBox(currentEnemies, listBox) :
        listBox.delete(0, tk.END)
        i = 0;
        for enemy in currentEnemies.get() :
            i+=1
            listBox.insert(tk.END, str(i) + " @" + str(enemy._spawningCycle))
        

    def updateCampaignLevelLabel():
        infoCampaignValueLabel['text'] ="Campaign: %s" % (campaign.get())
        infoLevelValueLabel['text'] ="Level: %s" % (level.get())

    #Clear local animations
    def clearLocalAnimations(animations) :
        newAnimations = dict()
        for i in animations :
            if not animations[i]._local :
                newAnimations[i] = animations[i]
        return newAnimations

    def clickHandler(event):
        lastClickPosition._x = event.x / gameScale - 100
        lastClickPosition._y = event.y / gameScale - 100
        levelView.coords(pointer, event.x-1, event.y-1, event.x +1, event.y + 1)
        infoPositionValueLabel['text'] = "Position: { %d, %d }" % (lastClickPosition._x ,lastClickPosition._y)
        levelView.itemconfig(positionLabelOnView, text = ( "{%d, %d)" % (lastClickPosition._x, lastClickPosition._y) ))
        levelView.coords(positionLabelOnView, (lastClickPosition._x + 100)*gameScale + 3, (lastClickPosition._y + 100)*gameScale - 2)

    #todo, feed object reference
    def loadHandle(currentEnemies, listBox, level, animations, campaign):                
        pop = PopLevel(root, campaign, level)
        root.wait_window(pop._window)
        if pop.valid :
            level.set(int(pop.value))
            del currentEnemies.get()[:]
            currentEnemies.set(makeListOfEnemiesFromFile(levelFilePath(campaign.get(), level.get())))
            updateListBox(currentEnemies, listBox)
            updateCampaignLevelLabel()
            for enemy in currentEnemies.get() :
                enemy.populatePosition()
            updateCampaignLevelLabel()

        else :
            messagebox.showinfo("Error", "Invalid input")

    def spawnHandler(currentEnemies, listBox, currentCycle) :
        if level.get() == None :
            messagebox.showinfo("Error", "No level loaded, No changes made")
            return
        popVel = PopEnemy(root, lastClickPosition, animations, currentCycle)
        root.wait_window(popVel._window)
        if popVel.valid == "True" :
            currentEnemies.get().append(popVel.value)
            updateListBox(currentEnemies, listBox)
        elif popVel.valid == "False" :
            messagebox.showinfo("Error", "Invalid parameters for an enemy, No changes made")
            
    
    levelView.bind("<Button-1>", clickHandler)
    levelView.bind("<B1-Motion>", clickHandler)

    updateCampaignLevelLabel()
    loadButton.configure(command=lambda: loadHandle(currentEnemies, listBox, level, animations, campaign))
    spawnEnemyButton.configure(command=lambda: spawnHandler(currentEnemies, listBox, currentCycle))

    animations = loadAnimations("load_list.txt", False, False)

    invalid = True
    #Get current campaign
    while (invalid) :
        pop = PopCampaign(root, campaign)
        root.wait_window(pop._window)
        invalid = not pop.valid            

    updateCampaignLevelLabel()
    
    root.mainloop()
    print("Clean end")
    print(len(currentEnemies.get()))
    return animations

#Entry Point
#if __name__ == "__main__":
#    currentEnemies = main()
#    for i in currentEnemies[0]._positions :
#        print(str(i._x) + " " + str(i._y))
