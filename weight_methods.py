"""
This file contains weighting methods for sampling battleships in the Gibbs
sampler. Design choice is to pass collisions last to allow for binding earlier
positional arguments using partial functools method in main.
"""
import math


def hard_uniform(collisions):
    """
    Args: Number of collisions.
    Gives weight 1 or 0 if the number of collisions is 0 or greater than 0 respectively.
    """

    return collisions == 0


def boltzman(beta, collisions):
    """
    Args: Number of collisions, inverse temperature
    """

    return math.exp(-beta * collisions)
