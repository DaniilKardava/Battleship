#include "Simulate.h"
#include "WeightMethods.h"
#include "GameStateManager.h"
#include <iostream>

using namespace Eigen;
using namespace std;

Tensor<int, 3> simulate(int n, int k, int q, int iterations, WeightingTemplate *weighting, bool verbose, vector<Ship> positions)
{
    Tensor<int, 2> grid(n, n);
    grid.setConstant(0);

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

    Tensor<int, 3> samples(n, n, iterations);

    for (int i = 0; i < iterations; i++)
    {

        if (verbose && i % 100 == 0)
        {
            cout << i << endl;
        }

        for (Ship ship : manager.active_ships)
        {
            manager.remove_ship(ship);
            Ship new_ship = manager.sample_ship();
            manager.place_ship(new_ship);
        }
        manager.update_active_ships();

        samples.chip(i, 2) = manager.grid;
    }

    return samples;
}