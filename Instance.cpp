#include "Instance.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

const Node& Instance::start() const {
    return nodes.front();
}

const Node& Instance::end() const {
    return nodes.back();
}

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

        if      (token == "n")    iss >> inst.n;
        else if (token == "m")    iss >> inst.m;
        else if (token == "tmax") iss >> inst.tMax;
        else {
            Node node;
            node.index = (int)inst.nodes.size();
            node.x     = std::stod(token);
            iss >> node.y >> node.score;
            inst.nodes.push_back(node);
        }
    }

    return inst;
}