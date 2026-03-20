#include "Replace.h"
#include <algorithm>

// Implementavcia zodpovedajuca pseudokodu Replace_Best(tour, unvisited, T_max) podla kapitoly 2.3.2
// Replace nahradi navstiveny vrchol v trase nenavstivenym vrcholom, s vyssim profitom, ak je to mozne
// BestImprovement - hladame vsetky moznosti nahrady a vyberieme tu najlepsiu (najvacsi profit, zaroven feasible)
Solution Replace(const Instance& inst, Solution sol,
                 const std::vector<int>& unvisited) {
    bool improved = true;

    // Pracujeme s kopiou unvisited ktoru aktualizujeme po kazdej zamene
    std::vector<int> remaining = unvisited;

    while (improved) {
        improved = false;

        // --- zacatok pseudokodu Replace_Best ---

        // bestCandidate = null
        int bestU    = -1;
        int bestV    = -1;
        int bestPos  = -1;
        int bestGain = 0;

        // for each u in unvisited
        for (int u : remaining) {

            // for i = 0..len(tour)-2
            for (int i = 0; i <= (int)sol.route.size() - 2; ++i) {

                // v = tour[i]
                int v = sol.route[i];

                // tourMinusV = tour with v removed
                std::vector<int> tourMinusV;
                for (int x = 0; x < (int)sol.route.size(); ++x)
                    if (x != i)
                        tourMinusV.push_back(sol.route[x]);

                // (posU2, extraU2) = argmin over k of insertionExtra(tourMinusV, u, k)
                int    bestInsertPos   = -1;
                double bestInsertExtra = 1e18;

                for (int k = 0; k < (int)tourMinusV.size() - 1; ++k) {
                    double extra =
                        inst.distance(tourMinusV[k], u) +
                        inst.distance(u, tourMinusV[k + 1]) -
                        inst.distance(tourMinusV[k], tourMinusV[k + 1]);
                    if (extra < bestInsertExtra) {
                        bestInsertExtra = extra;
                        bestInsertPos   = k;
                    }
                }

                // Vypocitaj dlzku trasy bez v
                double distMinusV = 0.0;
                for (int k = 0; k + 1 < (int)tourMinusV.size(); ++k)
                    distMinusV += inst.distance(tourMinusV[k], tourMinusV[k + 1]);

                // if time(tourMinusV) + extraU2 <= T_max
                if (distMinusV + bestInsertExtra <= inst.tMax) {

                    // gain = profit(u) - profit(v)
                    int gain = inst.nodes[u].profit - inst.nodes[v].profit;

                    // if gain > bestGain
                    if (gain > bestGain) {
                        bestGain = gain;
                        bestU    = u;
                        bestV    = i;
                        bestPos  = bestInsertPos;
                    }
                }
            }
        }

        // if bestCandidate == null then return tour
        // else remove v, insert u at posU2, return new tour
        if (bestU != -1) {
            int removedV = sol.route[bestV];

            // Odstran v z trasy
            sol.route.erase(sol.route.begin() + bestV);

            // Vloz u na najlepsiu poziciu
            sol.route.insert(sol.route.begin() + bestPos + 1, bestU);

            // Aktualizuj remaining - u uz nie je unvisited, v sa stal unvisited
            remaining.erase(std::find(remaining.begin(), remaining.end(), bestU));
            remaining.push_back(removedV);

            improved = true;
        }

        // --- koniec pseudokodu Replace_Best ---
    }

    return sol;
}