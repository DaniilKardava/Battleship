import numpy as np
from game_state_manager import GameStateManager
import weight_methods
from ship_class import Ship


def main(n, k, q, beta, iterations, burn_in, total_arrangements, verbose):
    """
    Parameters:
    n: integer grid dimension
    k: integer ship length
    q: number of ships
    num_samples: integer number of samples
    burn_in: number of samples to drop from analysis
    total_arrangements: valid ship arrangements
    verbose: boolean
    """
    grid = np.zeros((n, n))

    weighting = weight_methods.OverflowBoltzmann(beta)

    game_state = GameStateManager(
        grid=grid,
        ships=total_arrangements,
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
    samples = []

    for i in range(iterations):

        if verbose and i % 100 == 0:
            print(i)

        for ship in game_state.active_ships:
            game_state.remove_ship(ship)
            new_ship = game_state.sample_ship()
            game_state.place_ship(new_ship)

        game_state.update_active_ships()

        samples.append(game_state.grid.copy())

    # Analyze
    samples = np.array(samples[burn_in:])

    # Compute sample energies
    square_energies = weighting.compute_square_energy(samples)
    grid_energies = np.sum(square_energies, axis=(1, 2))

    return np.mean(grid_energies)


if __name__ == "__main__":

    n = 10
    k = 1
    q = 10
    beta = 0
    iterations = 1000
    burn_in = 100

    horizontal_ships = [Ship(r, c, "h", k) for r in range(n) for c in range(n - k + 1)]
    vertical_ships = [Ship(r, c, "v", k) for r in range(n - k + 1) for c in range(n)]
    total_arrangements = horizontal_ships + vertical_ships

    main(n, k, q, beta, iterations, burn_in, total_arrangements, True)
