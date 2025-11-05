#include "Simulate.h"
#include "WeightMethods.h"
#include "GameStateManager.h"
#include <iostream>
#include <chrono>

using namespace std;

vector<int> simulate(int n, int k, int q, int iterations, WeightingTemplate *weighting, bool verbose, vector<Ship> positions)
{
    vector<int> grid(n * n);

    vector<Ship> active_ships;

    GameStateManager manager = GameStateManager(grid, positions, active_ships, weighting);

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

    vector<int> samples(n * n * iterations);
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

        copy(manager.grid.begin(), manager.grid.end(), samples.begin() + insertion_idx);
        insertion_idx += manager.grid.size();
    }

    auto end = chrono::system_clock::now();
    chrono::duration<double> diff = end - start;

    cout << diff.count();

    return samples;
}