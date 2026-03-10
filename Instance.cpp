#include "Instance.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

// Prvý vrchol v zozname je vždy štartovací
const Node& Instance::start() const {
    return nodes.front();
}
// posledný je vždy cieľový
const Node& Instance::end() const {
    return nodes.back();
}
// Euklidovská vzdialenosť medzi vrcholmi i a j
double Instance::distance(int i, int j) const {
    double dx = nodes[i].x - nodes[j].x;
    double dy = nodes[i].y - nodes[j].y;
    return std::sqrt(dx * dx + dy * dy);
}

Instance loadInstance(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Nepodarilo sa otvorit subor: " << filepath << std::endl;
        exit(1);
    }

    Instance inst;
    inst.name = filepath;
    std::string line;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line.empty())
            continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        /*Formát vstupného súboru
            n 32
            m 2
            tmax 7.5
            10.50 14.40 0 // x y profit
            18.00 15.90 10
            ...
        */
        if      (token == "n")    iss >> inst.n;
        else if (token == "m")    iss >> inst.m;
        else if (token == "tmax") iss >> inst.tMax;
        else {
            // Riadok začína číslom = vrchol
            Node node;
            node.index = (int)inst.nodes.size(); // automaticke cislo vrcholu
            node.x     = std::stod(token); // prvy token je x
            iss >> node.y >> node.profit;
            inst.nodes.push_back(node);
        }
    }

    return inst;
}