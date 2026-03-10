#pragma once

#include <string>
#include <vector>

struct Result {
    std::string instanceKey;  // napr. "1.2.c"
    std::string algorithm;    // napr. "GreedyII"
    double      tMax;         // tMax z inštancie (per-vehicle)
    int         m;            // počet vozidiel
    int         profit;       // dosiahnutý profit
    double      distance;     // celková dĺžka trasy
    int         refTsili;     // Tsiligirides (1984) best
    int         refChao;      // Chao OH (1996) best
    double      cpuChao;      // CPU čas Chao v sekundách na SUN 4/370
    double      cpu_ms;       // čas výpočtu v milisekundách
};

typedef std::vector<Result> ResultList;

void saveResultCSV(const ResultList& results, const std::string& filename);
void printResults(const ResultList& results);