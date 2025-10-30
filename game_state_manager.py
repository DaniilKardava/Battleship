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
        weighting: a function for computing a ship's marginal weight
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

    def compute_marginals(self):
        """
        Calculate the unnormalized marginal probabilities of selecting a ship
        orientation.

        See theory for when compute and update marginals are appropriate.

        Returns:
        Array of unnormalized marginals.
        """
        marginals = [0] * len(self.ships)
        for ship in self.ships:
            intersection = self.grid[ship.rows, ship.cols]
            marginals[self.ship_to_id[ship]] = self.weighting(intersection)
        return marginals

    # @profile
    def update_marginals(self, ship):
        """
        Update the unnormalized marginal probabilities for a subset of ship
        orientations intersecting the squares occupied by the 'ship' parameter.

        See theory for when compute and update marginals are appropriate.

        Parameters:
        ship: a ship object

        Returns:
        None
        """
        intersection = self.squares_to_ship_ids[
            ship.rows, ship.cols
        ]  # Slice of array occupied by ship
        ids = set().union(*intersection)
        for id in ids:
            ship = self.id_to_ship[id]
            self.marginals[id] = self.weighting(self.grid[ship.rows, ship.cols])

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
        self.update_marginals(ship)

    def remove_ship(self, ship):
        """
        Update grid and marginals associated with new ship placement.

        Parameter:
        ship: Ship object

        Returns:
        None
        """
        self.update_grid(ship, inc=-1)
        self.update_marginals(ship)

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
