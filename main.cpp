#include <iostream>
#include <iomanip>
#include <string>
#include "Instance.h"
#include "Solution.h"
#include "Greedy.h"
#include "Benchmark.h"
#include "ReferenceData.h"
#include "Result.h"
#include "2opt.h"
#include "OrOpt.h"
#include "Swap.h"
#include "Replace.h"

using namespace std;

int main(int argc, char* argv[]) {

    int choice = -1;
    while (choice != 0) {
        
    // Interaktívne menu
    cout << "=== BP Local Search-Orienteering Problem ===\n\n";
    cout << "1. Greedy na jednej instancii\n";
    cout << "2. Greedy na vsetkych instanciach\n";
    cout << "0. Koniec\n";
        cout << "\nVolba: ";
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            cout << "Cesta k suboru: ";
            string path;
            getline(cin, path);
            runSingle(path, "GreedyII", greedyInsert);
        }
        else if (choice == 2) {
           cout << "Zakladny adresar (Enter = aktualny): ";
            string base;
            getline(cin, base);
            if (base.empty()) base = ".";

            // Greedy
            ResultList results = runAll(base, "GreedyII", greedyInsert);
            printResults(results);

            // Test TwoOpt na jednej instancii
            cout << "\n--- Test novych algoritmov na p1.2.c ---\n";
            Instance inst = loadInstance(base + "/Set_32_234/p1.2.c.txt");
            
            std::vector<int> unvisited;
            Solution solGreedy  = greedyInsert(inst, unvisited);
            Solution solTwoOpt  = twoOpt(inst, solGreedy);
            Solution solOrOpt   = OrOpt(inst, solGreedy);
            Solution solSwap    = mySwap(inst, solGreedy);
            Solution solReplace = Replace(inst, solGreedy, unvisited);

            cout << "Greedy  : " << solGreedy.totalScore(inst)  << "\n";
            cout << "TwoOpt  : " << solTwoOpt.totalScore(inst)  << "\n";
            cout << "OrOpt   : " << solOrOpt.totalScore(inst)   << "\n";
            cout << "Swap    : " << solSwap.totalScore(inst)     << "\n";
            cout << "Replace : " << solReplace.totalScore(inst)  << "\n";

            cout << "\nUlozit do CSV? (a/n): ";
            string ans;
            getline(cin, ans);
            if (ans == "a" || ans == "A")
                saveResultCSV(results, "vysledky_greedy.csv");
        }
        else if (choice != 0) {
            cout << "Neznama volba.\n";
        }
    }

    return 0;
}