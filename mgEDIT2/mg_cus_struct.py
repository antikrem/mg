from math import atan2

class CUS_Point :
    _x = 0.0
    _y = 0.0

    def __init__(self, x, y) :
        self._x = x
        self._y = y

    def add(self, point) :
        return CUS_Point(self._x+point._x, self._y+point._y)

    def getAngleToPoint(self, point) :
        return (atan2(point._y-self._y, point._x-self._x) * (180 /3.14159265359))

class CUS_Polar :
    def __init__(self, magnitude, angle) :
        self._magnitude = magnitude
        self._angle = angle

    def add(self, point) :
        return toPolar( toPoint(self).add(toPoint(point)))

#Angle is referenced from north, positive clockwise
def toPoint(polar) :
    point = CUS_Point
    point._x = polar._magnitude * sin(polar._angle * pi / 180.0)
    point._y = polar._magnitude * cos(polar._angle * pi / 180.0)
    return point

def toPolar(point) :
    polar = CUS_Polar
    polar._magnitude = sqrt(pow(point._x, 2) + pow(point._y, 2))
    polar._angle = atan2(point._x, point._y)* 180.0 / pi
    return polar
