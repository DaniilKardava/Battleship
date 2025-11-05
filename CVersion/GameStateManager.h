#include "Ship.h"
#include "WeightMethods.h"
#include <unordered_map>
#include <random>

class GameStateManager
{
    // Maintains grid state and marginal ship distributions.
public:
    std::vector<int> grid;
    int grid_dim;

    std::vector<Ship> ships;
    std::vector<Ship> active_ships;
    std::vector<Ship> temp_active_ships;
    WeightingTemplate *weighting;

    std::unordered_map<Ship, int> ship_to_id;
    std::unordered_map<int, Ship> id_to_ship;
    std::vector<std::vector<int>> squares_to_ship_ids;

    std::vector<int> energies;
    std::vector<float> marginals;

    int seed;
    std::mt19937 gen;

    /*
    Parameters:
    grid: initialize the game with a rank 2 tensor
    ships: array of possible ship arrangements
    active_ships: array of actively placed ships
    weighting: a weight class instance
    */
    GameStateManager(std::vector<int> grid, std::vector<Ship> ships, std::vector<Ship> active_ships, WeightingTemplate *weighting, int seed = std::random_device()());

    /*
    Assign squares to the ship orientations that intersect it.

    Returns:
    Square array containing lists of ships
    */
    std::vector<std::vector<int>> create_squares_to_ship_ids() const;

    /*
    Calculate the energy of each ship given the current grid state.

    Returns:
    Array of energies
    */
    std::vector<int> compute_energies() const;

    /*
    Calculate the unnormalized marginal probabilities of selecting a ship
    orientation.

    See theory for when compute and update marginals are appropriate.

    Returns:
    Array of unnormalized marginals.
    */
    std::vector<float> compute_marginals() const;

    /*
    Update the unnormalized marginal probabilities for a subset of ship
    orientations intersecting the squares occupied by the 'ship' parameter.

    To avoid recomputing marginals from scratch, we require knowing what
    change occured at those squares.

    See theory for when compute and update marginals are appropriate.

    Parameters:
    ship: a ship object
    change: integer

    Returns:
    None
    */
    void update_marginals(Ship &ship, int inc);

    /*
    Increment grid cells occupied by ship by amount inc.

    Parameters:
    ship: ship object
    inc: integer

    Returns:
    None
    */
    void update_grid(Ship &ship, int inc);

    /*
    Update grid and marginals associated with new ship placement.

    Parameter:
    ship: Ship object

    Returns:
    None
    */
    void place_ship(Ship &ship);

    /*
    Update grid and marginals associated with new ship placement.

    Parameter:
    ship: Ship object

    Returns:
    None
    */
    void remove_ship(Ship &ship);

    /*
    Sample a ship from the marginal distribution.

    Returns:
    Ship
    */
    Ship sample_ship();

    /*
    Update the list of active ships. The temporary list is created when
    traversing the original list of active ships and resampling each
    element.

    Returns:
    None
    */
    void update_active_ships();

    /*
    Flatten n-tuple coordinates.

    Parameters:
    int r: row index
    int c: col index
    Return:
    int: row major index
    */
    int linearize(int r, int c) const;
};