class Ref(object):
    _value = False

    def __init__(self, value) :
        self._value = value

    def set(self, value) :
        self._value = value

    def get(self) :
        return self._value
    
