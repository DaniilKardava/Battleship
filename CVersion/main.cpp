#include "Simulate.h"
#include "WeightMethods.h"
#include <iostream>

using namespace std;

int main()
{

    WeightingTemplate *weighting = new HardLattice();

    int n;
    int k;
    int q;
    int iterations;

    cout << "Grid Dimension:";
    cin >> n;

    cout << "Ship Length:";
    cin >> k;

    cout << "Ship number:";
    cin >> q;

    cout << "Iterations:";
    cin >> iterations;

    bool verbose = true;

    // Compute positions
    vector<Ship> positions;

    // Horizontal
    for (int r = 0; r < n; r++)
    {
        for (int c = 0; c < n - k + 1; c++)
        {
            positions.push_back(Ship(r, c, Orientation::H, k));
        }
    }
    // Vertical
    for (int c = 0; c < n; c++)
    {
        for (int r = 0; r < n - k + 1; r++)
        {
            positions.push_back(Ship(r, c, Orientation::V, k));
        }
    }

    vector<int> samples = simulate(n, k, q, iterations, weighting, verbose, positions);

    return 0;
}