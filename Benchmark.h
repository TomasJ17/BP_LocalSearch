#pragma once

#include "Result.h"
#include "Instance.h"
#include "Solution.h"
#include <string>

// Typ ukazovateľa na funkciu algoritmu, ktorá berie inštanciu a vracia riešenie
// Každý algoritmus (napr. Greedy, VNS, GA) musí mať tento podpis
//      Solution nazovFunkcie(const Instance& inst);
typedef Solution (*AlgoFunc)(const Instance&);

// Spustí algoritmus na všetkých inštanciách v jednom adresári
// napr. dataDir = "Set_32_234", algoName = "GreedyII"
ResultList runSet(const std::string& dataDir,
                  const std::string& algoName,
                  AlgoFunc algo);

// Spustí na všetkých 3 sadách naraz
// basePath = adresár kde ležia Set_32_234, Set_21_234, Set_33_234
ResultList runAll(const std::string& basePath,
                  const std::string& algoName,  
                  AlgoFunc algo);

// Spustí greedy na jednej inštancii a zobrazí porovnanie s referenciami
void runSingle(const std::string& filepath,
               const std::string& algoName,
               AlgoFunc algo);