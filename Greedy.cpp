#include "Greedy.h"

Solution greedyInsert(const Instance& inst) {
    Solution sol;
    sol.route.push_back(inst.start().index);
    sol.route.push_back(inst.end().index);

    std::vector<bool> visited(inst.n, false);
    visited[inst.start().index] = true;
    visited[inst.end().index] = true;

    bool feasible;

    do {
        feasible = false;
        double bestScore = -1.0;
        int bestNode = -1;
        int bestPos = -1;

        for (int j = 1; j < inst.n - 1; ++j) {
            if (visited[j]) continue;
        
            for(int i = 0; i < (int)sol.route.size() - 1; ++i)
            {
                double delta_ij = inst.distance(sol.route[i], j) + inst.distance(j, sol.route[i + 1]) - inst.distance(sol.route[i], sol.route[i + 1]);
                if(sol.totalDistance(inst) + delta_ij <= inst.tMax)
                {
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
        
        if(feasible)
        {
            sol.route.insert(sol.route.begin() + bestPos + 1, bestNode);
            visited[bestNode] = true;
        }
    } while (feasible);
    return sol;
}