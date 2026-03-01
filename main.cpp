#include <iostream>
#include "Instance.h"
#include "Solution.h"
#include "Greedy.h"
#include <iomanip>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Pouzitie: main <subor.txt>" << endl;
        return 1;
    }

    // Načítanie inštancie zo súboru
    Instance inst = loadInstance(argv[1]);

    // Výpis informácií o načítanej inštanci
    cout << "Subor: " << inst.name << endl;
    cout << "n    = " << inst.n    << endl;
    cout << "m    = " << inst.m    << endl;
    cout << "tMax = " << inst.tMax << endl;
    cout << endl;
    cout << "Index\tx\ty\tProfit" << endl;
    cout << "-----\t-\t-\t------" << endl;
    for (const Node& node : inst.nodes) {
        cout << node.index << "\t"
             << node.x     << "\t"
             << node.y     << "\t"
             << node.profit << endl;
    }

    // Greedy algoritmu
    cout << endl;
    cout << "=== Greedy Insert II ===" << endl;
    Solution sol = greedyInsert(inst);
    cout << "Profit   : " << sol.totalScore(inst)    << endl;
    cout << fixed << setprecision(4);
    cout << "Vzd.     : " << sol.totalDistance(inst) << endl;
    cout << "Feasible : " << (sol.isFeasible(inst) ? "ano" : "NIE") << endl;
    cout << "Trasa    : ";
    for (int i = 0; i < (int)sol.route.size(); ++i) {
        if (i > 0) cout << " -> ";
        cout << sol.route[i];
    }
    cout << endl;

    return 0;
}