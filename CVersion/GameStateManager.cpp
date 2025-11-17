#include "GameStateManager.h"
#include <unordered_set>
#include <random>
#include <cmath>

using namespace std;

GameStateManager::GameStateManager(int dim, int ship_length, int active_ship_number, const vector<Ship> &ships, WeightingTemplate *weighting, int seed) : grid_dim(dim), ship_length(ship_length), active_ship_number(active_ship_number), ships(ships), weighting(weighting), seed(seed), grid_energy(0)
{
    // Build the grid
    grid.resize(grid_dim * grid_dim);

    create_squares_to_ship_ids();

    max_energy = weighting->max_energy(ship_length, active_ship_number);
    bucketed_ids.resize(max_energy + 1);

    cached_weights.resize(max_energy + 1);
    for (int E = 0; E <= max_energy; ++E)
    {
        cached_weights[E] = weighting->compute_weight(E);
    }

    // In place of the compute energies call.
    bucketed_ids[0].resize(ships.size());
    int bound = ships.size();
    for (int i = 0; i < bound; ++i)
    {
        bucketed_ids[0][i] = i;
        ships[i].level_idx = i;
        ships[i].E_current = 0;
        ships[i].E_old = 0;
    }
    total_weight = ships.size();

    gen.seed(seed);
}

void GameStateManager::create_squares_to_ship_ids()
{
    squares_to_ship_ids.resize(grid_dim * grid_dim);
    for (const Ship &ship : ships)
    {
        for (int i = 0; i < ship.len; i++)
        {
            squares_to_ship_ids[linearize(ship.rows[i], ship.cols[i])].push_back(ship.id);
        }
    }
}

void GameStateManager::update_buckets(const Ship &ship, int inc)
{
    // Do i have redundant computations that i can save for computation of total grid energy?
    // All weighting assumes linearity.
    // Do this once in ctor?
    vector<int> cumulative_ids;
    cumulative_ids.reserve(2 * ship_length * ship_length); // Less actually. (or not?)

    for (int i = 0; i < ship.len; i++)
    {
        vector<int> &ids = squares_to_ship_ids[linearize(ship.rows[i], ship.cols[i])];
        int square = grid[linearize(ship.rows[i], ship.cols[i])];
        for (int id : ids)
        {
            // ships[id].E_current = weighting->update_energy(ships[id].E_current, square, inc);
            ships[id].E_current = WeightingTemplate::bla(ships[id].E_current, square, inc);
            cumulative_ids.push_back(id);
        }
    }

    for (int id : cumulative_ids)
    {
        const Ship &_ship = ships[id];
        if (_ship.E_old != _ship.E_current)
        {
            // The logic works even if there is only one element.
            int idx_of_removed = _ship.level_idx;
            int last_id = bucketed_ids[_ship.E_old].back();      // Get last id of the vector
            bucketed_ids[_ship.E_old][idx_of_removed] = last_id; // Place last id in index of removed
            bucketed_ids[_ship.E_old].pop_back();                // Remove tail
            ships[last_id].level_idx = idx_of_removed;           // Remap the last id

            bucketed_ids[_ship.E_current].push_back(id);
            ships[id].level_idx = static_cast<int>(bucketed_ids[_ship.E_current].size()) - 1;

            total_weight -= cached_weights[_ship.E_old];
            _ship.E_old = _ship.E_current;

            total_weight += cached_weights[_ship.E_current];
        }
    }
}

void GameStateManager::update_grid(const Ship &ship, int inc)
{
    for (int i = 0; i < ship.len; i++)
    {
        int &square = grid[linearize(ship.rows[i], ship.cols[i])];
        grid_energy += max(0, square - 1 + inc) - max(0, square - 1);
        square += inc;
    }
}

void GameStateManager::place_ship(const Ship &ship)
{
    temp_active_ships.push_back(&ship);
    update_grid(ship, 1);
    update_buckets(ship, 1);
}

void GameStateManager::remove_ship(const Ship &ship)
{
    update_grid(ship, -1);
    update_buckets(ship, -1);
}

int GameStateManager::sample_shipid()
{
    // Find bucket
    float u = uniform_sampler(gen);
    float cumulative_weight = 0;
    int energy_level = 0;
    while (true)
    {
        cumulative_weight += static_cast<int>(bucketed_ids[energy_level].size()) * cached_weights[energy_level];
        if ((cumulative_weight >= total_weight * u) || max_energy == energy_level) // Floating point accuracy don't guarantee the first condition.
        {
            break;
        }
        energy_level++;
    }

    // Sample bucket
    u = uniform_sampler(gen);
    int idx = floor(u * static_cast<int>(bucketed_ids[energy_level].size()));
    int id = bucketed_ids[energy_level][idx];
    return id;
}

void GameStateManager::update_active_ships()
{
    active_ships = move(temp_active_ships);
    temp_active_ships.clear(); // Just in case.
}

int GameStateManager::linearize(int r, int c) const
{
    return r * grid_dim + c;
}

void GameStateManager::reset()
{
    // Reset stuff to empty

    grid_energy = 0;

    for (int E = 0; E <= max_energy; ++E)
    {
        cached_weights[E] = weighting->compute_weight(E);
    }

    // try memset?
    int bound = grid.size();
    for (int i = 0; i < bound; ++i)
    {
        grid[i] = 0;
    }

    active_ships.clear();
    temp_active_ships.clear();

    bucketed_ids.clear();
    bucketed_ids.resize(max_energy + 1);
    bucketed_ids[0].resize(ships.size());

    bound = ships.size();
    for (int i = 0; i < bound; ++i)
    {
        bucketed_ids[0][i] = i;
        ships[i].level_idx = i;
        ships[i].E_current = 0;
        ships[i].E_old = 0;
    }
    total_weight = ships.size();

    // Reset grid to starting position
    int count = 0;
    for (int r = 0; r < grid_dim; r++)
    {
        if (count >= active_ship_number)
        {
            break;
        }
        for (int c = 0; c < grid_dim - ship_length + 1; c += ship_length)
        {
            int id = compute_id(grid_dim, r, c, Orientation::H, ship_length);
            place_ship(ships.at(id));
            count++;
            if (count >= active_ship_number)
            {
                break;
            }
        }
    }
    update_active_ships();
}