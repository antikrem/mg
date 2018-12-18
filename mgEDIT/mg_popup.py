import tkinter as tk
from PIL import Image, ImageTk
import os
from tkinter import messagebox
from math import *
import mg_cus_struct


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
    def __init__(self, root, campaign):
        self._window = tk.Toplevel(root)
        self._window.title("Choose Campaign")
        self._window.attributes('-topmost', 'true')
        self._root = root
        self._campaign = campaign
        tk.Label(self._window, text = "Choose campaign").pack()
    
        self._listbox = tk.Listbox(self._window)
        self._listbox.pack()
        self.__button = tk.Button( self._window, text = 'Load Existing', command = self.loadCampaign)
        self.__button.pack()

        tk.Label(self._window, text = "Or make new campaign").pack()
        
        self.__entry = tk.Entry(self._window)
        self.__entry.pack()
        self.__new = tk.Button( self._window, text = 'Generate', command = self.generate)
        self.__new.pack()

        self.valid = False
        
        for name in os.listdir("..//mg//campaigns") :
            self._listbox.insert(tk.END, name)
        self._window.lift(aboveThis=root)
    
    def loadCampaign(self) :
        try :
            self._campaign.set(self._listbox.get(self._listbox.curselection()))
            self.valid = True
            self._window.destroy()
            
        except tk.TclError:
             messagebox.showerror("Error", "No selection made")
             self._window.lift(aboveThis=self._root)

    def generate(self) :
        try :
            baseDir = "..//mg//campaigns//" + str(self.__entry.get())
            print(str(self.__entry.get()))
            if baseDir ==  "..//mg//campaigns//" :
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
    def __init__(self, root, campaign, level):
        self._window = tk.Toplevel(root)
        self._window.title("Choose Level")
        self._window.attributes('-topmost', 'true')
        self._root = root
        self._level = level
        tk.Label(self._window, text = "Choose level").pack()
    
        self._listbox = tk.Listbox(self._window)
        self._listbox.pack()
        self.__button = tk.Button( self._window, text = 'Load level from ' + campaign.get() , command = self.loadLevel)
        self.__button.pack()

        tk.Label(self._window, text = 'Otherwise use "New Level"').pack()

        self.valid = False
        
        for name in os.listdir("..//mg//campaigns//"+ campaign.get()) :
            if name != "campaign_load.txt" :
                self._listbox.insert(tk.END, name)
        self._window.lift(aboveThis=root)
    
    def loadLevel(self) :
        try :
            self.value=int(self._listbox.get(self._listbox.curselection()))
            self.valid = True
            self._window.destroy()
            
        except tk.TclError:
             messagebox.showerror("Error", "Invalid value, no level loaded")
             self._window.lift(aboveThis=self._root)

class PopEnemy(object) :
    def switchVel(obj) :
        try:
            if obj._pointVel :
                a = float(obj.__velX.get())
                b = float(obj.__velY.get())
                obj.__velX.delete(0, tk.END)
                obj.__velY.delete(0, tk.END)
                obj.__velX.insert(0,sqrt(pow(a, 2) + pow(b, 2)))
                obj.__velY.insert(0,atan2(a, b)* 180.0 / pi)
                obj._velLabelX['text'] = "mag"
                obj._velLabelY['text'] = "ang"
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
    
    def __init__(self, root, startPoint, animations, currentCycle):
        self._root = root
        self._window = tk.Toplevel(root)
        self._window.title("Create Enemy")
        self._window.attributes('-topmost', 'true')
        self._animations = animations
        self._startPosView = startPoint
        self._pointVel = None
        
        (tk.Label(self._window, text = "Create enemy")).grid(row=0, columnspan = 3)
        (tk.Label(self._window, text = "Starting frame: ")).grid(row=1, column=0)
        (tk.Label(self._window, text = "Animation: ")).grid(row=2, column=0)
        (tk.Label(self._window, text = "Starting Position: ")).grid(row=3, column=0)
        (tk.Button(self._window, text="From View", command=self.insertFromView)).grid(row=4, column=0)
        (tk.Label(self._window, text = "Starting Velocity: ")).grid(row=5, column=0)
        self._toPolBut = tk.Button(self._window, text="To Polar", command=self.switchVel)
        self._toPolBut.grid(row=6, column=0)

        self.secondsSpawnLabel = tk.Label(self._window, text = " seconds")
        self.secondsSpawnLabel.grid(row=1, column = 2)
        self._startEntry = tk.Entry(self._window)
        self._startEntry.grid(row=1, column=1)

        self._animationEntry = tk.Entry(self._window)
        self._animationEntry.grid(row=2, column=1)

        self._startEntry.delete(0, tk.END)
        self._startEntry.insert(0, currentCycle.get())
        self._startEntry["text"] = "%d Seconds" % (float(currentCycle.get())/300)

        self.__posX = tk.Entry(self._window)
        self.__posX.insert(0, startPoint._x)
        self.__posX.grid(row=3, column=1)
        (tk.Label(self._window, text = "X")).grid(row=3, column=2)
        self.__posY = tk.Entry(self._window)
        self.__posY.insert(0, startPoint._y)
        self.__posY.grid(row=4, column=1)
        (tk.Label(self._window, text = "Y")).grid(row=4, column=2)

        self.__velX = tk.Entry(self._window)
        self.__velX.grid(row=5, column=1)
        self.__velX.insert(0, "0")
        self._velLabelX = tk.Label(self._window, text = "X")
        self._velLabelX.grid(row=5, column=2)
        self.__velY = tk.Entry(self._window)
        self.__velY.grid(row=6, column=1)
        self.__velY.insert(0, "0")
        self._velLabelY = tk.Label(self._window, text = "Y")
        self._velLabelY.grid(row=6, column=2)
                           
        tk.Button(self._window, text="...", command=self.handleAnimation).grid(row=2, column=2)
        self.valid = False

        tk.Button(self._window, text="", command=self.cleanup).grid(row=7, column=0)
        self.valid = False

        self._window.lift(aboveThis=root)

    def handleAnimation(self) :
        pop = popAnimationSelector(self._root, self._animations)
        self._root.wait_window(pop._window)
        if pop.valid :
            self._animationEntry.delete(0, tk.END)
            self._animationEntry.insert(0, pop.value)
        else :
            messagebox.showerror("Error", "No animation selected")
        self._window.lift(aboveThis=self._root)
    def cleanup(self):
        import mg_ent_classes as mg
        try:
            if self._pointVel :
                velocity = mg.CUS_Point(float(self.__velX.get()), float(self.__velY.get()))
                velocity = mg.CUS_Point(float(self.__velX.get()), float(self.__velY.get()))
            else :
                velocity = mg.CUS_Polar(float(self.__velX.get()), float(self.__velY.get()))
                velocity = toPoint(velocity)
            self.value = mg.EnemyEntity(self._startEntry.get(),
                                     self._animationEntry.get(),
                                     mg.CUS_Point(float(self.__posX.get()), float(self.__posX.get())),
                                     velocity)
                                     
        except ValueError:
            print("caught")
            self.valid = "False"
            self._window.destroy()
        else :
            self.valid = "True"
            self._window.destroy()

class popAnimationSelector(object) :
    def __init__(self, root, animations):
        self._window = tk.Toplevel(root)
        self._window.title("Select Animation")
        self._window.attributes('-topmost', 'true')
        self._root = root
        self._animations = animations
        tk.Label(self._window, text = "Animation Selector").grid(row = 0, columnspan = 3)
    
        self._listbox = tk.Listbox(self._window, height = 31)
        self._listbox.grid(row = 1, column = 0)
        self.__button = tk.Button( self._window, text = 'Chose Animation' , command = self.choose)
        self.__button.grid(row = 2, column = 0)
        self._canvas = tk.Canvas(self._window, width=500, height=500, background='black')
        self._canvas.grid(row = 1, column = 1, columnspan = 2)

        self.valid = False
        
        for i in animations :
            self._listbox.insert(tk.END, i)
        self._window.lift(aboveThis=root)

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
