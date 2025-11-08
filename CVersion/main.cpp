#include "Simulate.h"
#include "WeightMethods.h"
#include <iostream>
#include <fstream>
#include <tuple>

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

    WeightingTemplate *weighting = new BinaryBoltzmann(0);

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

    float beta = 0;
    vector<int> energies((static_cast<int>(beta_max / inc) - static_cast<int>(beta_min / inc)) * iterations);
    vector<int> samples;

    if (record_samples)
    {
        samples.resize(energies.size() * n * n); // A grid associated with each energy value.
    }
    int sample_offset = 0;
    int energy_offset = 0;

    for (int i = static_cast<int>(beta_min / inc); i < static_cast<int>(beta_max / inc); ++i)
    {
        beta = i * inc;
        cout << beta << endl;
        weighting->beta = beta;
        auto t = simulate(n, k, q, iterations, weighting, verbose, positions, record_samples);

        copy(get<0>(t).begin(), get<0>(t).end(), energies.begin() + energy_offset);
        energy_offset += iterations;

        if (record_samples)
        {
            copy(get<1>(t).begin(), get<1>(t).end(), samples.begin() + sample_offset);
            sample_offset += n * n * iterations;
        }
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