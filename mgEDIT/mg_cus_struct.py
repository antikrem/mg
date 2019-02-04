from math import *
#CUS_struct copy

class CUS_Point(object) :
    def __init__(self, x, y) :
        self._x = x
        self._y = y

    def add(self, point) :
        return CUS_Point(self._x+point._x, self._y+point._y)

    def minus(self, point) :
        return CUS_Point(self._x-point._x, self._y-point._y)

    def __str__(self) :
        return str(self._x) + " " + str(self._y)

class CUS_Polar :
    def __init__(self, magnitude, angle) :
        self._magnitude = magnitude
        self._angle = angle

    def add(self, point) :
        return toPolar( toPoint(self).add(toPoint(point)))

    def __str__(self) :
        point = CUS_Point(0,0)
        point._x = self._magnitude * sin(self._angle * pi / 180.0)
        point._y = self._magnitude * cos(self._angle * pi / 180.0)
        return str(point._x) + " " + str(point._y)

#Angle is referenced from north, positive clockwise
def toPoint(polar) :
    point = CUS_Point(0,0)
    point._x = polar._magnitude * sin(polar._angle * pi / 180.0)
    point._y = polar._magnitude * cos(polar._angle * pi / 180.0)
    return point

def toPolar(point) :
    polar = CUS_Polar(0,0)
    if point._x == 0 and point._y == 0 :
        return polar
    polar._magnitude = sqrt(pow(point._x, 2) + pow(point._y, 2))
    polar._angle = atan2(point._x, point._y)* 180.0 / pi
    return polar
