# Update the ship dictionary and the valid placements list
def update_valid_placements_and_ship_dictionary(
    grid_ship_map, ship_intersection_counter, weighting, k, ship, insert
):
    """
    Call this function after inserting or removing a ship to update the number
    of times that a ship configuration collides with the current
    board. Update which ship configurations have 0 collisions.
    """
    increment = 1 * insert + -1 * (not insert)
    if ship.orientation == "h":
        for i in range(k):
            for s in grid_ship_map[ship.row, ship.col + i]:
                ship_intersection_counter[s][0] += increment
                ship_intersection_counter[s][1] = weighting(
                    ship_intersection_counter[s][0]
                )
    else:
        for i in range(k):
            for s in grid_ship_map[ship.row + i, ship.col]:
                ship_intersection_counter[s][0] += increment
                ship_intersection_counter[s][1] = weighting(
                    ship_intersection_counter[s][0]
                )


# Update grid with new ship
def update_grid(grid, k, ship, b):
    """
    Provide grid, a ship and a binary to indicate whether to fill the region with 1 or 0.
    """
    if ship.orientation == "v":
        grid[ship.row : ship.row + k, ship.col] = b
    else:
        grid[ship.row, ship.col : ship.col + k] = b
