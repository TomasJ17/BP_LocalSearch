#pragma once

#include <vector>
#include <string>
#include <map>

// Typ výpočtu vzdialenosti
enum class EdgeWeightType{
    EUC_2D, // euklidovská vzdialenosť v 2D (eil51, eil76, eil101)
    ATT,     // pseudo-euklidovská ATT vzdialenosť (att48)
    MATRIX   // vzdialenosti načítané z matice (rescue)
};

// Jeden vrchol grafu (vrchol 0 = start, vrchol n-1 = end)
struct Node {
    int    index; // index vrcholu (0 až n-1), poradie v súbore
    double x;     // súradnica x  
    double y;     // súradnica y
    int    profit;// profit získaný návštěvou tohoto vrcholu
    int id; // názov vrcholu (napr. 2645 v Rescue formátu)
};

// Inštancia problému na základe vstupného súboru
struct Instance {
    std::string       name; // názov súboru (napr. "1.2.c.txt")
    int               n;    // počet vrcholov (vrcholy 0 až n-1)
    double            tMax;  // maximálny čas (dĺžka trasy) – per-vehicle, tMax_subor z inštancie
    EdgeWeightType   edgeWeightType; // typ výpočtu vzdialenosti (EUC_2D, ATT)
    std::vector<Node> nodes; // zoznam vrcholov (vrchol 0 = start, vrchol n-1 = end)

    std::vector<std::vector<double>> distMatrix; // len pre EdgeWeightType::MATRIX – načítané vzdialenosti
    std::map<int, int> idToIndex; // len pro Rescue format – mapovanie z ID (napr. 2645) na index (napr. 250)

    const Node& start() const; // vráti prvý vrchol (index 0) štartovací
    const Node& end()   const; // vráti posledný vrchol (index n-1) cielový
    double distance(int i, int j) const; // vráti euklidovskú vzdialenosť medzi vrcholy i a j

    std::map<int, int> rescueMatrixMap; // len pro Rescue format – mapovanie z ID (napr. 2645) na index v matici (napr. 0)
    bool loadRescueData(const std::string& matrixFile, const std::string& profitFile); // načíta dáta z Rescue formátu (Matica.txt a Profit.txt) a naplní inštanciu
};

Instance loadInstance(const std::string& filepath); // načíta súbor a vráti inštanciu