#pragma once

#include <vector>
#include <string>

// Jeden vrchol grafu (vrchol 0 = start, vrchol n-1 = end)
struct Node {
    int    index; // index vrcholu (0 až n-1), poradie v súbore
    double x;     // súradnica x  
    double y;     // súradnica y
    int    profit;// profit získaný návštěvou tohoto vrcholu
};

// Inštancia problému na základe vstupného súboru
struct Instance {
    std::string       name; // názov súboru (napr. "1.2.c.txt")
    int               n;    // počet vrcholov (vrcholy 0 až n-1)
    int               m;    // počet vozidiel (u nas len 1)
    double            tMax;  // maximálny čas (dĺžka trasy) – per-vehicle, tMax_subor z inštancie
    std::vector<Node> nodes; // zoznam vrcholov (vrchol 0 = start, vrchol n-1 = end)

    const Node& start() const; // vráti prvý vrchol (index 0) štartovací
    const Node& end()   const; // vráti posledný vrchol (index n-1) cielový
    double distance(int i, int j) const; // vráti euklidovskú vzdialenosť medzi vrcholy i a j
};

Instance loadInstance(const std::string& filepath); // načíta súbor a vráti inštanciu