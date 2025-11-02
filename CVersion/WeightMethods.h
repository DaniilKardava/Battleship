#pragma once
#include <vector>
#include <cmath>
#include <unsupported/Eigen/CXX11/Tensor>

class WeightingTemplate
{
    // Blueprint for weighting mechanisms.

public:
    /*
    Compute the marginal ship weight from an integer summary.

    Parameters:
    E: integer

    Returns:
    Unnormalized marginal weight
    */
    virtual float compute_weight(int E) const = 0;

    /*
    Compute the ship's energy given its intersection with the current board.

    Parameters:
    intersection: view of grid squares

    Returns:
    Energy
    */
    virtual int compute_energy(std::vector<int> intersection) const = 0;

    /*
    Computes the elementwise energy contribution on a rank 3 tensor.

    Parameters:
    samples: rank 3 tensor

    Returns:
    tensor, 3: elementwise operation applied to input.
    */
    virtual Eigen::Tensor<int, 3> compute_square_energy(Eigen::Tensor<int, 3> &arr) const = 0;

    /*
    Update the energy given a change at a single square.

    Parameters:
    E: current energy
    S: current square value
    D: change in square value, not 0.

    return:
    energy
    */
    virtual int update_energy(int E, int S, int D) const = 0;
};

class HardLattice : public WeightingTemplate
{
    /*
    Weighting associated with assigning equal probability to all zero
    intersection states. A ship has weight 1 or 0 depending on whether it can be
    placed on the current board.
    */
public:
    float compute_weight(int E) const override;

    int compute_energy(std::vector<int> intersection) const override;

    Eigen::Tensor<int, 3> compute_square_energy(Eigen::Tensor<int, 3> &arr) const override;

    int update_energy(int E, int S, int D) const override;
};

class BinaryBoltzmann : public WeightingTemplate
{
    /*
    Weighting associated with max(0, min(1, n-1)) energy per square. The ship
    contributes 1 unit of energy for each square where occupancy was 1 before
    placement of this ship.
    */
public:
    float beta;

    BinaryBoltzmann(float beta);

    float compute_weight(int E) const override;

    int compute_energy(std::vector<int> intersection) const override;

    Eigen::Tensor<int, 3> compute_square_energy(Eigen::Tensor<int, 3> &arr) const override;

    int update_energy(int E, int S, int D) const override;
};

class PairwiseBoltzmann : public WeightingTemplate
{
    /*
    Weighting associated with n choose 2 energy per square with n occupants. The
    ship contributes n units of energy for each square with n occupants.
    */

public:
    float beta;
    PairwiseBoltzmann(float beta);

    float compute_weight(int E) const override;

    int compute_energy(std::vector<int> intersection) const override;

    int update_energy(int E, int S, int D) const override;

    Eigen::Tensor<int, 3> compute_square_energy(Eigen::Tensor<int, 3> &arr) const override;
};

class OverflowBoltzmann : public WeightingTemplate
{
    /*
    Weighting associated with max(0, n-1) energy per square with n occupants. The
    ship contributes 1 units of energy for each square with 1 or more occupants.
    */

public:
    float beta;
    OverflowBoltzmann(float beta);

    float compute_weight(int E) const override;

    int compute_energy(std::vector<int> intersections) const override;

    int update_energy(int E, int S, int D) const override;

    Eigen::Tensor<int, 3> compute_square_energy(Eigen::Tensor<int, 3> &arr) const override;
};