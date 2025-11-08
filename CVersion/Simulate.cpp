#include "Simulate.h"
#include "WeightMethods.h"
#include "GameStateManager.h"
#include <iostream>
#include <chrono>
#include <tuple>

using namespace std;

tuple<vector<int>, vector<int>> simulate(int n, int k, int q, int iterations, WeightingTemplate *weighting, bool verbose, vector<Ship> positions, bool record_samples)
{
    vector<Ship> active_ships;

    GameStateManager manager = GameStateManager(n, positions, weighting);

    int count = 0;
    for (int r = 0; r < n; r++)
    {
        if (count >= q)
        {
            break;
        }
        for (int c = 0; c < n - k + 1; c += k)
        {
            Ship ship = Ship(r, c, Orientation::H, k);
            manager.place_ship(ship);
            count++;
            if (count >= q)
            {
                break;
            }
        }
    }
    manager.update_active_ships();

    vector<int> energies(iterations);
    vector<int> samples;

    if (record_samples)
    {
        samples.resize(n * n * iterations);
    }

    int insertion_idx = 0;

    auto start = chrono::system_clock::now();

    for (int i = 0; i < iterations; i++)
    {
        if (verbose && i % 100 == 0)
        {
            cout << i << endl;
        }

        for (Ship &ship : manager.active_ships)
        {
            manager.remove_ship(ship);
            Ship new_ship = manager.sample_ship();
            manager.place_ship(new_ship);
        }
        manager.update_active_ships();

        // Compute energy
        int E = 0;
        for (int i = 0; i < manager.grid.size(); i++)
        {
            E += weighting->compute_square_energy(manager.grid[i]);
        }
        energies[i] = E;

        if (record_samples)
        {
            copy(manager.grid.begin(), manager.grid.end(), samples.begin() + insertion_idx);
            insertion_idx += n * n;
        }
    }

    auto end = chrono::system_clock::now();
    chrono::duration<double> diff = end - start;

    if (verbose)
    {
        cout << diff.count() << endl;
    }

    return tuple(energies, samples);
}