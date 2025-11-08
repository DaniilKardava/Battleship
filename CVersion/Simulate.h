#include <vector>
#include "Ship.h"
#include "WeightMethods.h"
#include <tuple>
/*
Return sampled ship arrangements. Accepts precomputed positions to avoid
needlessly recomputing them on each run.

Parameters:
n: integer grid dimension
k: integer ship length
q: number of ships
iterations: integer number of samples
weighting: method for marginal computation
verbose: boolean
positions: single ship positions
*/
std::tuple<std::vector<int>, std::vector<int>> simulate(int n, int k, int q, int iterations, WeightingTemplate *weighting, bool verbose, std::vector<Ship> positions, bool record_samples);