#include "OrOpt.h"

// Implementacia zodpoveda pseudokodu OrOpt_FirstImprovement(tour) z kapitoly 2.3.1
// Pseudokod skusa presuvat segmenty dlzky k=1,2,3 na vsetky mozne pozicie.
// FirstImprovement znamena ze pri prvom zlepseni okamzite vyskoci z vnutornych
// cyklov (goto next_iteration) a zacne novu iteraciu while cyklu od zaciatku.
// While cyklus opakuje cely prehlaad kym v celej iteracii nenajde ziadne zlepsenie.

Solution OrOpt(const Instance& inst, Solution sol) {
    bool improved = true;

    while (improved) {
        improved = false;

        // for k in {1,2,3}
        for (int k = 1; k <= 3; ++k) {

            // for i = 0..len(tour)-1-k
            for (int i = 0; i <= (int)sol.route.size() - 1 - k; ++i) {

                // v = (tour[i],...,tour[i+k-1])
                std::vector<int> v(sol.route.begin() + i,
                                   sol.route.begin() + i + k);

                // tour_without_v = tour with v removed
                std::vector<int> tour_without_v;
                for (int x = 0; x < (int)sol.route.size(); ++x)
                    if (x < i || x >= i + k)
                        tour_without_v.push_back(sol.route[x]);

                // for j = 0..len(tour_without_v)-2
                for (int j = 0; j <= (int)tour_without_v.size() - 2; ++j) {

                    // tour2 = insert v after position j in tour_without_v
                    std::vector<int> tour2;
                    for (int x = 0; x <= j; ++x)
                        tour2.push_back(tour_without_v[x]);
                    for (int x : v)
                        tour2.push_back(x);
                    for (int x = j + 1; x < (int)tour_without_v.size(); ++x)
                        tour2.push_back(tour_without_v[x]);

                    // if cost(tour2) < cost(tour) then return tour2
                    Solution sol2{tour2};
                    if (sol2.totalDistance(inst) < sol.totalDistance(inst)) {
                        sol = sol2;
                        improved = true;
                        goto next_iteration;
                    }
                }
            }
        }
        next_iteration:;
    }

    return sol;
}