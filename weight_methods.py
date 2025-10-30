import math
import numpy as np


# @profile
def hard_lattice(intersection):
    """
    Weighting associated with assigning equal probability to all zero intersection states.
    A ship has weight 1 or 0 depending on whether it can be placed on the current board.

    Receive a view of grid squares occupied by the ship for which the update is
    being made.

    Parameters:
    intersection: view of grid squares

    Returns:
    integer: unnormalized weight
    """
    return int(all(x == 0 for x in intersection))


def binary_boltzman(intersection):
    """
    Weighting associated with max(0, min(1, n-1)) energy per square. The ship
    contributes 1 unit of energy for each square where occupancy was previously
    1.

    Receive a view of grid squares occupied by the ship for which the update is
    being made.

    Parameters:
    intersection: view of grid squares

    Returns:
    float: unnormalized weight
    """

    return sum(x == 1 for x in intersection)


def pairwise_boltzman(beta, intersections):
    """
    Weighting associated with n choose 2 energy per square with n occupants. The
    ship contributes n units of energy for each square with n occupants.

    Receive a view of grid squares occupied by the ship for which the update is
    being made.

    Beta is proportional to the strength of the large energy penalty.

    Parameters:
    beta: float
    intersection: view of grid squares

    Returns:
    float: unnormalized weight
    """
    collisions = sum(intersections)
    return math.exp(-beta * collisions)


def overflow_boltzman(beta, intersections):
    """
    Weighting associated with max(0, n-1) energy per square with n occupants. The
    ship contributes 1 unit of energy for each occupied square.

    Receive a view of grid squares occupied by the ship for which the update is
    being made.

    Beta is proportional to the strength of the large energy penalty.

    Parameters:
    beta: float
    intersection: view of grid squares

    Returns:
    float: unnormalized weight
    """
    collisions = sum(x > 0 for x in intersections)
    return math.exp(-beta * collisions)
