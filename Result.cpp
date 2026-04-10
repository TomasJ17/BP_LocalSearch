#include "Result.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>

using namespace std;

void saveResultCSV(const ResultList& results, const string& filename) {
    ofstream f(filename);
    if (!f.is_open()) {
        cerr << "Chyba: Súbor sa nedá otvoriť " << filename << endl;
        return;
    }
    // Hlavička CSV
    f << "Instancia;Algoritmus;tMax;Profit;Vzdialenost;"
      << "OPT;GLS;Chao;Fischetti;ea4op;CPU_ms;Gap_profit(%);Gap_distance(%)\n";

    for (const Result& r : results) {
        auto ref = [](int v) { return v >= 0 ? to_string(v) : "-1"; };
        f << r.instanceKey  << ";"
          << r.algorithm    << ";"
          << r.tMax         << ";"
          << r.profit       << ";"
          << fixed << setprecision(2) << r.distance << ";"
          << ref(r.refOpt)       << ";"
          << ref(r.refGls)       << ";"
          << ref(r.refChao)      << ";"
          << ref(r.refFischetti) << ";"
          << ref(r.refEa4op)     << ";"
          << fixed << setprecision(2) << r.cpu_ms << ";"
          << fixed << setprecision(2) << r.gap_percent << ";"
          << fixed << setprecision(2) << r.gap_dist_percent

          << "\n";
    }

    cout << "Výsledky uložené do " << filename << endl;
    f.close();
}

void printResults(const ResultList& results) {
    cout << "\n";
    cout << left
         << setw(15) << "Instancia"
         << setw(10) << "tMax"
         << setw(10) << "Profit"
         << setw(12) << "Vzd."
         << setw(8)  << "OPT"
         << setw(8)  << "GLS"
         << setw(8)  << "Chao"
         << setw(10) << "Fischetti"
         << setw(8)  << "ea4op"
         << setw(10) << "CPU(ms)"
         << setw(10) << "Gap_profit(%)"
         << setw(10) << "Gap_distance(%)"
         << "\n";
    cout << string(99, '-') << "\n";

    for (const Result& r : results) {
        auto ref = [](int v) -> string {
            return v >= 0 ? to_string(v) : "---";
        };

        cout << left
             << setw(15) << r.instanceKey
             << fixed << setprecision(2)
             << setw(10) << r.tMax
             << setw(10) << r.profit
             << fixed << setprecision(2)
             << setw(12) << r.distance
             << setw(8)  << ref(r.refOpt)
             << setw(8)  << ref(r.refGls)
             << setw(8)  << ref(r.refChao)
             << setw(10) << ref(r.refFischetti)
             << setw(8)  << ref(r.refEa4op)
             << fixed << setprecision(2)
             << setw(10) << r.cpu_ms
             << fixed << setprecision(2)
             << setw(10) << r.gap_percent
             << setw(10) << r.gap_dist_percent
             << "\n";
    }

    cout << string(99, '-') << "\n";
    int total = 0;
    for (const Result& r : results) total += r.profit;
    cout << "Celkovy profit: " << total << "\n";

    double avgGap = 0.0;
    int countWithRef = 0;
    for (const Result& r : results) {
        if (r.gap_percent >= 0) {
            avgGap += r.gap_percent;
            countWithRef++;
        }
    }
    if (countWithRef > 0) {
        avgGap /= countWithRef;
        cout << "Priemerny gap% (pre " << countWithRef << " instancii s referenciou): "
             << fixed << setprecision(2) << avgGap << "%\n";
    }
}


// Gap% = percentuálna odchylka naseho riesenia od najlepsieho znameho riesenia
// gap% = (OPT - profit) / OPT * 100
// kde OPT je:
//   - pre Tsiligirides sady: optimalne riesenie (Vansteenwegen et al. 2009, Tab.3)
//   - pre Chao sady: najlepsie znamy heuristicky vysledok (Chao et al. 1996b)
//   - pre TSPLIB gen2: optimalne riesenie branch-and-cut (Fischetti et al. 1998, Tab.IV)
// Zdroj vzorca: Vansteenwegen et al. (2009), Tab.3, stlpec Gap;
//               Santini (2019), Tab.4, stlpec gap
// gap% = 0 znamena ze sme dosiahli optimum
// gap% > 0 znamena ze sme od optima o tolko percent
double computeGap(int profit, int opt) {
    if (opt <= 0) return -1.0;  // optimum nezname
    return (opt - profit) / (double)opt * 100.0;
}

double computeDistanceGap(double distance, double greedyDistance) {
    if (greedyDistance <= 0) return -1.0;  // greedy vzdialenost nezname
    return (greedyDistance - distance) / greedyDistance * 100.0;
}

void saveSummaryCSV(const ResultList& results, const string& filename) {
    auto getSada = [](const string& key) -> string {
        if (key.find("tsili1") != string::npos) return "Tsiligirides1";
        if (key.find("tsili2") != string::npos) return "Tsiligirides2";
        if (key.find("tsili3") != string::npos) return "Tsiligirides3";
        if (key.find("chao64") != string::npos) return "Chao64";
        if (key.find("chao66") != string::npos) return "Chao66";
        return "TSPLIB";
    };

    struct Stats {
        vector<double> pGaps;    // Profit Gaps
        vector<double> dGaps;    // Distance Gaps (Skracovanie)
        vector<double> times;    // CPU časy
        vector<int> profits;     // Absolútny profit
    };

    map<pair<string, string>, Stats> storage;
    for (const Result& r : results) {
        string sName = getSada(r.instanceKey);
        auto key = make_pair(r.algorithm, sName);
        if (r.gap_percent >= 0) storage[key].pGaps.push_back(r.gap_percent);
        if (r.gap_dist_percent != -1.0) storage[key].dGaps.push_back(r.gap_dist_percent);
        storage[key].times.push_back(r.cpu_ms);
        storage[key].profits.push_back(r.profit);
    }

    vector<string> sady = {"Tsiligirides1", "Tsiligirides2", "Tsiligirides3", "Chao64", "Chao66", "TSPLIB"};
    // Pre Kobeaga tabuľky filtrujeme len skracovacie + finálny Replace
    vector<string> kobeAlgos = {"2-Opt", "2-Opt+Greedy", "Or-Opt", "Or-Opt+Greedy", "Swap","Swap+Greedy", "Replace"};
    vector<string> santiniAlgos = {"Greedy", "2-Opt", "Or-Opt", "Swap", "Replace"};

    // --- 1. SANTINI TABUĽKA (Pôvodná horizontálna verzia) ---
    string santiniFile = "SANTINI_" + filename;
    ofstream fs(santiniFile);
    if (fs.is_open()) {
        fs << "Algoritmus";
        for (const string& s : sady) fs << ";" << s << "_AvgGap%;" << s << "_AvgCas_ms";
        fs << "\n";
        for (const string& algo : santiniAlgos) {
            fs << algo;
            for (const string& s : sady) {
                auto& st = storage[{algo, s}];
                if (st.pGaps.empty()) fs << ";---"; else {
                    double sum = 0; for (double v : st.pGaps) sum += v;
                    fs << ";" << fixed << setprecision(2) << sum / st.pGaps.size();
                }
                if (st.times.empty()) fs << ";---"; else {
                    double sum = 0; for (double v : st.times) sum += v;
                    fs << ";" << fixed << setprecision(2) << sum / st.times.size();
                }
            }
            fs << "\n";
        }
        fs.close();
    }

    // --- 2. KOBEAGA TABUĽKY (3 špecifické porovnania) ---
    string kobeagaFile = "KOBEAGA_" + filename;
    ofstream fk(kobeagaFile);
    if (fk.is_open()) {
        // TABUĽKA 1: Profit a Čas (Gap na profit)
        fk << "TABULKA 1: PROFIT A CAS (GAP NA PROFIT)\n";
        fk << "Algoritmus"; for (const string& s : sady) fk << ";" << s << "_AvgProfitGap%;" << s << "_AvgTime_ms";
        fk << "\n";
        for (const string& algo : kobeAlgos) {
            fk << algo;
            for (const string& s : sady) {
                auto& st = storage[{algo, s}];
                if (st.pGaps.empty()) fk << ";---;---"; else {
                    double sumG = 0, sumT = 0;
                    for (double g : st.pGaps) sumG += g;
                    for (double t : st.times) sumT += t;
                    fk << ";" << fixed << setprecision(2) << sumG / st.pGaps.size() << ";" << sumT / st.times.size();
                }
            }
            fk << "\n";
        }

        // TABUĽKA 2: Vzdialenosť a Čas (Gap na vzdialenosť oproti Greedy)
        fk << "\nTABULKA 2: VZDIALENOST A CAS (GAP NA VZDIALENOST)\n";
        fk << "Algoritmus"; for (const string& s : sady) fk << ";" << s << "_AvgDistGap%;" << s << "_AvgTime_ms";
        fk << "\n";
        for (const string& algo : kobeAlgos) {
            fk << algo;
            for (const string& s : sady) {
                auto& st = storage[{algo, s}];
                if (st.dGaps.empty()) fk << ";---;---"; else {
                    double sumD = 0, sumT = 0;
                    for (double d : st.dGaps) sumD += d;
                    for (double t : st.times) sumT += t;
                    fk << ";" << fixed << setprecision(2) << sumD / st.dGaps.size() << ";" << sumT / st.times.size();
                }
            }
            fk << "\n";
        }

        // TABUĽKA 3: Profit a Čas po skrátení a vložení (Finálny výsledok)
        fk << "\nTABULKA 3: PROFIT A CELKOVY CAS (PO SKRATENI A VLOZENI)\n";
        fk << "Algoritmus"; for (const string& s : sady) fk << ";" << s << "_AvgProfitGap%;" << s << "_AvgTime_ms";
        fk << "\n";
        // Tu nás zaujíma hlavne Replace, ktorý reprezentuje ten proces "Skráť -> Vlož"
        for (const string& algo : kobeAlgos) {
            fk << algo;
            for (const string& s : sady) {
                auto& st = storage[{algo, s}];
                if (st.pGaps.empty()) fk << ";---;---"; else {
                    double sumG = 0, sumT = 0;
                    for (double g : st.pGaps) sumG += g;
                    for (double t : st.times) sumT += t;
                    fk << ";" << fixed << setprecision(2) << sumG / st.pGaps.size() << ";" << sumT / st.times.size();
                }
            }
            fk << "\n";
        }
        fk.close();
    }
    cout << "Hotovo. Santini v: " << santiniFile << ", Kobeaga v: " << kobeagaFile << endl;
}