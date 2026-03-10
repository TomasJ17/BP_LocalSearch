#include "Result.h"
#include <fstream>
#include <iostream>
#include <iomanip>

using namespace std;

void saveResultCSV(const ResultList& results, const string& filename) {
    ofstream f(filename);
    if (!f.is_open()) {
        cerr << "Chyba: Súbor sa nedá otvoriť " << filename << endl;
        return;
    }
    // Hlavička CSV
    f << "Inštancia;Algoritmus;tMax;m;Profit;Vzdialenosť;Ref_Tsili;Ref_Chao;CPU_Chao;CPU_ms\n";
    for (const Result& r : results) {
        f << r.instanceKey << ";"
          << r.algorithm   << ";"
          << r.tMax       << ";"
          << r.m          << ";"
          << r.profit     << ";"
          << fixed << setprecision(4) << r.distance << ";"
          << r.refTsili   << ";"
          << r.refChao    << ";"
          << fixed << setprecision(2) << r.cpuChao << ";"
          << fixed << setprecision(2) << r.cpu_ms
          << "\n";
    }
    cout << "Výsledky uložené do " << filename << endl;
    f.close();
}

void printResults(const ResultList& results) {
    cout << "\n";
    cout << left
         << setw(12) << "Instancia"
         << setw(12) << "tMax"
         << setw(5)  << "m"
         << setw(10) << "Profit"
         << setw(12) << "Vzd."
         << setw(10) << "Tsili"
         << setw(10) << "Chao"
         << setw(12) << "CPU_Chao"
         << setw(10) << "CPU(ms)"
         << "\n";
    cout << string(90, '-') << "\n";

    for (const Result& r : results) {
        string ts  = (r.refTsili >= 0) ? to_string(r.refTsili) : "---";
        string ch  = (r.refChao  >= 0) ? to_string(r.refChao)  : "---";
        string cpu = (r.cpuChao  >= 0) ? to_string(r.cpuChao).substr(0, 4) + "s" : "---";

        cout << left
             << setw(12) << r.instanceKey
             << fixed << setprecision(1)
             << setw(12) << r.tMax
             << setw(5)  << r.m
             << setw(10) << r.profit
             << fixed << setprecision(4)
             << setw(12) << r.distance
             << setw(10) << ts
             << setw(10) << ch
             << setw(12) << cpu
             << fixed << setprecision(2)
             << setw(10) << r.cpu_ms
             << "\n";
    }

    cout << string(90, '-') << "\n";
    int total = 0;
    for (const Result& r : results) total += r.profit;
    cout << "Celkovy profit (Greedy): " << total << "\n";
}
