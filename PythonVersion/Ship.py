import numpy as np
from enum import Enum 
from collections import namedtuple

class Orientation(Enum):
        H = 1
        V = 2

Ship = namedtuple("Ship", ["r", "c", "o", "length", "rows", "cols"])

def rows(o,r, length):
    if o is Orientation.H:
        # horizontal: row fixed = r, cols vary
        return tuple(np.full(length, r))
    else:
        # vertical: rows vary starting at r
        return tuple(np.arange(r, r + length))

def cols(o,c, length):
    if o is Orientation.V:
        # horizontal: row fixed = r, cols vary
        return tuple(np.full(length, c))
    else:
        # vertical: rows vary starting at r
        return tuple(np.arange(c, c + length))

