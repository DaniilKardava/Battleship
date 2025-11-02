#include "WeightMethods.h"

using namespace std;
using namespace Eigen;

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

int HardLattice::compute_energy(vector<int> intersection) const
{
    int E = 0;
    for (int i = 0; i < intersection.size(); i++)
    {
        E += intersection[i];
    }
    return E;
}

Tensor<int, 3> HardLattice::compute_square_energy(Tensor<int, 3> &arr) const
{
    return arr.constant(0);
}

int HardLattice::update_energy(int E, int S, int D) const
{
    return E + D;
}

// BinaryBoltzmann implementation
BinaryBoltzmann::BinaryBoltzmann(float beta) : beta(beta) {};

float BinaryBoltzmann::compute_weight(int E) const
{
    return exp(-beta * E);
}

int BinaryBoltzmann::compute_energy(vector<int> intersection) const
{
    int E = 0;
    for (int i = 0; i < intersection.size(); i++)
    {
        if (E == 1)
        {
            E++;
        }
    }
    return E;
}

Tensor<int, 3> BinaryBoltzmann::compute_square_energy(Tensor<int, 3> &arr) const
{
    return arr.constant(0).cwiseMax(arr.constant(1).cwiseMax(arr - 1));
}

int BinaryBoltzmann::update_energy(int E, int S, int D) const
{
    if (S == 1)
    {
        return E++;
    }
    else if (S - D == 1)
    {
        return E--;
    }
    else
    {
        return E;
    }
}

// PairwiseBoltzmann implementation
PairwiseBoltzmann::PairwiseBoltzmann(float beta) : beta(beta) {};

float PairwiseBoltzmann::compute_weight(int E) const
{
    return exp(-beta * E);
}

int PairwiseBoltzmann::compute_energy(vector<int> intersection) const
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

Tensor<int, 3> PairwiseBoltzmann::compute_square_energy(Tensor<int, 3> &arr) const
{
    return arr.unaryExpr([](int n)
                         { return n * (n - 1) / 2; });
}

// OverflowBoltzmann implementation
OverflowBoltzmann::OverflowBoltzmann(float beta) : beta(beta) {};

float OverflowBoltzmann::compute_weight(int E) const
{
    return exp(-beta * E);
}

int OverflowBoltzmann::compute_energy(vector<int> intersections) const
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

Tensor<int, 3> OverflowBoltzmann::compute_square_energy(Tensor<int, 3> &arr) const
{
    return arr.constant(0).cwiseMax(arr - 1);
}
