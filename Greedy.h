#pragma once

#include "Instance.h"
#include "Solution.h"
#include <vector>

Solution greedyInsert(const Instance& inst);

Solution greedyInsert(const Instance& inst, std::vector<int>& unvisited);

// Použitie: po 2-Opt/Or-Opt/Swap, kde máme skrátenú trasu a chceme do nej vložiť ďalšie vrcholy
Solution greedyInsert(const Instance& inst, const Solution& initialSolution, std::vector<int>& unvisited);