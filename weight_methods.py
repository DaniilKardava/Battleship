"""
This file contains weighting methods for sampling battleships in the Gibbs sampler.
"""
import math


def hard_uniform(collisions):
    """
    Args: Number of collisions.
    Gives weight 1 or 0 if the number of collisions is 0 or greater than 0 respectively.
    """

    return collisions == 0


def boltzman(collisions, beta):
    """
    Args: Number of collisions, inverse temperature
    """

    return math.exp(-beta * collisions)
