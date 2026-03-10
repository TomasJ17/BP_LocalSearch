#include <iostream>
#include <iomanip>
#include <string>
#include "Instance.h"
#include "Solution.h"
#include "Greedy.h"
#include "Benchmark.h"
#include "ReferenceData.h"
#include "Result.h"

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

            ResultList results = runAll(base, "GreedyII", greedyInsert);
            printResults(results);

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