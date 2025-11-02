#include <string>
#include <vector>
#include "Ship.h"

using namespace std;

Ship::Ship(int r, int c, Orientation o, int len) : r(r), c(c), o(o), len(len), rows(compute_rows()), cols(compute_cols()) {};

bool Ship::operator==(const Ship &other) const
{
    return r == other.r && c == other.c && o == other.o && len == other.len;
}

vector<int> Ship::compute_rows()
{
    vector<int> rows(Ship::len);
    if (Ship::o == Orientation::H)
    {
        for (int i = 0; i < Ship::len; ++i)
        {
            rows[i] = Ship::r;
        }
    }
    else
    {
        for (int i = 0; i < Ship::len; ++i)
        {
            rows[i] = Ship::r + i;
        }
    }
    return rows;
}

vector<int> Ship::compute_cols()
{
    vector<int> cols(Ship::len);
    if (Ship::o == Orientation::H)
    {
        for (int i = 0; i < Ship::len; ++i)
        {
            cols[i] = Ship::c + i;
        }
    }
    else
    {
        for (int i = 0; i < Ship::len; ++i)
        {
            cols[i] = Ship::c;
        }
    }
    return cols;
}