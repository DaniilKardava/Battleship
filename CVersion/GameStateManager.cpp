#include "GameStateManager.h"
#include <unordered_set>
#include <random>
#include <cmath>

using namespace std;

GameStateManager::GameStateManager(vector<int> grid, vector<Ship> ships, vector<Ship> active_ships, WeightingTemplate *weighting, int seed) : grid(grid), grid_dim(sqrt(grid.size())), ships(ships), active_ships(active_ships), weighting(weighting), seed(seed)
{
    for (int i = 0; i < ships.size(); i++)
    {
        ship_to_id[ships[i]] = i;
        id_to_ship.emplace(i, ships[i]); // Ship has no default constructor (implicitly called in [])
    }
    squares_to_ship_ids = create_squares_to_ship_ids();
    energies = compute_energies();
    marginals = compute_marginals();
    gen.seed(seed);
}

vector<vector<int>> GameStateManager::create_squares_to_ship_ids() const
{
    vector<vector<int>> squares_to_ship_ids(grid.size());
    for (const Ship &ship : ships)
    {
        for (int i = 0; i < ship.rows.size(); i++)
        {
            squares_to_ship_ids[linearize(ship.rows[i], ship.cols[i])].push_back(ship_to_id.at(ship));
        }
    }
    return squares_to_ship_ids;
}

vector<int> GameStateManager::compute_energies() const
{
    vector<int> energies(ships.size());
    for (const Ship &ship : ships)
    {
        vector<int> intersection;
        for (int i = 0; i < ship.rows.size(); i++)
        {
            intersection.push_back(grid[linearize(ship.rows[i], ship.cols[i])]);
        }
        energies[ship_to_id.at(ship)] = weighting->compute_energy(intersection); // [] is not const
    }
    return energies;
}

vector<float> GameStateManager::compute_marginals() const
{
    vector<int> energies = compute_energies();
    vector<float> marginals(ships.size());
    int E = 0;
    for (const Ship &ship : ships)
    {
        E = energies[ship_to_id.at(ship)];
        marginals[ship_to_id.at(ship)] = weighting->compute_weight(E);
    }
    return marginals;
}

void GameStateManager::update_marginals(Ship &ship, int inc)
{
    unordered_set<int> uniq_ids;
    for (int i = 0; i < ship.rows.size(); i++)
    {
        vector<int> ids = squares_to_ship_ids[linearize(ship.rows[i], ship.cols[i])]; // This is allegedly copying.
        uniq_ids.insert(ids.begin(), ids.end());                                      // This still feels inefficient
        for (int id : ids)
        {
            energies[id] = weighting->update_energy(energies[id], grid[linearize(ship.rows[i], ship.cols[i])], inc);
        }
    }
    for (int id : uniq_ids)
    {
        marginals[id] = weighting->compute_weight(energies[id]);
    }
}

void GameStateManager::update_grid(Ship &ship, int inc)
{
    for (int i = 0; i < ship.rows.size(); i++)
    {
        grid[linearize(ship.rows[i], ship.cols[i])] += inc;
    }
}

void GameStateManager::place_ship(Ship &ship)
{
    temp_active_ships.push_back(ship);
    update_grid(ship, 1);
    update_marginals(ship, 1);
}

void GameStateManager::remove_ship(Ship &ship)
{
    update_grid(ship, -1);
    update_marginals(ship, -1);
}

Ship GameStateManager::sample_ship()
{
    /*
    I can avoid reinit, but building is still n after any weight change. Use
    more efficient sampler that supports frequent weight changes.
    */
    discrete_distribution<> dist(marginals.begin(), marginals.end());
    return ships[dist(gen)]; // vector look up is constant, sampling is log n.
}

void GameStateManager::update_active_ships()
{
    active_ships = move(temp_active_ships);
}

int GameStateManager::linearize(int r, int c) const
{
    return r * grid_dim + c;
}