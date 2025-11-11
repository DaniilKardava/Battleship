#include <vector>
#include "Ship.h"

/*
Compute all arrangements of k length ships in an n by n grid.

Parameters:
n: grid dimension
k: ship length
positions: newly declared vector with size 0. preferable to reserve in advance 2n(n-k+1)
*/
void compute_total_positions(int n, int k, std::vector<Ship> &ships);
