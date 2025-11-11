#include "Ship.h"
#include "WeightMethods.h"
#include <random>

class GameStateManager
{
    // Maintains grid state and marginal ship distributions.
public:
    std::vector<int> grid;
    int grid_dim;
    int active_ship_number; // Used to specify an energy range in constructor.
    int ship_length;        // See above comment
    int max_energy;

    // There is one ship array outside of this class, passed by reference. All ships
    // used throughout the program are an instance in that array.
    const std::vector<Ship> &ships;
    std::vector<const Ship *> active_ships;
    std::vector<const Ship *> temp_active_ships;
    WeightingTemplate *weighting;

    std::vector<std::vector<int>> squares_to_ship_ids;

    int seed;
    std::mt19937 gen;
    std::uniform_real_distribution<float> uniform_sampler;

    std::vector<std::vector<int>> bucketed_ids;
    float total_weight = 0.0f;

    /*
    Parameters:
    dim: int dimension
    ships: array of possible ship arrangements
    weighting: a weight class instance
    */
    GameStateManager(int dim, int ship_length, int active_ship_number, const std::vector<Ship> &ships, WeightingTemplate *weighting, int seed = std::random_device()());

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
    void update_buckets(const Ship &ship, int inc);

    /*
    Increment grid cells occupied by ship by amount inc.

    Parameters:
    ship: ship object
    inc: integer

    Returns:
    None
    */
    void update_grid(const Ship &ship, int inc);

    /*
    Update grid and marginals associated with new ship placement.

    Parameter:
    ship: Ship object

    Returns:
    None
    */
    void place_ship(const Ship &ship);

    /*
    Update grid and marginals associated with new ship placement.

    Parameter:
    ship: Ship object

    Returns:
    None
    */
    void remove_ship(const Ship &ship);

    /*
    Sample a ship id from the marginal distribution.

    Returns:
    int
    */
    int sample_shipid();

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

    /*
    Reset the state of the manager;
    Make grid empty.
    Reset energy and level index attributes of ships.
    Reset bucketed ids.
    Make active ships and temp active ships empty and repopulate.
    */
    void reset();

private:
    /*
    Assign squares to the ship orientations that intersect it.
    */
    void create_squares_to_ship_ids();
};