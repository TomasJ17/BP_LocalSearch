#pragma once

#include "Instance.h"
#include "Solution.h"
#include <vector>

Solution greedyInsert(const Instance& inst);

Solution greedyInsert(const Instance& inst, std::vector<int>& unvisited);