import numpy as np

# Experiment parameters
n = 10  # Grid dimension
k = 1  # Ship length
q = 10  # Number of ships
beta = 5  # Penalty

grid = np.zeros((n, n))

# Valid ship arrangements
from ship_class import Ship

horizontal_ships = [Ship(r, c, "h", k) for r in range(n) for c in range(n - k + 1)]
vertical_ships = [Ship(r, c, "v", k) for r in range(n - k + 1) for c in range(n)]
total_arangements = horizontal_ships + vertical_ships

from game_state_manager import GameStateManager
import weight_methods

weighting = weight_methods.OverflowBoltzmann(beta)

game_state = GameStateManager(
    grid=grid,
    ships=total_arangements,
    active_ships=[],
    weighting=weighting,
)


# Begin with a dense hard lattice packing of the ships.
count = 0
for r in range(n):
    if count >= q:
        break
    for c in range(0, n - k + 1, k):
        ship = Ship(r, c, "h", k)
        game_state.place_ship(ship)
        count += 1
        if count >= q:
            break

game_state.update_active_ships()

# Run Gibbs sampler
import time
import cProfile, pstats

iterations = 10000
samples = []

begin_sampling = time.time()
"""pr = cProfile.Profile()
pr.enable()"""


for i in range(iterations):

    if i % 100 == 0:
        print(f"Sampled: {i}")

    for ship in game_state.active_ships:
        game_state.remove_ship(ship)
        new_ship = game_state.sample_ship()
        game_state.place_ship(new_ship)

    game_state.update_active_ships()

    samples.append(game_state.grid.copy())

end_sampling = time.time()

"""pr.disable()
stats = pstats.Stats(pr).sort_stats("tottime")
stats.print_stats(15)  # show top 15 entries"""

print(f"Sampling took: {end_sampling - begin_sampling:.0f}s")

import sys

# Analyze
burn_in = 100
samples = np.array(samples)

# Compute sample energies
square_energies = weighting.compute_square_energy(samples)
grid_energies = np.sum(square_energies, axis=(1, 2))

avgs = [e / (i + 1) for i, e in enumerate(np.cumsum(grid_energies))]
from matplotlib import pyplot as plt

plt.plot(
    avgs,
)
plt.show()

sys.exit()


grid_averages = np.sum(samples[burn_in:], 0) / (iterations - burn_in)

from visualize import make_heatmap, animate_samples

make_heatmap(grid_averages)
animate_samples(samples)
