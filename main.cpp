#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <fstream>
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
namespace fs = std::filesystem;

// Funkcia pre generovanie nazvu CSV suboru: "algoritmus_sada_datumCas.csv"
string generateCSVFilename(const string& algoName, const string& setName) {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm* tm_info = localtime(&t);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", tm_info);
    string setKey = setName;
    std::replace(setKey.begin(), setKey.end(), ' ', '_'); 
    std::replace(setKey.begin(), setKey.end(), '/', '_');
    return algoName + "_" + setKey + "_" + buffer + ".csv";
}

void setupRescueScenario(Instance& inst, int startID, int endID, double limit) {
    // 1. Nájdeme aktuálne indexy uzlov podľa ID
    int sIdx = -1, eIdx = -1;
    for(int i = 0; i < (int)inst.nodes.size(); i++) {
        if(inst.nodes[i].id == startID) sIdx = i;
        if(inst.nodes[i].id == endID) eIdx = i;
    }
    if(sIdx == -1 || eIdx == -1) return;
    std::vector<int> oldToNew(inst.nodes.size(), -1);
    // 2. Preusporiadame LEN vektor nodes (štart na začiatok, cieľ na koniec)
    std::vector<Node> noveNodes;
    noveNodes.push_back(inst.nodes[sIdx]); 
    oldToNew[sIdx] = 0;

    int newIdx = 1;
    for(int i = 0; i < (int)inst.nodes.size(); i++) {
        if(inst.nodes[i].id != startID && inst.nodes[i].id != endID) {
            noveNodes.push_back(inst.nodes[i]);
            oldToNew[i] = newIdx;
            newIdx++;
        }
    }
    noveNodes.push_back(inst.nodes[eIdx]);
    oldToNew[eIdx] = newIdx;

    std::vector<std::vector<double>> newDistMatrix(inst.nodes.size(), 
                                                std::vector<double>(inst.nodes.size(), 0.0));

    for(int oldI = 0; oldI < (int)inst.nodes.size(); oldI++) {
        for(int oldJ = 0; oldJ < (int)inst.nodes.size(); oldJ++) {
            int newI = oldToNew[oldI];
            int newJ = oldToNew[oldJ];
            
            if(newI != -1 && newJ != -1) {
                newDistMatrix[newI][newJ] = inst.distMatrix[oldI][oldJ];
            }
        }
    }

    inst.distMatrix = newDistMatrix;

    inst.nodes = noveNodes;
    inst.n = (int)inst.nodes.size();
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 2* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //Pre každú dvojicu vrcholov spočítajte najcennejšiu trasu, ktorá nebude dlhšia ako 2-krát najkratšia cesta.
    inst.tMax = 2 * limit;

    // 3. Preindexovanie (aby algoritmy vedeli, kde je teraz ktorý uzol)
    inst.idToIndex.clear();
    for(int i = 0; i < inst.nodes.size(); i++) {
        inst.nodes[i].index = i;
        inst.idToIndex[inst.nodes[i].id] = i;
    }
    inst.nodes.front().profit = 0;
    inst.nodes.back().profit = 0;
}

int main(int argc, char* argv[]) {

    if(argc >= 2){
        runSingle(argv[1], "GreedyII", greedyInsert);
        return 0;
    }

    int choice = -1;
    while (choice != 0) {
        // Interaktívne menu
        cout << "=== BP Local Search-Orienteering Problem ===\n\n";
        cout << "ZÁKLADNÉ ALGORITMY:\n";
        cout << "1. Greedy - jedna instancia\n";
        cout << "2. Greedy + 2-opt - jedna instancia\n";
        cout << "3. Greedy + Or-opt - jedna instancia\n";
        cout << "4. Greedy + Swap - jedna instancia\n";
        cout << "5. Greedy + Replace - jedna instancia\n";
        cout << "\nKOMBINOVANÉ ALGORITMY:\n";
        cout << "11. Greedy + 2-opt + GreedyII - jedna instancia\n";
        cout << "12. Greedy + Or-opt + GreedyII - jedna instancia\n";
        cout << "13. Greedy + Swap + GreedyII - jedna instancia\n";
        cout << "\n Spracovanie experimentov:\n";
        cout << "6. Spusti vsetky ZÁKLADNÉ algoritmy na vsetkych instanciach\n";
        cout << "7. Jeden algoritmus na vsetkych instanciach -> CSV\n";
        cout << "8. Spusti vsetky ZÁKLADNÉ algoritmy na vybranych testovacich instanciach -> CSV\n";
        cout << "9. Spusti vsetky KOMBINOVANÉ algoritmy na vybranych testovacich instanciach -> CSV\n";
        cout << "10. Spusti ZAKLADNE + KOMBINOVANE algoritmy na vybranych testovacich instanciach -> CSV\n";
        cout << "14. Prakticka uloha: Zachranna sluzba ZSK\n";

        cout << "0. Koniec\n";
        cout << "\nVolba: ";
        cin >> choice;
        cin.ignore();

        if ((choice >= 1 && choice <= 5) || (choice >= 11 && choice <= 13)) {
            cout << "Cesta k suboru: ";
            string path;
            getline(cin, path);
            
            Instance inst = loadInstance(path);
            vector<int> unvisited;
            auto t0 = chrono::high_resolution_clock::now();
            Solution solGreedy = greedyInsert(inst, unvisited);
            auto t1 = chrono::high_resolution_clock::now();
            double greedy_ms = chrono::duration<double, milli>(t1 - t0).count();

            string algoName;
            Solution improvedSol;
            double ms = 0.0;
            // Zakladne algoritmy
            if(choice >= 1 && choice <= 5) {
                auto t2 = chrono::high_resolution_clock::now();
                switch (choice) {
                    case 1:
                        improvedSol = solGreedy;
                        algoName = "Greedy II";
                        break;
                    case 2:
                        improvedSol = twoOpt(inst, solGreedy);
                        algoName = "GreedyII + 2-opt";
                        break;
                    case 3:
                        improvedSol = OrOpt(inst, solGreedy);
                        algoName = "GreedyII + Or-opt";
                        break;
                    case 4:
                        improvedSol = Swap(inst, solGreedy);
                        algoName = "GreedyII + Swap";
                        break;
                    case 5:
                        improvedSol = Replace(inst, solGreedy, unvisited);
                        algoName = "GreedyII + Replace";
                        break;
                }
                ms = chrono::duration<double, milli>
                (chrono::high_resolution_clock::now() - t2).count();
            }
            // Kombinovane algoritmy ( 2-opt/Or-Opt/Swap + opat Greedy
            else if(choice >= 11 && choice <= 13) {
                auto t2 = chrono::high_resolution_clock::now();
                Solution afterShortening;
                Solution afterGreedyII;
    
                switch (choice) {
                    case 11:
                        afterShortening = twoOpt(inst, solGreedy);
                        algoName = "2-Opt+Greedy";
                        break;
                    case 12:
                        afterShortening = OrOpt(inst, solGreedy);
                        algoName = "Or-Opt+Greedy";
                        break;
                    case 13:
                        afterShortening = Swap(inst, solGreedy);
                        algoName = "Swap+Greedy";
                        break;
                }
    
                // Aktualizuj unvisited - zisti ktore vrcholy nie su v afterShortening
                vector<bool> visited(inst.n, false);
                for(int v : afterShortening.route) {
                    visited[v] = true;
                }
                unvisited.clear();
                for(int j = 1; j < inst.n - 1; ++j) {
                    if(!visited[j]) unvisited.push_back(j);
                }
                
                // Spusti Greedy na skratenej trase
                afterGreedyII = greedyInsert(inst, afterShortening, unvisited);
                improvedSol = afterGreedyII;
                ms = chrono::duration<double, milli>
                    (chrono::high_resolution_clock::now() - t2).count();
            }


        string key = fs::path(path).filename().string();
        RefValues rv = getReference(key);
        cout << "\nSubor : " << inst.name << "\n";
        cout << "n     : " << inst.n    << "\n";
        cout << "tMax  : " << inst.tMax << "\n";
        cout << left
             << setw(25) << "Algoritmus"
             << setw(10) << "Profit"
             << setw(12) << "Vzdialenost"
             << setw(12) << "CPU-ms"
             << "\n";  
        cout << string(60, '-') << "\n";
            // Greedy II vždy zobrazíme, aj keď uživatel zvolil napríklad Greedy + 2-opt, aby bolo vidno zlepšenie oproti pôvodnému greedy riešeniu
            // Ak zvolil Greedy II, tak ho zobrazíme len raz
            if (choice > 1)
            {            
                cout << left << setw(25) << "Greedy II"
                     << setw(10) << solGreedy.totalScore(inst)
                     << fixed << setprecision(4) << setw(12) << solGreedy.totalDistance(inst)
                     << fixed << setprecision(2) << setw(12) << greedy_ms  << "\n";
            }
            
            cout << left << setw(25) << algoName
                 << setw(10) << improvedSol.totalScore(inst)
                 << fixed << setprecision(4) 
                 << setw(12) << improvedSol.totalDistance(inst)
                 << fixed << setprecision(2) << ms << "ms\n";

            int ref = -1;

            if (rv.opt >= 0) ref = rv.opt;
            else if (rv.gls >= 0) ref = rv.gls;
            else if (rv.chao >= 0) ref = rv.chao;
            else if (rv.fischetti >= 0) ref = rv.fischetti;
            else if (rv.ea4op >= 0) ref = rv.ea4op;

            
            if (rv.opt >= 0)
            {
                cout << left << setw(25) << "Optimum"
                     << setw(10) << rv.opt << "\n";
            }
            if (rv.gls >= 0)
            {
                cout << left << setw(25) << "Vansteenwegen GLS (2009)"
                     << setw(10) << rv.gls << "\n";
            }
            if(rv.chao >= 0)
            {
                cout << left << setw(25) << "Chao OH (1996)"
                     << setw(10) << rv.chao << "\n";
            }
            if(rv.fischetti >= 0)
            {
                cout << left << setw(25) << "Fischetti B&C (1998)"
                     << setw(10) << rv.fischetti << "\n";
            }
            if(rv.ea4op >= 0)
            {
                cout << left << setw(25) << "ea4op (OPLib)"
                     << setw(10) << rv.ea4op << "\n";
            }

             cout << string(60, '-') << "\n";
             
             cout << "Ulozit do CSV? (a/n): ";
             string saveChoice;
             getline(cin, saveChoice);
             if(saveChoice == "a" || saveChoice == "A"){
                 Result r;
                 r.instanceKey  = key;
                 r.algorithm    = algoName;
                 r.tMax         = inst.tMax;
                 r.profit       = improvedSol.totalScore(inst);
                 r.distance     = improvedSol.totalDistance(inst);
                 r.refOpt       = rv.opt;
                 r.refGls       = rv.gls;
                 r.refChao      = rv.chao;
                 r.refFischetti = rv.fischetti;
                 r.refEa4op     = rv.ea4op;
                 r.cpu_ms       = ms;
                 r.gap_percent  = computeGap(r.profit, ref);
                 r.gap_dist_percent = computeDistanceGap(r.distance, solGreedy.totalDistance(inst));
                    
                 ResultList results = {r};
                 string csvName = generateCSVFilename(algoName, key);
                 saveResultCSV(results, csvName);
             }
            
        }
        else if (choice == 6) {
            cout << "Zakladny adresat (Enter pre aktualny adresar): ";
            string baseDir;
            getline(cin, baseDir);
            if (baseDir.empty()) baseDir = ".";

            vector<pair<string, AlgoFunc>> algos = {
                {"Greedy", [](const Instance& inst){
                    vector<int> u; return greedyInsert(inst, u);}},
                {"2-Opt",   [](const Instance& inst) {
                    vector<int> u; return twoOpt(inst, greedyInsert(inst, u)); }},
                {"Or-Opt",  [](const Instance& inst) {
                    vector<int> u; return OrOpt(inst, greedyInsert(inst, u)); }},
                {"Swap",    [](const Instance& inst) {
                    vector<int> u; return Swap(inst, greedyInsert(inst, u)); }},
                {"Replace", [](const Instance& inst) {
                    vector<int> u;
                    Solution s = greedyInsert(inst, u);
                    return Replace(inst, s, u); }},
            };

            ResultList allResults;

            for (auto& [algoName, algoFunc] : algos) {
                //Vsetko do jedneho CSV, nazov bude napr. "Greedy_vsetky_instancie_20240615_153045.csv", vsetky instancie jedneho algoritmu
                cout << "\n--- " << algoName << " ---\n";
                ResultList results = runAll(baseDir, algoName, algoFunc);
                printResults(results);
                string csvName = generateCSVFilename(algoName, "vsetky_instancie");
                saveResultCSV(results, csvName);

                allResults.insert(allResults.end(), results.begin(), results.end());
            }
            saveResultCSV(allResults, generateCSVFilename("vsetky_algoritmy", "vsetky_instancie"));
            saveSummaryCSV(allResults, generateCSVFilename("vsetky_algoritmy", "suhrna"));
            cout << "\nHotovo. CSV subory ulozene.\n";
        }
        else if (choice == 7){
            cout << "Vyber algoritmus:\n";
            cout << "1. Greedy\n2. 2-Opt\n3. Or-Opt\n4. Swap\n5. Replace\n";
            cout << "Volba: ";
            int algoChoice; cin >> algoChoice; cin.ignore();

            cout << "Zakladny adresar (Enter = aktualny): ";
            string base; getline(cin, base);
            if (base.empty()) base = ".";

            string algoName;
            AlgoFunc algoFunc;

            if (algoChoice == 1) { 
                algoName = "Greedy";  
                algoFunc = [](const Instance& inst) { 
                    vector<int> u; 
                    return greedyInsert(inst, u); 
                };
            }
            else if (algoChoice == 2) { 
                algoName = "2-Opt";   
                algoFunc = [](const Instance& inst) { 
                    vector<int> u; 
                    return twoOpt(inst, greedyInsert(inst,u)); 
                }; 
            }
            else if (algoChoice == 3) { 
                algoName = "Or-Opt";  
                algoFunc = [](const Instance& inst) { 
                    vector<int> u; 
                    return OrOpt(inst, greedyInsert(inst,u)); 
                }; 
            }
            else if (algoChoice == 4) { 
                algoName = "Swap";    
                algoFunc = [](const Instance& inst) { 
                    vector<int> u; 
                    return Swap(inst, greedyInsert(inst,u)); 
                }; 
            }
            else if (algoChoice == 5) {
                 algoName = "Replace"; 
                 algoFunc = [](const Instance& inst) { 
                    vector<int> u; 
                    Solution s = greedyInsert(inst,u); 
                    return Replace(inst,s,u); }; }
            else { cout << "Neznama volba.\n"; continue; }

            ResultList results = runAll(base, algoName, algoFunc);
            printResults(results);
            string csvName = generateCSVFilename(algoName, "vsetky_instancie");
            saveResultCSV(results, csvName);
            cout << "Ulozene do: " << csvName << "\n";
        }
        else if (choice == 8){
            cout << "Zakladny adresar (Enter = aktualny): ";
            string base; 
            getline(cin, base);
            if (base.empty()) base = ".";

            vector<pair<string, AlgoFunc>> algos = {
                {"Greedy",  greedyInsert},
                {"2-Opt",   [](const Instance& inst) {
                    vector<int> u; return twoOpt(inst, greedyInsert(inst, u)); }},
                {"Or-Opt",  [](const Instance& inst) {
                    vector<int> u; return OrOpt(inst, greedyInsert(inst, u)); }},
                {"Swap",    [](const Instance& inst) {
                    vector<int> u; return Swap(inst, greedyInsert(inst, u)); }},
                {"Replace", [](const Instance& inst) {
                    vector<int> u;
                    Solution s = greedyInsert(inst, u);
                    return Replace(inst, s, u); }},
            };

            ResultList selectedResults;
            vector<string> sets = {
            "Tsiligirides 1",
            "Tsiligirides 2",
            "Tsiligirides 3",
            "set_64_1",
            "set_66_1",
            "OPLib-selected"
            };

            for (auto& [algoName, algoFunc] : algos) {
                cout << "\n--- " << algoName << " ---\n";

                ResultList allResults;

                for (const string& set : sets) {
                    string dataDir = base + "/" + set;

                    if(!fs::exists(dataDir)) {
                        cerr << "Chyba: Adresar neexistuje " << dataDir << endl;
                        continue;
                    }

                    cout << "\n[" << set << "]\n";

                    ResultList results = runSet(dataDir, algoName, algoFunc);
                    allResults.insert(allResults.end(), results.begin(), results.end());
                }

                printResults(allResults);

                string csvName = generateCSVFilename(algoName, "vybrane_instancie");
                saveResultCSV(allResults, csvName);

                selectedResults.insert(selectedResults.end(), allResults.begin(), allResults.end());
            }

            saveResultCSV(selectedResults, generateCSVFilename("vsetky_algoritmy", "vybrane_instancie"));
            saveSummaryCSV(selectedResults, generateCSVFilename("suhrn_Santini", "vybrane_instancie"));
            cout << "\nHotovo. CSV subory ulozene.\n";
        }
        else if (choice == 9){
            // Kombinované algoritmy na vybraných inštanciách
            cout << "Zakladny adresar (Enter = aktualny): ";
            string base; 
            getline(cin, base);
            if (base.empty()) base = ".";

            vector<pair<string, AlgoFunc>> combinedAlgos = {
                {"2-Opt+Greedy", [](const Instance& inst) {
                    vector<int> u;
                    Solution s1 = greedyInsert(inst, u);
                    Solution s2 = twoOpt(inst, s1);
                    // Aktualizuj unvisited
                    vector<bool> visited(inst.n, false);
                    for(int v : s2.route) visited[v] = true;
                    u.clear();
                    for(int j = 1; j < inst.n - 1; ++j)
                        if(!visited[j]) u.push_back(j);
                    return greedyInsert(inst, s2, u);
                }},
                {"Or-Opt+Greedy", [](const Instance& inst) {
                    vector<int> u;
                    Solution s1 = greedyInsert(inst, u);
                    Solution s2 = OrOpt(inst, s1);
                    vector<bool> visited(inst.n, false);
                    for(int v : s2.route) visited[v] = true;
                    u.clear();
                    for(int j = 1; j < inst.n - 1; ++j)
                        if(!visited[j]) u.push_back(j);
                    return greedyInsert(inst, s2, u);
                }},
                {"Swap+Greedy", [](const Instance& inst) {
                    vector<int> u;
                    Solution s1 = greedyInsert(inst, u);
                    Solution s2 = Swap(inst, s1);
                    vector<bool> visited(inst.n, false);
                    for(int v : s2.route) visited[v] = true;
                    u.clear();
                    for(int j = 1; j < inst.n - 1; ++j)
                        if(!visited[j]) u.push_back(j);
                    return greedyInsert(inst, s2, u);
                }},
            };

            ResultList selectedResults;
            vector<string> sets = {
                "Tsiligirides 1", "Tsiligirides 2", "Tsiligirides 3",
                "set_64_1", "set_66_1", "OPLib-selected"
            };

            for (auto& [algoName, algoFunc] : combinedAlgos) {
                cout << "\n--- " << algoName << " ---\n";
                ResultList allResults;

                for (const string& set : sets) {
                    string dataDir = base + "/" + set;
                    if(!fs::exists(dataDir)) {
                        cerr << "Chyba: Adresar neexistuje " << dataDir << endl;
                        continue;
                    }
                    cout << "\n[" << set << "]\n";
                    ResultList results = runSet(dataDir, algoName, algoFunc);
                    allResults.insert(allResults.end(), results.begin(), results.end());
                }

                printResults(allResults);
                string csvName = generateCSVFilename(algoName, "vybrane_instancie");
                saveResultCSV(allResults, csvName);
                selectedResults.insert(selectedResults.end(), allResults.begin(), allResults.end());
            }

            saveResultCSV(selectedResults, generateCSVFilename("kombinovane_algoritmy", "vybrane_instancie"));
            saveSummaryCSV(selectedResults, generateCSVFilename("kombinovane_suhrn", "vybrane_instancie"));
            cout << "\nHotovo. CSV subory pre kombinovane algoritmy ulozene.\n";
        }
        else if (choice == 10){
            cout << "Zakladny adresar (Enter = aktualny): ";
            string base; 
            getline(cin, base);
            if (base.empty()) base = ".";

            vector<string> sets = {
                "Tsiligirides 1", "Tsiligirides 2", "Tsiligirides 3",
                "set_64_1", "set_66_1", "OPLib-selected"
            };

            // --- ZÁKLADNÉ ---
            vector<pair<string, AlgoFunc>> basicAlgos = {
                {"Greedy", greedyInsert},
                {"2-Opt", [](const Instance& inst) {
                    vector<int> u; return twoOpt(inst, greedyInsert(inst, u)); }},
                {"Or-Opt", [](const Instance& inst) {
                    vector<int> u; return OrOpt(inst, greedyInsert(inst, u)); }},
                {"Swap", [](const Instance& inst) {
                    vector<int> u; return Swap(inst, greedyInsert(inst, u)); }},
                {"Replace", [](const Instance& inst) {
                    vector<int> u;
                    Solution s = greedyInsert(inst, u);
                    return Replace(inst, s, u); }},
            };

            // --- KOMBINOVANÉ ---
            vector<pair<string, AlgoFunc>> combinedAlgos = {
                {"2-Opt+Greedy", [](const Instance& inst) {
                    vector<int> u;
                    Solution s1 = greedyInsert(inst, u);
                    Solution s2 = twoOpt(inst, s1);

                    vector<bool> visited(inst.n, false);
                    for(int v : s2.route) visited[v] = true;

                    u.clear();
                    for(int j = 1; j < inst.n - 1; ++j)
                        if(!visited[j]) u.push_back(j);

                    return greedyInsert(inst, s2, u);
                }},
                {"Or-Opt+Greedy", [](const Instance& inst) {
                    vector<int> u;
                    Solution s1 = greedyInsert(inst, u);
                    Solution s2 = OrOpt(inst, s1);

                    vector<bool> visited(inst.n, false);
                    for(int v : s2.route) visited[v] = true;

                    u.clear();
                    for(int j = 1; j < inst.n - 1; ++j)
                        if(!visited[j]) u.push_back(j);

                    return greedyInsert(inst, s2, u);
                }},
                {"Swap+Greedy", [](const Instance& inst) {
                    vector<int> u;
                    Solution s1 = greedyInsert(inst, u);
                    Solution s2 = Swap(inst, s1);

                    vector<bool> visited(inst.n, false);
                    for(int v : s2.route) visited[v] = true;

                    u.clear();
                    for(int j = 1; j < inst.n - 1; ++j)
                        if(!visited[j]) u.push_back(j);

                    return greedyInsert(inst, s2, u);
                }},
            };

            ResultList allResults;

            // --- spustenie všetkých algoritmov ---
            auto runGroup = [&](const vector<pair<string, AlgoFunc>>& algos){
                for (auto& [algoName, algoFunc] : algos) {
                    cout << "\n--- " << algoName << " ---\n";
                    ResultList groupResults;

                    for (const string& set : sets) {
                        string dataDir = base + "/" + set;
                        if(!fs::exists(dataDir)) {
                            cerr << "Chyba: Adresar neexistuje " << dataDir << endl;
                            continue;
                        }

                        cout << "[" << set << "]\n";
                        ResultList results = runSet(dataDir, algoName, algoFunc);
                        groupResults.insert(groupResults.end(), results.begin(), results.end());
                    }

                    printResults(groupResults);

                    allResults.insert(allResults.end(), groupResults.begin(), groupResults.end());
                }
            };

            runGroup(basicAlgos);
            runGroup(combinedAlgos);

            // --- výstupy ---
            string baseName = generateCSVFilename("ALL_ALGOS", "vybrane_instancie");

            saveResultCSV(allResults, baseName);
            saveSummaryCSV(allResults, baseName);

            cout << "\n Hotovo: vsetko v jednom CSV + summary.\n";

        }
        else if (choice == 14) {
            cout << "\n--- HROMADNE SPRACOVANIE ZSK (Priklady_BP.txt) ---\n";
            
            Instance baseInst;
            // Načítanie základnej matice a profitov
            if (!baseInst.loadRescueData("RZZSSR/Matica.txt", "RZZSSR/Profit.txt")) {
                cout << "Chyba: Nepodarilo sa nacitat zakladne data (Matica/Profit)!\n";
            } else {
                ifstream examples("RZZSSR/Priklady_BP.txt");
                if (!examples.is_open()) {
                    cout << "Chyba: Subor Priklady_BP.txt nenajdeny!\n";
                } else {
                    ResultList rescueResults;
                    string line;
                    
                    cout << left << setw(15) << "Start" << setw(15) << "Ciel" 
                         << setw(10) << "Limit" << setw(10) << "Zisk" << "Trasa" << endl;
                    cout << string(80, '-') << endl;

                    while (getline(examples, line)) {
                        if (line.empty()) continue;

                        stringstream ss(line);
                        string part;
                        int sID, eID;
                        double limit;

                        try {
                            // Formát: 2645;Zilina;2627;Bytca;12.2;0
                            getline(ss, part, ';'); sID = stoi(part);
                            getline(ss, part, ';'); // skip meno startu
                            getline(ss, part, ';'); eID = stoi(part);
                            getline(ss, part, ';'); // skip meno ciela
                            getline(ss, part, ';'); limit = stod(part);
                        } catch (...) {
                            continue;
                        }

                        if (baseInst.idToIndex.find(sID) == baseInst.idToIndex.end() || 
                            baseInst.idToIndex.find(eID) == baseInst.idToIndex.end()) {
                            continue;
                        }

                        // 1. Kópia základných dát
                        Instance currentInst = baseInst;
                        setupRescueScenario(currentInst, sID, eID, limit);

                        auto t0 = chrono::high_resolution_clock::now();
                        
                        // 2. Algoritmy
                        vector<int> unvisitedForReplace; 
                        unvisitedForReplace.clear();
                        //cout << "DEBUG: Max index v unvisited: " << unvisited.back() << ", Size of nodes: " << currentInst.nodes.size() << endl;
                        // Nechaj Greedy, nech si sám naplní unvisited
                        Solution solGreedy = greedyInsert(currentInst, unvisitedForReplace);
                        // Spusti Replace s tým, čo zostalo v unvisited
                        Solution solFinal = Replace(currentInst, solGreedy, unvisitedForReplace);

                        auto t1 = chrono::high_resolution_clock::now();
                        double ms = chrono::duration<double, milli>(t1 - t0).count();

                        // 3. Uloženie výsledku
                        Result r;
                        r.instanceKey = to_string(sID) + "_" + to_string(eID);
                        r.algorithm = "Replace_Rescue";
                        r.tMax = limit;
                        r.profit = solFinal.totalScore(currentInst);
                        r.distance = solFinal.totalDistance(currentInst);
                        r.cpu_ms = ms;
                        r.refOpt = r.refGls = r.refChao = r.refFischetti = r.refEa4op = -1;
                        r.gap_percent = -1;
                        r.gap_dist_percent = -1;

                        rescueResults.push_back(r);

                        // 4. Výpis do konzoly
                        cout << left << setw(15) << sID 
                             << setw(15) << eID 
                             << setw(10) << limit 
                             << setw(10) << r.profit;
                        
                        for (int idx : solFinal.route){
                            if (idx < 0 || idx >= (int)currentInst.nodes.size()){
                                cout << "ERROR: Neplatny index v trase: " << idx << ". Preskakujem tento index." << endl;
                                continue;
                            }else {
                                cout << currentInst.nodes[idx].id << " -> ";
                            }
                        } 
                        cout << "KONIEC" << endl;
                        cout << "DEBUG: Koniec vypisu trasy. Idem na koniec bloku." << endl;
                    }
                    cout << "DEBUG: Cyklus skoncil. Idem zavret subor." << endl;
                    examples.close();
                    cout << "DEBUG: Idem volat saveResultCSV." << endl;
                    if (!rescueResults.empty()) {
                        saveResultCSV(rescueResults, "Vysledky_ZSK_Prakticke.csv");
                        cout << "\n--- HOTOVO. Vysledky ulozene. ---\n";
                    }
                }
            }
            cout << "\nStlacte Enter pre navrat do menu...";
            cin.ignore();
        }
        else if (choice != 0) {
            cout << "Neplatna volba. Skuste znova.\n";
        }
    }
    cout << "Koniec programu. Dovidenia!\n";
    return 0;
}