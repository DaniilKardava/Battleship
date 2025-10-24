import numpy as np

# Dimension, ship length, number of ships
n = 20
k = 5
q = 50

# Grid
grid = np.zeros((n, n))

# Ship struct
from collections import namedtuple

Ship = namedtuple("Ship", ["row", "col", "orientation"])

# Arrangements
horizontal_arrangements = [Ship(r, c, "h") for r in range(n) for c in range(n - k + 1)]
vertical_arrangements = [Ship(r, c, "v") for r in range(n - k + 1) for c in range(n)]

# Map how many times a ship intersects an occupied square
ship_intersection_counter = {
    s: 0 for s in horizontal_arrangements + vertical_arrangements
}
valid_placements = list(ship_intersection_counter.keys())


# Map which arrangements intersect which squares
grid_ship_map = np.empty((n, n), dtype=object)
grid_ship_map[:] = [[[] for _ in range(n)] for _ in range(n)]

for ship, _ in ship_intersection_counter.items():
    if ship.orientation == "h":
        for i in range(k):
            grid_ship_map[ship.row, ship.col + i].append(ship)
    else:
        for i in range(k):
            grid_ship_map[ship.row + i, ship.col].append(ship)

# Update the ship dictionary and the valid placements list
def update_valid_placements_and_ship_dictionary(ship, insert):
    """
    Call this function after inserting or removing a ship.
    """
    increment = 1 * insert + -1 * (not insert)
    if ship.orientation == "h":
        for i in range(k):
            for s in grid_ship_map[ship.row, ship.col + i]:
                if insert and ship_intersection_counter[s] == 0:
                    valid_placements.remove(s)
                elif not insert and ship_intersection_counter[s] == 1:
                    valid_placements.append(s)
                ship_intersection_counter[s] += increment
    else:
        for i in range(k):
            for s in grid_ship_map[ship.row + i, ship.col]:
                if insert and ship_intersection_counter[s] == 0:
                    valid_placements.remove(s)
                elif not insert and ship_intersection_counter[s] == 1:
                    valid_placements.append(s)
                ship_intersection_counter[s] += increment


# Active Ships
active_ships = []

# Create an initial configuration of ships
count = 0
for r in range(n):
    if count >= q:
        break
    for c in range(0, n - k + 1, k):
        active_ships.append(Ship(r, c, "h"))

        # Increment counter of all ships that intersect this region
        update_valid_placements_and_ship_dictionary(Ship(r, c, "h"), insert=True)

        grid[r, c : c + k] = 1
        count += 1
        if count >= q:
            break

# Check if ship collides in grid
def check_collision(ship):
    """
    Accept a tuple (r,c,b) indicating the coordinate of the upper left block of the ship
    and its orientation.
    """
    if ship.orientation == "v":
        return np.any(grid[ship.row : ship.row + k, ship.col])
    else:
        return np.any(grid[ship.row, ship.col : ship.col + k])


# Update grid with new ship
def update_grid(ship, b):
    """
    Provide a ship and a binary to indicate whether to fill the region with 1 or 0.
    """
    if ship.orientation == "v":
        grid[ship.row : ship.row + k, ship.col] = b
    else:
        grid[ship.row, ship.col : ship.col + k] = b


# Run Gibbs sampler
import time

samples = []
iterations = 1000

begin_sampling = time.time()

for i in range(iterations):

    if i % 100 == 0:
        print(f"Sampled: {i}")

    for j, ship in enumerate(active_ships):
        # Remove the ship from the grid
        update_grid(ship, 0)
        update_valid_placements_and_ship_dictionary(ship, insert=False)

        # Sample a new ship
        m = np.random.randint(0, len(valid_placements))
        new_ship = valid_placements[m]
        update_valid_placements_and_ship_dictionary(new_ship, insert=True)
        active_ships[j] = new_ship
        update_grid(new_ship, 1)

    samples.append(grid.copy())

end_sampling = time.time()

print(f"Sampling took: {end_sampling - begin_sampling:.0f}s")

# Process samples
samples = np.array(samples)

burn_in = 100

begin_computing_average = time.time()
grid_averages = np.sum(samples[burn_in:], 0) / (iterations - burn_in)
end_computing_average = time.time()

print(f"Computing average took {end_computing_average - begin_computing_average:.2f}s")

# Plot table and heat map
from matplotlib import pyplot as plt

plt.imshow(grid_averages, cmap="viridis")
plt.colorbar(label="Value")

plt.show()

# Create an animation of the samples
from matplotlib.animation import FuncAnimation

# --- Setup figure and first frame ---
fig, ax = plt.subplots()
im = ax.imshow(samples[0], cmap="Greys", vmin=0, vmax=1)
title = ax.set_title(f"Frame: {0}")
plt.colorbar(im, ax=ax)

# --- Update function for animation ---
def update(frame_idx):
    im.set_array(samples[frame_idx])
    title.set_text(f"Frame {frame_idx}")


# --- Create animation ---
anim = FuncAnimation(fig, update, frames=len(samples), interval=50, blit=False)

plt.show()
