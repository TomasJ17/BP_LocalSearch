#include "Benchmark.h"
#include "Instance.h"
#include "Solution.h"
#include "ReferenceData.h"
#include <filesystem>
#include <chrono>
#include <iostream>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

// Z nazvu súboru získame kľúč napr "p1.2.c.txt" -> "1.2.c"
static string extractInstanceKey(const string& filepath) {
    // Najprv zobraz len nazov suboru bez cesty
    string key = filepath;
    size_t pos = key.find_last_of("/\\");
    if (pos != string::npos)
        key = key.substr(pos + 1);  // napr. "p2.2.a.txt"

    // Odstran 'p' na zaciatku
    if (!key.empty() && key[0] == 'p')
        key = key.substr(1);        // napr. "2.2.a.txt"

    // Odstran ".txt" na konci
    size_t dot = key.rfind(".txt");
    if (dot != string::npos)
        key = key.substr(0, dot);   // napr. "2.2.a"

    return key;
}

ResultList runSet(const string& dataDir, const string& algoName, AlgoFunc algo) {
    ResultList results;
    // Vektor pre všetky .txt súbory v adresári
    vector<string> files;
    for (const auto& entry : fs::directory_iterator(dataDir)) {
        if(entry.path().extension() == ".txt")
            files.push_back(entry.path().string());
    }
    // Zoradíme súbory podľa názvu (aby sme mali konzistentné poradie)
    sort(files.begin(), files.end());

    for (const auto& filepath : files) {
        // Načítame inštanciu zo súboru
        Instance inst = loadInstance(filepath);
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
        r.m          = inst.m;
        r.profit     = sol.totalScore(inst);
        r.distance   = sol.totalDistance(inst);
        r.refTsili   = rv.tsili;
        r.refChao    = rv.chao;
        r.cpu_ms     = cpu_ms;

        results.push_back(r);
    }

    return results;
}

ResultList runAll(const string& basePath, const string& algoName, AlgoFunc algo) {
    ResultList allResults;

    for (const string& set :  {"Set_32_234", "Set_21_234", "Set_33_234"}) {
        string dataDir = basePath + "/" + set;
        if(!fs::exists(dataDir)) {
            cerr << "Chyba: Adresár neexistuje " << dataDir << endl;
            continue;
        }
        ResultList setResults = runSet(dataDir, algoName, algo);
        allResults.insert(allResults.end(), setResults.begin(), setResults.end());
    }
    return allResults;
}

// Spustí greedy na jednej inštancii a zobrazí porovnanie
void runSingle(const string& filepath, const string& algoName, AlgoFunc algo) {
    Instance inst = loadInstance(filepath);

    cout << "\nSubor : " << inst.name << "\n";
    cout << "n     : " << inst.n    << "\n";
    cout << "m     : " << inst.m    << "\n";
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
         << setw(22) << algoName
         << setw(10) << sol.totalScore(inst)
         << fixed << setprecision(4)
         << setw(12) << sol.totalDistance(inst)
         << fixed << setprecision(2)
         << ms << "ms"
         << "\n";
    // Tsilligirides (1984) a Chao OH (1996) referenčné hodnoty
    if (rv.tsili >= 0)
        cout << left
             << setw(22) << "Tsiligirides (1984)"
             << setw(10) << rv.tsili
             << setw(12) << "---"
             << "N/A"
             << "\n";

    if (rv.chao >= 0)
        cout << left
             << setw(22) << "Chao OH (1996)"
             << setw(10) << rv.chao
             << setw(12) << "---"
             << fixed << setprecision(2) << rv.cpuChao << "s (SUN 4/370)"
             << "\n";

    cout << string(70, '-') << "\n";
    cout << "(Pozn.: ref. hodnoty su pre tMax_total = tMax * m = "
         << fixed << setprecision(1) << inst.tMax * inst.m << ")\n";
}