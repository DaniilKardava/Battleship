import numpy as np

# Dimension, ship length, number of ships
n = 20
k = 5
q = 50
beta = 1e10

# Grid
grid = np.zeros((n, n))

# Ship struct
from collections import namedtuple

Ship = namedtuple("Ship", ["row", "col", "orientation"])

# Arrangements
horizontal_arrangements = [Ship(r, c, "h") for r in range(n) for c in range(n - k + 1)]
vertical_arrangements = [Ship(r, c, "v") for r in range(n - k + 1) for c in range(n)]
total_arangements = horizontal_arrangements + vertical_arrangements
ship_id = {s: i for i, s in enumerate(total_arangements)}

# Map how many times a ship intersects an occupied square
from weight_methods import hard_uniform, boltzman
from functools import partial

weight_args = []
weighting = partial(
    hard_uniform, *weight_args
)  # Specify the weighting mechanism for sampling.

intersections = [0] * len(total_arangements)
weights = [weighting(0)] * len(total_arangements)

# Map which arrangements intersect which squares
grid_ship_map = np.empty((n, n), dtype=object)
grid_ship_map[:] = [[[] for _ in range(n)] for _ in range(n)]

for ship in total_arangements:
    if ship.orientation == "h":
        for i in range(k):
            grid_ship_map[ship.row, ship.col + i].append(ship_id[ship])
    else:
        for i in range(k):
            grid_ship_map[ship.row + i, ship.col].append(ship_id[ship])


# Active Ships
active_ships = []

# Create an initial configuration of ships
from helper_methods import update_valid_placements_and_ship_dictionary

count = 0
for r in range(n):
    if count >= q:
        break
    for c in range(0, n - k + 1, k):
        ship = Ship(r, c, "h")
        active_ships.append(ship)

        # Increment counter of all ships that intersect this region
        update_valid_placements_and_ship_dictionary(
            grid_ship_map,
            intersections,
            weights,
            weighting,
            k,
            ship,
            insert=True,
        )

        grid[r, c : c + k] = 1
        count += 1
        if count >= q:
            break

# Run Gibbs sampler
from helper_methods import update_grid

import time, random

samples = []
iterations = 3000

begin_sampling = time.time()

for i in range(iterations):

    if i % 100 == 0:
        print(f"Sampled: {i}")

    for j, ship in enumerate(active_ships):
        # Remove the ship from the grid
        update_grid(grid, k, ship, 0)
        update_valid_placements_and_ship_dictionary(
            grid_ship_map,
            intersections,
            weights,
            weighting,
            k,
            ship,
            insert=False,
        )

        # Sample a new ship
        new_ship = random.choices(total_arangements, weights=weights)[0]

        update_valid_placements_and_ship_dictionary(
            grid_ship_map,
            intersections,
            weights,
            weighting,
            k,
            new_ship,
            insert=True,
        )
        active_ships[j] = new_ship
        update_grid(grid, k, new_ship, 1)

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
