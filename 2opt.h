#pragma once

#include "Instance.h"
#include "Solution.h"

// 2-opt lokálny search pre zlepšenie riešenia - Best Improvement
// Hlada najlepsiu dvojicu hrán (i, i+1) a (j, j+1) v trase, ktoru by sme mohli prohodit a zlepsit tak profit/trasu
// Opakujeme kým sa dá nájsť zlepšenie
Solution twoOpt(const Instance& inst, const Solution& sol);