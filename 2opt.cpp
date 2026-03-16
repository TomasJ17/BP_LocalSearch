#include "2opt.h"
#include <algorithm>

// Otoci segment trasy od i+1 do j (vratane)
// napr. [0,1,2,3,4,5] s i=1, j=4 -> [0,1,4,3,2,5]
static Solution applyTwoOpt(Solution sol, int i, int j) {
    std::reverse(sol.route.begin() + i + 1,
                 sol.route.begin() + j + 1);
    return sol;
}

// Implementacia zodpoveda pseudokodu TwoOpt_BestImprovement(tour) z kapitoly 2.3.1
// Pseudokod popisuje jednu iteraciu - prehla vsetkych dvojic hran a najde
// najlepsiu vymennu (bestDelta). V implementacii je pseudokod obaleny do while
// cyklu ktory ho opakuje kym existuje zlepsenie. Toto je standardny sposob
// aplikacie BestImprovement - jedna iteracia nemusi stacit pretoze po kazdej
// vymene mozu vzniknut nove prilezitosti na zlepsenie.
Solution twoOpt(const Instance& inst, Solution sol) {
    bool improved = true;

    while (improved) {
        improved = false;

        // --- zacatok pseudokodu TwoOpt_BestImprovement ---

        // bestTour = tour
        Solution bestTour = sol;

        // bestDelta = 0
        double bestDelta = 0;

        // for i = 0..len(tour)-2
        for (int i = 0; i <= (int)sol.route.size() - 2; ++i) {

            // for j = i+2..len(tour)-2
            for (int j = i + 2; j <= (int)sol.route.size() - 2; ++j) {

                // delta = dist(tour[i],tour[j]) + dist(tour[i+1],tour[j+1])
                //       - dist(tour[i],tour[i+1]) - dist(tour[j],tour[j+1])
                double delta =
                    inst.distance(sol.route[i],     sol.route[j])     +
                    inst.distance(sol.route[i + 1], sol.route[j + 1]) -
                    inst.distance(sol.route[i],     sol.route[i + 1]) -
                    inst.distance(sol.route[j],     sol.route[j + 1]);

                // if delta < bestDelta then
                if (delta < bestDelta) {
                    // bestDelta = delta
                    bestDelta = delta;
                    // bestTour = applyTwoOpt(tour, i, j)
                    bestTour = applyTwoOpt(sol, i, j);
                }
            }
        }

        // return bestTour - v implementacii nenahradzame return ale aplikujeme
        // zlepsenie a opakujeme while cyklus
        if (bestDelta < 0) {
            sol = bestTour;
            improved = true;
        }

        // --- koniec pseudokodu TwoOpt_BestImprovement ---
    }

    return sol;
}