import numpy as np
from main import main

# Experiment parameters
n = 10  # Grid dimension
k = 1  # Ship length
q = 10  # Number of ships

# Valid ship arrangements
from Ship import Ship, Orientation, rows, cols

horizontal_ships = [Ship(r, c, Orientation.H, k, rows(Orientation.H, r, k), cols(Orientation.H, c, k)) for r in range(n) for c in range(n - k + 1)]
vertical_ships = [Ship(r, c, Orientation.V, k, rows(Orientation.V, r, k), cols(Orientation.V, c, k)) for r in range(n - k + 1) for c in range(n)]
total_arrangements = horizontal_ships + vertical_ships


betas = np.arange(0, 1, 0.01)

energies = []
for beta in betas:

    print(beta)

    iterations = 100
    burn_in = 0
    energies.append(main(n, k, q, beta, iterations, burn_in, total_arrangements, False))

from matplotlib import pyplot as plt

plt.plot(betas, energies)
plt.show()

# Numerically integrate
integral = 0.1 * sum(energies)
print(integral)
