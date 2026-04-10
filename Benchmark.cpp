#include "Benchmark.h"
#include "Instance.h"
#include "Solution.h"
#include "ReferenceData.h"
#include "Greedy.h"
#include <filesystem>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;
namespace fs = std::filesystem;

static string extractInstanceKey(const string& filepath) {
    // Najprv zobraz len nazov suboru bez cesty
    string key = filepath;
    size_t pos = key.find_last_of("/\\");
    if (pos != string::npos)
        key = key.substr(pos + 1);  

    // OPLib: "att48-gen2-50.oplib" -> "att48"
    if(key.find(".oplib") != string::npos) {
        size_t genPos = key.find('-');
        if (genPos != string::npos)
            key = key.substr(0, genPos);
    }

    // Tsiligirides: "tsiligirides_problem_1_budget_05.txt" -> "tsili1_5"
    if(key.find("tsiligirides_problem_") != string::npos) {
        size_t probPos = key.find("problem_") + 8;
        int problem = std::stoi(key.substr(probPos, 1));
        size_t budgetPos = key.find("budget_") + 7;
        string budget = key.substr(budgetPos, key.find(".", budgetPos) - budgetPos);
        int tmax = std::stoi(budget);
        key = "tsili" + to_string(problem) + "_" + to_string(tmax);
    }

    // Chao 64: "set_64_1_15.txt -> "chao64_15"
    if (key.find("set_64_") != string::npos) {
        size_t last = key.rfind('_') + 1;
        string tmax = key.substr(last, key.find('.', last) - last);
        key = "chao64_" + std::to_string(std::stoi(tmax));
    }

    // Chao 66: "set_66_1_005.txt" -> "chao66_5"
    if (key.find("set_66_") != string::npos) {
        size_t last = key.rfind('_') + 1;
        string tmax = key.substr(last, key.find('.', last) - last);
        key = "chao66_" + std::to_string(std::stoi(tmax));
    }

    return key;
}

ResultList runSet(const string& dataDir, const string& algoName, AlgoFunc algo) {
    ResultList results;
    // Vektor pre všetky .txt súbory v adresári
    vector<string> files;
    for (const auto& entry : fs::directory_iterator(dataDir)) {
        //if (!entry.is_regular_file()) continue;

        string ext = entry.path().extension().string();
        transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".txt" || ext == ".oplib") {
            //cout << " -> pouzivam\n";
            files.push_back(entry.path().string());
        }
    }


    // Zoradíme súbory podľa názvu (aby sme mali konzistentné poradie)
    sort(files.begin(), files.end());

    for (const auto& filepath : files) {
        // Načítame inštanciu zo súboru
        Instance inst = loadInstance(filepath);

        // Referenčný greedy pre výpočet zlepšenia vzdialenosti
        vector<int> unvisited;
        Solution solGreedy = greedyInsert(inst, unvisited);
        double distGreedy = solGreedy.totalDistance(inst);

        //cout << "DEBUG: " << filepath 
        //    << " n=" << inst.n 
        //    << " nodes=" << inst.nodes.size() << "\n";

        // DEBUG
        /*if (inst.n != (int)inst.nodes.size()) {
        cerr << "CHYBA: n=" << inst.n << " ale nodes.size()=" << inst.nodes.size() 
             << " subor=" << filepath << "\n";
        continue;
        }*/
        string instanceKey = extractInstanceKey(fs::path(filepath).filename().string());

        // Spustíme algoritmus a zmeráme čas
        auto start = chrono::high_resolution_clock::now();
        Solution sol = algo(inst);
        auto end = chrono::high_resolution_clock::now();
        double cpu_ms = chrono::duration<double, milli>(end - start).count();

        RefValues rv = getReference(instanceKey);

        // Vypočítame výsledky
        Result r;
        r.instanceKey = instanceKey;
        r.algorithm   = algoName;
        r.tMax       = inst.tMax;
        r.profit     = sol.totalScore(inst);
        r.distance   = sol.totalDistance(inst);
        r.refOpt       = rv.opt;
        r.refGls      = rv.gls;
        r.refChao     = rv.chao;
        r.refFischetti = rv.fischetti;
        r.refEa4op     = rv.ea4op;
        r.cpu_ms     = cpu_ms;

        // Výpočet GAP (%)
        // GAP vyjadruje percentuálnu odchýlku nášho riešenia od referenčnej hodnoty.
        // Používame najlepšiu dostupnú referenciu v tomto poradí:
        // 1. optimálne riešenie (opt)
        // 2. GLS (Vansteenwegen et al. 2009)
        // 3. Chao (1996)
        // 4. Fischetti B&C (1998)
        // 5. ea4op (OPLib)
        //
        // Dôvod: nie všetky inštancie majú známe optimum, preto používame
        // najlepšiu dostupnú hodnotu z literatúry.
        //
        // Vzorec:
        // gap% = (ref - profit) / ref * 100
        //
        // Interpretácia:
        // gap = 0   -> dosiahnuté optimum / najlepšie známe riešenie
        // gap > 0   -> o koľko % je riešenie horšie
        // gap = -1  -> referenčná hodnota neexistuje

        
        int ref = -1;

        if (rv.opt >= 0) ref = rv.opt;
        else if (rv.gls >= 0) ref = rv.gls;
        else if (rv.chao >= 0) ref = rv.chao;
        else if (rv.fischetti >= 0) ref = rv.fischetti;
        else if (rv.ea4op >= 0) ref = rv.ea4op;
        r.gap_percent = computeGap(r.profit, ref);
        r.gap_dist_percent = computeDistanceGap(r.distance, distGreedy);

        results.push_back(r);
        cout << "Vysledok: " << instanceKey 
     << " profit=" << r.profit 
     << " dist=" << r.distance 
     << " gap=" << r.gap_percent 
     << " gap_dist=" << r.gap_dist_percent << "\n";
    }

    return results;
}

ResultList runAll(const string& basePath, const string& algoName, AlgoFunc algo) {
    ResultList allResults;

    for (const string& set :  {
        "Tsiligirides 1",
        "Tsiligirides 2",
        "Tsiligirides 3",
        "set_64_1",
        "set_66_1",
        //"OPLib-master/instances/gen2"
        "OPLib-selected"
    }) {
        string dataDir = basePath + "/" + set;
        if(!fs::exists(dataDir)) {
            cerr << "Chyba: Adresár neexistuje " << dataDir << endl;
            continue;
        }
        cout << "\n---" << set << "---\n";
        ResultList setResults = runSet(dataDir, algoName, algo);
        allResults.insert(allResults.end(), setResults.begin(), setResults.end());
    }
    return allResults;
}

ResultList runSelected(const string& basePath, const string& algoName, AlgoFunc algo) {
    ResultList allResults;

    for (const string& set :  {
        "Tsiligirides 1",
        "Tsiligirides 2",
        "Tsiligirides 3",
        "set_64_1",
        "set_66_1",
        //"OPLib-master/instances/gen2"
        "OPLib-selected"
    }) {
        string dataDir = basePath + "/" + set;
        if(!fs::exists(dataDir)) {
            cerr << "Chyba: Adresár neexistuje " << dataDir << endl;
            continue;
        }
        cout << "\n---" << set << "---\n";
        ResultList setResults = runSet(dataDir, algoName, algo);
        allResults.insert(allResults.end(), setResults.begin(), setResults.end());
    }
    return allResults;
}

ResultList runInstances(const vector<string>& files, const string& algoName, AlgoFunc algo) {
    ResultList results;

    for (const auto& filepath : files) {
        Instance inst = loadInstance(filepath);

        // Referenčný greedy pre výpočet zlepšenia vzdialenosti
        vector<int> unvisited;
        Solution solGreedy = greedyInsert(inst, unvisited);
        double distGreedy = solGreedy.totalDistance(inst);

        cout << "\n--- " << filepath << " ---\n";
        cout << "n=" << inst.n << " tMax=" << inst.tMax << "\n";

        auto start = chrono::high_resolution_clock::now();
        Solution sol = algo(inst);
        auto end = chrono::high_resolution_clock::now();

        double cpu_ms = chrono::duration<double, milli>(end - start).count();

        string key = extractInstanceKey(filepath);
        RefValues rv = getReference(key);

        Result r;
        r.instanceKey = key;
        r.algorithm = algoName;
        r.tMax = inst.tMax;
        r.profit = sol.totalScore(inst);
        r.distance = sol.totalDistance(inst);
        r.refOpt = rv.opt;
        r.refGls = rv.gls;
        r.refChao = rv.chao;
        r.refFischetti = rv.fischetti;
        r.refEa4op = rv.ea4op;
        r.cpu_ms = cpu_ms;

        int ref = -1;
        if (rv.opt >= 0) ref = rv.opt;
        else if (rv.gls >= 0) ref = rv.gls;
        else if (rv.chao >= 0) ref = rv.chao;
        else if (rv.fischetti >= 0) ref = rv.fischetti;
        else if (rv.ea4op >= 0) ref = rv.ea4op;

        r.gap_percent = computeGap(r.profit, ref);
        r.gap_dist_percent = computeDistanceGap(r.distance, distGreedy);

        results.push_back(r);

        cout << "profit=" << r.profit
             << " dist=" << r.distance
             << " gap=" << r.gap_percent
             << " gap_dist=" << r.gap_dist_percent
             << " time=" << cpu_ms << "ms\n";
    }

    return results;
}

// Spustí greedy na jednej inštancii a zobrazí porovnanie
void runSingle(const string& filepath, const string& algoName, AlgoFunc algo) {
    Instance inst = loadInstance(filepath);

    cout << "\nSubor : " << inst.name << "\n";
    cout << "n     : " << inst.n    << "\n";
    cout << "tMax  : " << inst.tMax << "\n";

    auto t1 = chrono::high_resolution_clock::now();
    Solution sol = algo(inst);
    auto t2 = chrono::high_resolution_clock::now();
    double ms = chrono::duration<double, milli>(t2 - t1).count();

    cout << "\nTrasa : ";
    for (int i = 0; i < (int)sol.route.size(); ++i) {
        if (i > 0) cout << " -> ";
        cout << sol.route[i];
    }
    cout << "\n";

    string key = extractInstanceKey(filepath);
    RefValues rv = getReference(key);

    cout << "\n";
    cout << left
         << setw(22) << "Algoritmus"
         << setw(10) << "Profit"
         << setw(12) << "Vzd."
         << setw(12) << "CPU"
         << "\n";
    cout << string(70, '-') << "\n";

    cout << left
         << setw(25) << algoName
         << setw(10) << sol.totalScore(inst)
         << fixed << setprecision(4)
         << setw(12) << sol.totalDistance(inst)
         << fixed << setprecision(2)
         << ms << "ms"
         << "\n";
    // Referencne hodnoty – zobrazujeme len tie ktore existuju
    if (rv.opt >= 0)
        cout << left << setw(25) << "Optimum"
             << setw(10) << rv.opt << "\n";

    if (rv.gls >= 0)
        cout << left << setw(25) << "Vansteenwegen GLS (2009)"
             << setw(10) << rv.gls << "\n";

    if (rv.chao >= 0)
        cout << left << setw(25) << "Chao OH (1996)"
             << setw(10) << rv.chao << "\n";

    if (rv.fischetti >= 0)
        cout << left << setw(25) << "Fischetti B&C (1998)"
             << setw(10) << rv.fischetti << "\n";

    if (rv.ea4op >= 0)
        cout << left << setw(25) << "ea4op (OPLib)"
             << setw(10) << rv.ea4op << "\n";

    cout << string(59, '-') << "\n";
    cout << "(Pozn.: ref. hodnoty su pre tMax_total = tMax = "
         << fixed << setprecision(1) << inst.tMax<< ")\n";
}