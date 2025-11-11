#include "WeightMethods.h"
#include <cmath>

using namespace std;

// HardLattice Implementation
float HardLattice::compute_weight(int E) const
{
    if (E == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int HardLattice::compute_energy(vector<int> &intersection) const
{
    int E = 0;
    for (int i = 0; i < intersection.size(); i++)
    {
        E += intersection[i];
    }
    return E;
}

int HardLattice::compute_square_energy(int s) const
{
    return 0;
}

int HardLattice::update_energy(int E, int S, int D) const
{
    return E + D;
}

int HardLattice::max_energy(int k, int q) const
{
    return 1;
}

// BinaryBoltzmann implementation
BinaryBoltzmann::BinaryBoltzmann(float beta)
{
    this->beta = beta;
}

float BinaryBoltzmann::compute_weight(int E) const
{
    return exp(-beta * E);
}

int BinaryBoltzmann::compute_energy(vector<int> &intersection) const
{
    int E = 0;
    for (int i = 0; i < intersection.size(); i++)
    {
        if (intersection[i] == 1)
        {
            E++;
        }
    }
    return E;
}

int BinaryBoltzmann::compute_square_energy(int s) const
{
    return max(0, min(1, s - 1));
}

int BinaryBoltzmann::update_energy(int E, int S, int D) const
{
    if (S == 1)
    {
        E++;
        return E;
    }
    else if ((S - D) == 1)
    {
        E--;
        return E;
    }
    else
    {
        return E;
    }
}

int BinaryBoltzmann::max_energy(int k, int q) const
{
    return k;
}

// PairwiseBoltzmann implementation
PairwiseBoltzmann::PairwiseBoltzmann(float beta)
{
    this->beta = beta;
}

float PairwiseBoltzmann::compute_weight(int E) const
{
    return exp(-beta * E);
}

int PairwiseBoltzmann::compute_energy(vector<int> &intersection) const
{
    int E = 0;
    for (int i = 0; i < intersection.size(); i++)
    {
        E += intersection[i];
    }
    return E;
}

int PairwiseBoltzmann::update_energy(int E, int S, int D) const
{
    return E + D;
}

int PairwiseBoltzmann::compute_square_energy(int s) const
{
    return s * (s - 1) / 2;
}

int PairwiseBoltzmann::max_energy(int k, int q) const
{
    return k * (q - 1);
}

// OverflowBoltzmann implementation
OverflowBoltzmann::OverflowBoltzmann(float beta)
{
    this->beta = beta;
}

float OverflowBoltzmann::compute_weight(int E) const
{
    return exp(-beta * E);
}

int OverflowBoltzmann::compute_energy(vector<int> &intersections) const
{
    int E = 0;
    for (int i = 0; i < intersections.size(); i++)
    {
        if (intersections[i] > 0)
        {
            E++;
        }
    }
    return E;
}

int OverflowBoltzmann::update_energy(int E, int S, int D) const
{
    if (S == 0)
    {
        return E - 1;
    }
    else if (S == 1 && D == 1)
    {
        return E + 1;
    }
    else
    {
        return E;
    }
}

int OverflowBoltzmann::compute_square_energy(int s) const
{
    return max(0, s - 1);
}

int OverflowBoltzmann::max_energy(int k, int q) const
{
    return k;
}