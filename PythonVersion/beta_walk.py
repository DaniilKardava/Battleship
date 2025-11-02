import numpy as np
from main import main

# Experiment parameters
n = 10  # Grid dimension
k = 1  # Ship length
q = 10  # Number of ships

# Valid ship arrangements
from ship_class import Ship

horizontal_ships = [Ship(r, c, "h", k) for r in range(n) for c in range(n - k + 1)]
vertical_ships = [Ship(r, c, "v", k) for r in range(n - k + 1) for c in range(n)]
total_arrangements = horizontal_ships + vertical_ships

betas = np.arange(10, 11, 0.01)

energies = []
for beta in betas:

    print(beta)

    iterations = 10000
    burn_in = 100
    energies.append(main(n, k, q, beta, iterations, burn_in, total_arrangements, False))

from matplotlib import pyplot as plt

plt.plot(betas, energies)
plt.show()

# Numerically integrate
integral = 0.1 * sum(energies)
print(integral)
