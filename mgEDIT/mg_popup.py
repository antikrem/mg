import tkinter as tk
from PIL import Image, ImageTk
import os
from tkinter import messagebox
from math import *
from mg_cus_struct import *

class PopUp(object) :
    def __init__(self, root, textIn):
        self._window = tk.Toplevel(root)
        self._window.attributes('-topmost', 'true')
        self.__text = tk.Label(self._window, text = textIn)
        self.__entry = tk.Entry(self._window)
        self.__button = tk.Button( self._window, text = 'Ok', command = self.cleanup)
        self.__text.pack()
        self.__entry.pack()
        self.__button.pack()
        self.valid = False
        self._window.lift(aboveThis=root)

    def cleanup(self):
        try:
            self.value=int(self.__entry.get())
        except ValueError:
            self.valid = False
            self._window.destroy()
        else :
            self.valid = True
            self._window.destroy()

class PopCampaign(object) :
    def __init__(self, master):
        self._window = tk.Toplevel(master._root)
        self._window.title("Choose Campaign")
        self._window.attributes('-topmost', 'true')
        self._root = master._root
        self._master = master
        self._campaign = master._campaign
        tk.Label(self._window, text = "Choose campaign").pack()
    
        self._listbox = tk.Listbox(self._window, exportselection=0)
        self._listbox.pack()
        self.__button = tk.Button( self._window, text = 'Load Existing', command = self.loadCampaign)
        self.__button.pack()

        tk.Label(self._window, text = "Or make new campaign").pack()
        
        self.__entry = tk.Entry(self._window)
        self.__entry.pack()
        self.__new = tk.Button( self._window, text = 'Generate', command = self.generate)
        self.__new.pack()

        self.valid = False
        
        for name in os.listdir(master._pathToMaster + "campaigns") :
            self._listbox.insert(tk.END, name)
        self._window.lift(aboveThis=self._root)
    
    def loadCampaign(self) :
        try :
            self._master._campaign = (self._listbox.get(self._listbox.curselection()))
            self.valid = True
            self._window.destroy()
            
        except tk.TclError:
             messagebox.showerror("Error", "No selection made")
             self._window.lift(aboveThis=self._root)

    def generate(self) :
        try :
            baseDir = master._pathToMaster + "campaigns//" + str(self.__entry.get())
            print(str(self.__entry.get()))
            if baseDir ==  master._pathToMaster + "campaigns//" :
                raise TypeError
            os.makedirs(baseDir)
            f = open(baseDir + "//campaign_load.txt","w+")
            f.close()
            
            self.valid = True
            self._window.destroy()
            
        except TypeError:
             messagebox.showerror("Error", "Invalid name for file")
             self._window.lift(aboveThis=self._root)

class PopLevel(object) :
    def __init__(self, master):
        self._window = tk.Toplevel(master._root)
        self._window.title("Choose Level")
        self._window.attributes('-topmost', 'true')
        self._master = master
        self._root = master._root
        tk.Label(self._window, text = "Choose level").pack()
    
        self._listbox = tk.Listbox(self._window)
        self._listbox.pack()
        self.__button = tk.Button( self._window, text = 'Load level from ' + master._campaign , command = self.loadLevel)
        self.__button.pack()

        tk.Label(self._window, text = 'Otherwise use "New Level"').pack()

        self.valid = False
        
        for name in os.listdir("..//mg//campaigns//"+ master._campaign) :
            if name != "campaign_load.txt" :
                self._listbox.insert(tk.END, name)
        self._window.lift(aboveThis=self._root)
    
    def loadLevel(self) :
        try :
            self._master._level = int(self._listbox.get(self._listbox.curselection()))
            self.valid = True
            self._window.destroy()
            
        except tk.TclError:
             messagebox.showerror("Error", "Invalid value, no level loaded")
             self._window.lift(aboveThis=self._root)

MIN_DAMAGE_PER_SECOND = 100
MAX_DAMAGE_PER_SECOND = 300

class PopEnemy(object) :
    def switchVel(obj) :
        try:
            if not obj._pointVel :
                a = float(obj.__velX.get())
                b = float(obj.__velY.get())
                obj.__velX.delete(0, tk.END)
                obj.__velY.delete(0, tk.END)
                obj.__velX.insert(0,sqrt(pow(a, 2) + pow(b, 2)))
                obj.__velY.insert(0,atan2(a, b)* 180.0 / pi)
                obj._velLabelX['text'] = "Mag"
                obj._velLabelY['text'] = "Ang"
                obj._toPolBut['text'] = "To Point"
            else :
                mag = float(obj.__velX.get())
                ang = float(obj.__velY.get())
                obj.__velX.delete(0, tk.END)
                obj.__velY.delete(0, tk.END)
                obj.__velX.insert(0, mag * sin(ang * pi / 180.0))
                obj.__velY.insert(0, mag * cos(ang * pi / 180.0))
                obj._velLabelX['text'] = "X"
                obj._velLabelY['text'] = "Y"
                obj._toPolBut['text'] = "To Polar"
            obj._pointVel = not obj._pointVel
        except ValueError:
            obj.__velX.delete(0, tk.END)
            obj.__velY.delete(0, tk.END)
            obj.__velX.insert(0,0)
            obj.__velY.insert(0,0)

    def insertFromView(obj) :
        obj.__posX.delete(0, tk.END)
        obj.__posY.delete(0, tk.END)
        obj.__posX.insert(0, obj._startPosView._x)
        obj.__posY.insert(0, obj._startPosView._y)

    def updateTKK(self, throw) :
        try :
            minTTK = int(self._hitpointString.get()) / MIN_DAMAGE_PER_SECOND
            maxTTK = int(self._hitpointString.get()) / MAX_DAMAGE_PER_SECOND
            print(minTTK)
            print(maxTTK)
        except ValueError :
            print("caught")
    
    def __init__(self, master):
        self._root = master._root
        self._master = master
        self._window = tk.Toplevel(self._root)
        self._window.attributes('-topmost', 'true')
        self._window.title("Create Enemy")
        self._window.attributes('-topmost', 'true')
        self._startPosView = master._lastClickPosition
        self._pointVel = False
        
        (tk.Label(self._window, text = "Create enemy")).grid(row=0, columnspan = 3)
        (tk.Label(self._window, text = "Starting frame: ")).grid(row=1, column=0)
        (tk.Label(self._window, text = "Hit Points: ")).grid(row=2, column=0)
        (tk.Label(self._window, text = "Hit Box: ")).grid(row=3, column=0)
        (tk.Label(self._window, text = "Animation: ")).grid(row=4, column=0)
        (tk.Label(self._window, text = "Starting Position: ")).grid(row=5, column=0)
        (tk.Button(self._window, text="From View", command=self.insertFromView)).grid(row=6, column=0)
        (tk.Label(self._window, text = "Starting Velocity: ")).grid(row=7, column=0)
        self._toPolBut = tk.Button(self._window, text="To Point", command=self.switchVel)
        self._toPolBut.grid(row=8, column=0)

        self.secondsSpawnLabel = tk.Label(self._window, text = " seconds")
        self.secondsSpawnLabel.grid(row=1, column = 2)
        self._startEntry = tk.Entry(self._window)
        self._startEntry.grid(row=1, column=1)

        cmd = self._root.register(self.updateTKK)
        self._hitpointString = tk.IntVar()
        self._hitpointString.set(0)
        self.ttkLabel = tk.Label(self._window, text = "0 : 0")
        self.ttkLabel.grid(row=2, column = 2)
        self._hitpointEntry = tk.Entry(self._window, textvariable=self._hitpointString, validate="key", validatecommand=(cmd,'%P'))
        self._hitpointEntry.grid(row=2, column=1)

        self._hitboxEntry = tk.Entry(self._window)
        self._hitboxEntry.grid(row=3, column=1)
        self.aniBox = tk.Label(self._window, text = "0 Max")
        self.aniBox.grid(row=3, column = 2)

        self._animationEntry = tk.Entry(self._window)
        self._animationEntry.grid(row=4, column=1)

        self._startEntry.delete(0, tk.END)
        self._startEntry.insert(0, (float(master._currentCycle)/300))

        self.__posX = tk.Entry(self._window)
        self.__posX.insert(0, self._startPosView._x)
        self.__posX.grid(row=5, column=1)
        (tk.Label(self._window, text = "X")).grid(row=5, column=2)
        self.__posY = tk.Entry(self._window)
        self.__posY.insert(0, self._startPosView._y)
        self.__posY.grid(row=6, column=1)
        (tk.Label(self._window, text = "Y")).grid(row=6, column=2)

        self.__velX = tk.Entry(self._window)
        self.__velX.grid(row=7, column=1)
        self.__velX.insert(0, "0.0")
        self._velLabelX = tk.Label(self._window, text = "Mag")
        self._velLabelX.grid(row=7, column=2)
        self.__velY = tk.Entry(self._window)
        self.__velY.grid(row=8, column=1)
        self.__velY.insert(0, "0.0")
        self._velLabelY = tk.Label(self._window, text = "Ang")
        self._velLabelY.grid(row=8, column=2)
                           
        tk.Button(self._window, text="...", command=self.handleAnimation).grid(row=4, column=2)
        self.valid = False

        tk.Button(self._window, text="Spawn Enemy", command=self.cleanup).grid(row=9, column=0)

        self._window.lift(aboveThis=self._root)

    def handleAnimation(self) :
        pop = popAnimationSelector(self._master)
        self._root.wait_window(pop._window)
        if pop.valid :
            self._animationEntry.delete(0, tk.END)
            self._animationEntry.insert(0, pop.value)
            radius = self._master._animations[pop.value].getAnimation("idle")._radius
            self.aniBox["text"] = str(radius) + " Max"
            print(radius)
        else :
            messagebox.showerror("Error", "No animation selected")
        self._window.lift(aboveThis=self._root)
    def cleanup(self):
        import mg_ent_classes as mg
        
        if self._pointVel :
            try:
                velocity = toPolar(mg.CUS_Point(float(self.__velX.get()), float(self.__velY.get())))
            except ValueError:
                messagebox.showerror("Error", "Invalid Velocity: must be a decimal or whole number")
                self.valid = False
                return
        else :
            try:
                velocity = mg.CUS_Polar(float(self.__velX.get()), float(self.__velY.get()))
            except ValueError:
                messagebox.showerror("Error", "Invalid Velocity: must be a decimal or whole number")
                self.valid = False
                return

        try:
            int(300*float(self._startEntry.get()))
        except ValueError:
            messagebox.showerror("Error", "Invalid Starting Frame: must be a decimal or whole number")
            self.valid = False
            return

        if not (self._animationEntry.get() in self._master._animations) :
            print(self._animationEntry.get())
            print(self._master._currentEnemies)
            messagebox.showerror("Error", "Invalid Animation Set Name, use the Animation Set Selector (...)")
            self.valid = False
            return

        try:
            mg.CUS_Point(float(self.__posX.get()), float(self.__posY.get()))
        except ValueError:
            messagebox.showerror("Error", "Invalid position")
            self.valid = False
            return

        try:
            float(self._hitboxEntry.get())
            if float(self._hitboxEntry.get()) <= 0 :
                messagebox.showerror("Error", "Invalid Hit Box, must be a positive number")
                return
        except ValueError:
            messagebox.showerror("Error", "Invalid Hit Points")
            self.valid = False
            return   
        
        try:
            int(self._hitpointString.get())
        except ValueError:
            messagebox.showerror("Error", "Invalid Hit Points")
            self.valid = False
            return        
        
        self.value = mg.EnemyEntity(int(300*float(self._startEntry.get())),
                                        self._animationEntry.get(),
                                        mg.CUS_Point(float(self.__posX.get()), float(self.__posY.get())),
                                        velocity,
                                        float(self._hitboxEntry.get()),
                                        int(self._hitpointString.get()))
        self.valid = True
        self._window.destroy()

class popAnimationSelector(object) :
    def __init__(self, master):
        self._window = tk.Toplevel(master._root)
        self._window.title("Select Animation")
        self._window.attributes('-topmost', 'true')
        self._root = master._root
        self._master = master
        self._animations = master._animations
        tk.Label(self._window, text = "Animation Selector").grid(row = 0, columnspan = 3)
    
        self._listbox = tk.Listbox(self._window, height = 31)
        self._listbox.grid(row = 1, column = 0)
        self.__button = tk.Button( self._window, text = 'Chose Animation' , command = self.choose)
        self.__button.grid(row = 2, column = 0)
        self._canvas = tk.Canvas(self._window, width=500, height=500, background='black')
        self._canvas.grid(row = 1, column = 1, columnspan = 2)

        self.valid = False
        
        for i in self._animations :
            self._listbox.insert(tk.END, i)
        self._window.lift(aboveThis=master._root)

        background = Image.open("background.png")
        background.load()
        self._backgroundPhoto = ImageTk.PhotoImage(background)
        self._canvas.create_image(0, 0, image = self._backgroundPhoto, anchor=tk.NW)
        
        self._preview = self._canvas.create_image(250, 250, image = None)
        self._listbox.bind('<<ListboxSelect>>', self.updateCanvas)

    def updateCanvas(self, event) :
        name = self._listbox.get(self._listbox.curselection())
        self._canvas.itemconfig(self._preview, image = self._animations[name].getAnimation("idle")._frames[0])
        
    def choose(self) :
        try :
            self.value = str(self._listbox.get(self._listbox.curselection()))
            self.valid = True
        except tk.TclError:
            self.value = None
            self.valid = False
        self._window.destroy()
