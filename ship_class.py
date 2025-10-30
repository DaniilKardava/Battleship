from dataclasses import dataclass


class Ship:
    """ """

    def __init__(self, row, col, orientation, length):
        self.corner_row = row
        self.corner_col = col
        self.orientation = orientation
        self.length = length
        self.rows, self.cols = self.compute_coordinates()

    def compute_coordinates(self):
        """
        Returns the coordinates occupied by the ship.

        Return:
        Array containing the
        """
        if self.orientation == "h":
            return [
                self.corner_row,
                range(self.corner_col, self.corner_col + self.length),
            ]
        else:
            return [
                range(self.corner_row, self.corner_row + self.length),
                self.corner_col,
            ]
