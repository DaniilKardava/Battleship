#include "GameStateManager.h"
#include "WeightMethods.h"
#include <iostream>
#include <fstream>
#include <tuple>
#include "main_helpers.h"

using namespace std;

int main()
{

    int n;
    int k;
    int q;
    int iterations;
    bool verbose;
    bool record_samples;

    cout << "Grid Dimension:";
    cin >> n;

    cout << "Ship Length:";
    cin >> k;

    cout << "Ship number:";
    cin >> q;

    cout << "Iterations:";
    cin >> iterations;

    cout << "Verbose:";
    cin >> verbose;

    cout << "Record Samples:";
    cin >> record_samples;

    float beta_min;
    float beta_max;
    float inc;

    cout << "Beta min:";
    cin >> beta_min;

    cout << "Beta max:";
    cin >> beta_max;

    cout << "Beta inc:";
    cin >> inc;

    int period;
    cout << "Print period:";
    cin >> period;

    WeightingTemplate *weighting = new OverflowBoltzmann(0);

    // Compute positions
    vector<Ship> ships;
    ships.reserve(2 * n * (n - k + 1));
    compute_total_positions(n, k, ships);
    const vector<Ship> &frozen_ships = ships; // Don't move ships but allow mutation of content.

    // Record data
    vector<int> energies((static_cast<int>(beta_max / inc) - static_cast<int>(beta_min / inc)) * iterations);
    vector<int> samples;
    if (record_samples)
    {
        samples.resize(energies.size() * n * n); // A grid associated with each energy value.
    }
    int sample_insertion_idx = 0;

    float beta = 0;
    GameStateManager manager = GameStateManager(n, k, q, frozen_ships, weighting);
    int counter = 0;
    for (int b = static_cast<int>(beta_min / inc); b < static_cast<int>(beta_max / inc); ++b)
    {
        beta = b * inc;
        if ((b % period * static_cast<int>(1 / inc)) == 0)
        {
            cout << beta << endl;
        }

        // Run simulation
        weighting->beta = beta;
        manager.reset();

        // Run simulation
        for (int i = 0; i < iterations; i++)
        {
            if (verbose && i % 100 == 0)
            {
                cout << i << endl;
            }

            for (const Ship *ship : manager.active_ships)
            {
                manager.remove_ship(*ship);
                int id = manager.sample_shipid();
                manager.place_ship(ships[id]);
            }
            manager.update_active_ships();

            // Compute energy
            int E = 0;
            for (int j = 0; j < manager.grid.size(); ++j)
            {
                E += weighting->compute_square_energy(manager.grid[j]);
            }
            energies[counter * iterations + i] = E;

            if (record_samples)
            {
                copy(manager.grid.begin(), manager.grid.end(), samples.begin() + sample_insertion_idx);
                sample_insertion_idx += n * n;
            }
        }

        ++counter;
    }

    ofstream F("energies.txt");
    for (float E : energies)
    {
        F << E << ",";
    }
    F.close();

    if (record_samples)
    {
        F.open("samples.txt");
        for (int s : samples)
        {
            F << s << ",";
        }
        F.close();
    }

    return 0;
}