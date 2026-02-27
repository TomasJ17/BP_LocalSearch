#include <iostream>
#include "Instance.h"
#include "Solution.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Pouzitie: main <subor.txt>" << endl;
        return 1;
    }

    Instance inst = loadInstance(argv[1]);

    cout << "n    = " << inst.n    << endl;
    cout << "m    = " << inst.m    << endl;
    cout << "tMax = " << inst.tMax << endl;
    cout << endl;
    cout << "Index\tx\ty\tScore" << endl;
    cout << "-----\t-\t-\t-----" << endl;
    for (const Node& node : inst.nodes) {
        cout << node.index << "\t"
             << node.x     << "\t"
             << node.y     << "\t"
             << node.score << endl;
    }

    return 0;
}