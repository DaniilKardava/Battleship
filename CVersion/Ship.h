#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstddef>

enum class Orientation
{
    H,
    V
};

class Ship
{
    // Immutable and compared by value.

public:
    const int r; // Corner row
    const int c; // Corner col
    const Orientation o;
    const int len;
    const std::vector<int> rows;
    const std::vector<int> cols;
    const int id;
    mutable int level_idx;
    mutable int E;

    Ship(int id, int r, int c, Orientation o, int len);

    bool operator==(const Ship &other) const;

private:
    std::vector<int> compute_cols();
    std::vector<int> compute_rows();
};

/*
Compute id of a length k ship in an n by n grid with orientation o and
coordinates r,c. Indices are computed according to tuple comparison rules (o, r,
c) when o is horizontal and (o,c,r) when o is vertical.

Parameters:
n: grid dimension
r: corner row coordinate
c: corner col coordinate
o: orientation
k: length
*/
int compute_id(int n, int r, int c, Orientation o, int k);

inline void hash_combine(std::size_t &seed, std::size_t v) noexcept { seed ^= v + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2); }

namespace std
{
    template <>
    struct hash<Ship>
    {
        size_t operator()(const Ship &s) const noexcept
        {
            size_t seed = 0;
            hash_combine(seed, hash<int>{}(s.r));
            hash_combine(seed, hash<int>{}(s.c));
            hash_combine(seed, hash<int>{}(s.len));
            hash_combine(seed, hash<int>{}(static_cast<int>(s.o)));
            return seed;
        }
    };
}