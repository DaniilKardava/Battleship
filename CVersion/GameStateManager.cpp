#include "GameStateManager.h"
#include <unordered_set>
#include <random>
#include <cmath>

using namespace std;

GameStateManager::GameStateManager(int dim, int ship_length, int active_ship_number, const vector<Ship> &ships, WeightingTemplate *weighting, int seed) : grid_dim(dim), ship_length(ship_length), active_ship_number(active_ship_number), ships(ships), weighting(weighting), seed(seed)
{
    // Build the grid
    grid.resize(grid_dim * grid_dim);

    create_squares_to_ship_ids();

    max_energy = weighting->max_energy(ship_length, active_ship_number);
    bucketed_ids.resize(max_energy + 1);

    // In place of the compute energies call.
    bucketed_ids[0].resize(ships.size());
    for (int i = 0; i < ships.size(); ++i)
    {
        bucketed_ids[0][i] = i;
        ships[i].level_idx = i;
        ships[i].E = 0;
    }
    total_weight = ships.size();

    gen.seed(seed);
}

void GameStateManager::create_squares_to_ship_ids()
{
    squares_to_ship_ids.resize(grid_dim * grid_dim);
    for (const Ship &ship : ships)
    {
        for (int i = 0; i < ship.rows.size(); i++)
        {
            squares_to_ship_ids[linearize(ship.rows[i], ship.cols[i])].push_back(ship.id);
        }
    }
}

void GameStateManager::update_buckets(const Ship &ship, int inc)
{
    for (int i = 0; i < ship.len; i++)
    {
        vector<int> &ids = squares_to_ship_ids[linearize(ship.rows[i], ship.cols[i])];
        for (int id : ids)
        {
            int E_old = ships[id].E;
            int E_new = weighting->update_energy(E_old, grid[linearize(ship.rows[i], ship.cols[i])], inc);
            if (E_old != E_new)
            {
                // The logic works even if there is only one element.
                int idx_of_removed = ships[id].level_idx;
                int last_id = bucketed_ids[E_old].back();      // Get last id of the vector
                bucketed_ids[E_old][idx_of_removed] = last_id; // Place last id in index of removed
                bucketed_ids[E_old].pop_back();                // Remove tail
                ships[last_id].level_idx = idx_of_removed;     // Remap the last id

                bucketed_ids[E_new].push_back(id);
                ships[id].level_idx = static_cast<int>(bucketed_ids[E_new].size()) - 1;

                ships[id].E = E_new;

                total_weight -= weighting->compute_weight(E_old);
                total_weight += weighting->compute_weight(E_new);
            }
        }
    }
}

void GameStateManager::update_grid(const Ship &ship, int inc)
{
    for (int i = 0; i < ship.len; i++)
    {
        grid[linearize(ship.rows[i], ship.cols[i])] += inc;
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
        cumulative_weight += static_cast<int>(bucketed_ids[energy_level].size()) * weighting->compute_weight(energy_level);
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

    // try memset
    for (int i = 0; i < grid.size(); ++i)
    {
        grid[i] = 0;
    }

    active_ships.clear();
    temp_active_ships.clear();

    bucketed_ids.clear();
    bucketed_ids.resize(max_energy + 1);
    bucketed_ids[0].resize(ships.size());
    for (int i = 0; i < ships.size(); ++i)
    {
        bucketed_ids[0][i] = i;
        ships[i].level_idx = i;
        ships[i].E = 0;
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