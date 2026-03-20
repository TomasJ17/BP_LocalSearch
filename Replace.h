#pragma once

#include "Instance.h"
#include "Solution.h"
#include <vector>

// Replace - Best improvement
// Nahradi navstiveny vrchol v trase nenavstivenym vrcholom, s vyssim profitom, ak je to mozne
// Podla pseudokodu Replace_Best(tour, unvisited, T_max) z kapitoly 2.3.2
Solution Replace(const Instance& instance, Solution sol, const std::vector<int>& unvisited);