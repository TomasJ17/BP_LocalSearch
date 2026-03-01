#pragma once

#include <vector>
#include <string>

struct Node {
    int    index;
    double x;
    double y;
    int    profit;
};

struct Instance {
    std::string       name;
    int               n;
    int               m;
    double            tMax;
    std::vector<Node> nodes;

    const Node& start() const;
    const Node& end()   const;
    double distance(int i, int j) const;
};

Instance loadInstance(const std::string& filepath);