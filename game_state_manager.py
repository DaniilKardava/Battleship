import numpy as np
import random


class GameStateManager:
    """
    Maintains grid state and marginal ship distributions.
    """

    def __init__(self, grid, ships, active_ships, weighting):
        """
        Parameters:
        grid: initialize the game with a 2d array
        ships: array of possible ship arrangements
        active_ships: array of actively placed ships
        weighting: a weight class instance
        """
        self.grid = grid
        self.ships = ships  # Possible ship arrangements
        self.active_ships = active_ships  # Active ships, can include repeats
        self.temp_active_ships = []

        self.weighting = weighting

        self.ship_to_id = {
            s: i for i, s in enumerate(ships)
        }  # Map ship to position in weights.
        self.id_to_ship = {i: s for s, i in self.ship_to_id.items()}
        self.squares_to_ship_ids = self.create_squares_to_ship_ids()

        self.energies = self.compute_energies()
        self.marginals = self.compute_marginals()

    def create_squares_to_ship_ids(self):
        """
        Assign squares to the ship orientations that intersect it.

        Returns:
        Square array containing lists of ships
        """
        squares_to_ship_ids = np.empty_like(self.grid, dtype=object)

        # Populate with empty arrays
        for idx in np.ndindex(squares_to_ship_ids.shape):
            squares_to_ship_ids[idx] = []

        for ship in self.ships:
            for a in np.ravel(squares_to_ship_ids[ship.rows, ship.cols]):
                a.append(self.ship_to_id[ship])
        return squares_to_ship_ids

    def compute_energies(self):
        """
        Calculate the energy of each ship given the current grid state.

        return:
        Energy array
        """
        energies = [0] * len(self.ships)
        for ship in self.ships:
            intersection = self.grid[ship.rows, ship.cols]
            energies[self.ship_to_id[ship]] = self.weighting.compute_energy(
                intersection
            )
        return energies

    def compute_marginals(self):
        """
        Calculate the unnormalized marginal probabilities of selecting a ship
        orientation.

        See theory for when compute and update marginals are appropriate.

        Returns:
        Array of unnormalized marginals.
        """
        energies = self.compute_energies()
        marginals = [0] * len(self.ships)
        for ship in self.ships:
            E = energies[self.ship_to_id[ship]]
            marginals[self.ship_to_id[ship]] = self.weighting.compute_weight(E)
        return marginals

    # @profile
    def update_marginals(self, ship, inc):
        """
        Update the unnormalized marginal probabilities for a subset of ship
        orientations intersecting the squares occupied by the 'ship' parameter.

        To avoid recomputing marginals from scratch, we require knowing what
        change occured at those squares.

        See theory for when compute and update marginals are appropriate.

        Parameters:
        ship: a ship object
        change: integer

        Returns:
        None
        """
        uniq_ids = set()
        for s in zip(ship.rows, ship.cols):
            ids = self.squares_to_ship_ids[s]
            uniq_ids.update(ids)
            for id in ids:
                self.energies[id] = self.weighting.update_energy(
                    self.energies[id], self.grid[s], inc
                )

        # Update weights once
        for id in uniq_ids:
            ship = self.id_to_ship[id]
            self.marginals[id] = self.weighting.compute_weight(self.energies[id])

    def update_grid(self, ship, inc):
        """
        Increment grid cells occupied by ship by amount inc.

        Parameters:
        ship: ship object
        inc: integer

        Returns:
        None
        """
        self.grid[ship.rows, ship.cols] += inc

    def place_ship(self, ship):
        """
        Update grid and marginals associated with new ship placement.

        Parameter:
        ship: Ship object

        Returns:
        None
        """
        self.temp_active_ships.append(ship)
        self.update_grid(ship, inc=1)
        self.update_marginals(ship, inc=1)

    def remove_ship(self, ship):
        """
        Update grid and marginals associated with new ship placement.

        Parameter:
        ship: Ship object

        Returns:
        None
        """
        self.update_grid(ship, inc=-1)
        self.update_marginals(ship, inc=-1)

    def sample_ship(self):
        """
        Sample a ship from the marginal distribution.

        Returns:
        Ship
        """

        return random.choices(self.ships, weights=self.marginals)[0]

    def update_active_ships(self):
        """
        Update the list of active ships. The temporary list is created when
        traversing the original list of active ships and resampling each
        element.

        Returns:
        None
        """

        self.active_ships = self.temp_active_ships
        self.temp_active_ships = []
