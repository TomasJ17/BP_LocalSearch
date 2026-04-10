#pragma once

#include <string>
#include <vector>

struct Result {
    std::string instanceKey;  
    std::string algorithm;    // napr. "GreedyII"
    double      tMax;         // tMax z inštancie (per-vehicle)
    int         profit;       // dosiahnutý profit
    double      distance;     // celková dĺžka trasy
    int         refOpt;       // optimálne alebo najlepšie známe riešenie
    int         refGls;       // GLS (Fischetti et al. 1998) – len pre TSPLIB, inak -1
    int         refChao;      // Chao OH (1996) – len pre Tsiligirides sady, inak -1
    int         refFischetti; // Fischetti B&C (1998) – len pre TSPLIB, inak -1
    int         refEa4op;     // OPLib ea4op – len pre TSPLIB, inak -1
    double      cpu_ms;       // čas výpočtu v milisekundách
    double      gap_percent;  // výpočet gapu oproti referencii (napr. opt, GLS, Chao, Fischetti, ea4op) v percentách, profit gap
    double      gap_dist_percent; // výpočet gapu oproti referencii v percentách, ale počítaný z rozdílu vzdialeností místo profitů
};

typedef std::vector<Result> ResultList;

void saveResultCSV(const ResultList& results, const std::string& filename);
void printResults(const ResultList& results);
double computeGap(int profit, int opt);
double computeDistanceGap(double distance, double greedyDistance);
void saveSummaryCSV(const ResultList& results, const std::string& filename);