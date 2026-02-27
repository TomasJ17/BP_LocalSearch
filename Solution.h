#pragma once

#include <vector>
#include "Instance.h"

struct Solution {
    std::vector<int> route;

    int    totalScore(const Instance& inst)    const;
    double totalDistance(const Instance& inst) const;
    bool   isFeasible(const Instance& inst)    const;
};