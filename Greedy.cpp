#include "Greedy.h"

Solution greedyInsert(const Instance& inst) {
    Solution sol;
    // Začneme s trasou, ktorá obsahuje len štart a cieľ
    sol.route.push_back(inst.start().index);
    sol.route.push_back(inst.end().index);

    // Pomocný vektor na sledovanie navštívených vrcholov
    std::vector<bool> visited(inst.n, false);
    visited[inst.start().index] = true;
    visited[inst.end().index] = true;

    bool feasible;

    do {
        feasible = false;
        double bestScore = -1.0;
        int bestNode = -1; // ktorý vrchol vložiť
        int bestPos = -1; // za ktorú poziciu v trase vložit

        // Prejdi všetky nenavštívené vrcholy(okrem štartu a cieľa)
        for (int j = 1; j < inst.n - 1; ++j) {
            if (visited[j]) continue;
            // Skús vložiť vrchol j medzi všetky možné dvojice susedných vrcholov v trase
            for(int i = 0; i < (int)sol.route.size() - 1; ++i)
            {
                // Vypočítaj nárast vzdialenosti, ak vložíme vrchol j mezi sol.route[i] a sol.route[i + 1]
                double delta_ij = inst.distance(sol.route[i], j) + inst.distance(j, sol.route[i + 1]) - inst.distance(sol.route[i], sol.route[i + 1]);
                // Je vloženie feasible (neprekročí tMax)?
                if(sol.totalDistance(inst) + delta_ij <= inst.tMax)
                {
                    // Hľadáme vrchol s najlepším pomerom profit/delta_ij(zisk/cena)
                    double score= inst.nodes[j].profit / delta_ij;
                    feasible = true;
                    if(score > bestScore)
                    {
                        bestNode = j;
                        bestPos = i;
                        bestScore = score;
                    }
                }
            }
        }
        // Ak sme našli vrchol, ktorý môžeme vložiť, vložíme ho do trasy na najlepšiu pozíciu
        if(feasible)
        {
            sol.route.insert(sol.route.begin() + bestPos + 1, bestNode);
            visited[bestNode] = true;
        }
    // Opakuj kým je možné vložiť ďalší vrchol bez prekročenia tMax
    } while (feasible);
    
    return sol;
}