#include "Solution.h"

// Prejde celú trasu a spočíta súčet profitov navštívených vrcholov
int Solution::totalScore(const Instance& inst) const {
    int s = 0;
    for (int idx : route)
        s += inst.nodes[idx].profit;
    return s;
}
// Prejde celú trasu a spočíta súčet vzdialeností medzi po sebe idúcimi vrcholmi
double Solution::totalDistance(const Instance& inst) const {
    double d = 0.0;
    for (int i = 0; i + 1 < (int)route.size(); ++i)
        d += inst.distance(route[i], route[i + 1]);
    return d;
}
// Trasa je realizovateľná, ak celková dĺžka trasy je menšia alebo rovná tMax z inštancie (per-vehicle)
// Používame malú toleranciu 1e-9 pre zaokrúhľovacie chyby s double
bool Solution::isFeasible(const Instance& inst) const {
    return totalDistance(inst) <= inst.tMax + 1e-9;
}