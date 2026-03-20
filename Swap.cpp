#include "Swap.h"

// Implementavcia zodpovedajuca pseudokodu Swap_First_Improvement(tour) podla kapitolt 2.3.1
// Swap vymiena dva vrcholy v trase
// FirstImprovement - pri prvom zlepseni okamzite vyskoci a zacne odznova.
// While cyklus opakuje kym existuje zlepsenie (rovnako ako TwoOpt a OrOpt)

Solution mySwap(const Instance& instance, Solution sol) {
    bool improved = true;
    while(improved){
        improved = false;
        // Zaciatok pseudokodu Swap_First_Improvement
        for(int i = 0; i <= sol.route.size() - 3; ++i){
            for(int j = i+1; j <= sol.route.size() - 2; ++j){
                // Vymen vrcholy na poziciach i a j
                Solution newSolution = sol;
                std::swap(newSolution.route[i], newSolution.route[j]);

                // Ak je nova trasa lepsia (ma vacsi profit a je feasible), akceptuj ju a zacni odznova
                if(newSolution.totalDistance(instance) < sol.totalDistance(instance)){
                    sol = newSolution;
                    improved = true;
                    goto next_iteration; // skok na zaciatok while cyklu, zacneme hladat zlepsenie odznova
                }
            }
        }
        next_iteration:; // label pre skok
    }
    return sol;
}