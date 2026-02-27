#include "Solution.h"

int Solution::totalScore(const Instance& inst) const {
    int s = 0;
    for (int idx : route)
        s += inst.nodes[idx].score;
    return s;
}

double Solution::totalDistance(const Instance& inst) const {
    double d = 0.0;
    for (int i = 0; i + 1 < (int)route.size(); ++i)
        d += inst.distance(route[i], route[i + 1]);
    return d;
}

bool Solution::isFeasible(const Instance& inst) const {
    return totalDistance(inst) <= inst.tMax + 1e-9;
}