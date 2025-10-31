import math
from abc import ABC, abstractmethod
import numpy as np
from scipy.special import comb


class WeightingTemplate(ABC):
    """
    Blueprint for weighting mechanisms.
    """

    @abstractmethod
    def compute_weight(self, E):
        """
        Compute the marginal ship weight from an integer summary.

        Parameters:
        E: integer

        Returns:
        unnormalized marginal weight
        """
        pass

    @abstractmethod
    def compute_energy(self, intersection):
        """
        Compute the ship's energy given its intersection with the current board.

        Parameters:
        intersection: view of grid squares

        Returns:
        energy
        """
        pass

    @abstractmethod
    def update_energy(self, E, S, D):
        """
        Update the energy given a change at a single square.

        Parameters:
        E: current energy
        S: current square value
        D: change in square value

        return:
        energy
        """
        # Later logic relies on D not being 0. I shouldn't call update if there is no update.
        pass

    @abstractmethod
    def compute_square_energy(self, arr):
        """
        Performs elementwise the energy contribution of a np array.

        Parameters:
        arr: np.array
        """
        pass


class HardLattice(WeightingTemplate):
    """
    Weighting associated with assigning equal probability to all zero
    intersection states. A ship has weight 1 or 0 depending on whether it can be
    placed on the current board.
    """

    def compute_weight(self, E):
        return int(E == 0)

    def compute_energy(self, intersection):
        return sum(intersection)

    def update_energy(self, E, S, D):
        return E + D

    def compute_square_energy(self, arr):
        return np.zeros_like(arr)


class BinaryBoltzmann(WeightingTemplate):
    """
    Weighting associated with max(0, min(1, n-1)) energy per square. The ship
    contributes 1 unit of energy for each square where occupancy was 1 before
    placement of this ship.
    """

    def __init__(self, beta):
        self.beta = beta

    def compute_weight(self, E):
        return math.exp(-self.beta * E)

    def compute_energy(self, intersection):
        return sum(x == 1 for x in intersection)

    def update_energy(self, E, S, D):
        if S == 1:
            return E + 1
        elif S - D == 1:
            return E - 1
        else:
            return E

    def compute_square_energy(self, arr):
        return np.maximum(0, np.minimum(1, arr - 1))


class PairwiseBoltzmann(WeightingTemplate):
    """
    Weighting associated with n choose 2 energy per square with n occupants. The
    ship contributes n units of energy for each square with n occupants.
    """

    def __init__(self, beta):
        self.beta = beta

    def compute_weight(self, E):
        return math.exp(-self.beta * E)

    def compute_energy(self, intersection):
        return sum(intersection)

    def update_energy(self, E, S, D):
        return E + D

    def compute_square_energy(self, arr):
        return comb(arr, 2)


class OverflowBoltzmann(WeightingTemplate):
    """
    Weighting associated with max(0, n-1) energy per square with n occupants. The
    ship contributes 1 units of energy for each square with 1 or more occupants.
    """

    def __init__(self, beta):
        self.beta = beta

    def compute_weight(self, E):
        return math.exp(-self.beta * E)

    def compute_energy(self, intersection):
        return sum([x > 0 for x in intersection])

    def update_energy(self, E, S, D):
        if S == 0:
            return E - 1
        elif S == 1 and D == 1:
            return E + 1
        else:
            return E

    def compute_square_energy(self, arr):
        return np.maximum(0, arr - 1)
