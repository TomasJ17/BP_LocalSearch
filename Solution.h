#pragma once

#include <vector>
#include "Instance.h"

struct Solution {
    /** Trasa riešenia 
     * Trasa = zoznam vrcholov v poradí návštev, vrátane štartovacieho a cieľového (indexy do inst.nodes)
     * Vždy začína štartovacím vrcholom (index 0) a končí cieľovým vrcholom (index n-1)
     * Naapr.: [0, 5, 12, 7, 31]
     * */
    std::vector<int> route;

    int    totalScore(const Instance& inst)    const; // suma profitov navštívených vrcholov
    double totalDistance(const Instance& inst) const; // súčet vzdialeností medzi po sebe idúcimi vrcholmi v trase (celková dĺžka trasy)
    bool   isFeasible(const Instance& inst)    const; // true ak totalDistance <= inst.tMax, false inak
};