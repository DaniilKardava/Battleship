#include "main_helpers.h"
#include "Ship.h"
#include <vector>
#include <cassert>

using namespace std;

void compute_total_positions(int n, int k, vector<Ship> &ships)
{
    int id = 0;

    // Horizontal
    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n - k + 1; c++)
        {
            ships.push_back(Ship(id, r, c, Orientation::H, k));
            ++id;
        }
    }

    // Vertical
    for (int c = 0; c < n; c++)
    {
        for (int r = 0; r < n - k + 1; r++)
        {
            ships.push_back(Ship(id, r, c, Orientation::V, k));
            ++id;
        }
    }
}