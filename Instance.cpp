#include "Instance.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

const Node& Instance::start() const { return nodes.front(); }
const Node& Instance::end()   const { return nodes.back();  }

double Instance::distance(int i, int j) const {
    if (edgeWeightType == EdgeWeightType::MATRIX) {
        // 1. Získame ID uzlov na aktuálnych pozíciách i a j
        int idI = nodes[i].id;
        int idJ = nodes[j].id;

        // 2. Použijeme find() namiesto at(), aby sme predišli crashu pri chýbajúcom ID
        auto itI = rescueMatrixMap.find(idI);
        auto itJ = rescueMatrixMap.find(idJ);

        if (itI == rescueMatrixMap.end() || itJ == rescueMatrixMap.end()) {
            // Ak ID nie je v mape, vrátime veľkú vzdialenosť (penalta) 
            // a nevypisujeme do konzoly (kvôli rýchlosti)
            return 1e9; 
        }
        int idxI = itI->second;
        int idxJ = itJ->second;

        // TOTO JE TA KONTROLA:
        if (idxI >= (int)distMatrix.size() || idxJ >= (int)distMatrix.size() || 
            idxJ >= (int)distMatrix[idxI].size()) {
            return 1e9; 
        }

        // 3. Vrátime hodnotu z matice podľa pôvodných indexov
        return distMatrix[itI->second][itJ->second];
    }
    double dx = nodes[i].x - nodes[j].x;
    double dy = nodes[i].y - nodes[j].y;

    if (edgeWeightType == EdgeWeightType::ATT) {
        // Pseudo-euklidovska ATT vzdialenost (Padberg/Rinaldi)
        // Pouziva sa napr. pre att48
        // Formula: r = sqrt((dx^2 + dy^2) / 10), vysledok zaokruhli nahor
        double r = std::sqrt((dx * dx + dy * dy) / 10.0);
        int    t = (int)r;
        return (t < r) ? t + 1 : t;
    }

    // EUC_2D – standardna euklidovska vzdialenost
    return std::sqrt(dx * dx + dy * dy);
}

bool Instance::loadRescueData(const std::string& matrixFile, const std::string& profitFile) {
    this->nodes.clear();
    this->idToIndex.clear();
    this->rescueMatrixMap.clear();
    this->edgeWeightType = EdgeWeightType::MATRIX;
    
    // 1. Načítanie Profit.txt
    std::ifstream pFile(profitFile);
    if (!pFile.is_open()) return false;

    std::string line;
    int currentIdx = 0;
    while (std::getline(pFile, line)) {
        if (line.empty()) continue;
        // Odstránenie \r na konci (ak je súbor z Windows)
        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::stringstream ss(line);
        std::string idPart, profitPart, namePart;
        
        if (!std::getline(ss, idPart, ';')) continue;
        if (!std::getline(ss, profitPart, ';')) continue;

        Node n;
        n.index = currentIdx;
        n.profit = std::stoi(profitPart);
        int realID = std::stoi(idPart); 
        n.id = realID;
        n.x = 0; n.y = 0;

        this->idToIndex[realID] = currentIdx; 
        this->rescueMatrixMap[realID] = currentIdx; 
        this->nodes.push_back(n);
        currentIdx++;
    }
    this->n = (int)nodes.size();
    pFile.close();

    // 2. Načítanie Matica.txt 
    std::ifstream mFile(matrixFile);
    if (!mFile.is_open()) return false;

    this->distMatrix.clear();
    std::vector<double> allNumbers;
    double val;

    // Načítame úplne všetky čísla zo súboru do jedného veľkého vektora
    while (mFile >> val) {
        allNumbers.push_back(val);
    }
    mFile.close();

    // Teraz tie čísla rozdelíme do riadkov podľa toho, koľko máme uzlov (n)
    if (this->n > 0 && !allNumbers.empty()) {
        for (int i = 0; i < n; ++i) {
            std::vector<double> row;
            for (int j = 0; j < n; ++j) {
                int index = i * n + j;
                if (index < allNumbers.size()) {
                    row.push_back(allNumbers[index]);
                }
            }
            if (!row.empty()) this->distMatrix.push_back(row);
        }
    }

    // KONTROLNÝ VÝPIS - teraz tu musí byť 315 x 315
    std::cout << "DEBUG: Nacitanych uzlov: " << nodes.size() << std::endl;
    std::cout << "DEBUG: Rozmer matice: " << distMatrix.size() << " x " << (distMatrix.empty() ? 0 : distMatrix[0].size()) << std::endl;

    return true;
}


static bool endsWith(const std::string& s, const std::string& suffix) {
    return s.size() >= suffix.size() &&
           s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// ── Nacitanie OPLib formatu (.oplib) ──
static Instance loadOPLib(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Nepodarilo sa otvorit subor: " << filepath << "\n";
        exit(1);
    }

    Instance inst;
    inst.name            = filepath;
    inst.edgeWeightType  = EdgeWeightType::EUC_2D;
    inst.n               = 0;
    inst.tMax            = 0.0;

    std::string line;
    enum Section { HEADER, COORDS, SCORES, DEPOT } section = HEADER;

    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        if (line == "EOF") break;

        // Prechody medzi sekciami
        if (line == "NODE_COORD_SECTION") { section = COORDS; continue; }
        if (line == "NODE_SCORE_SECTION") { section = SCORES; continue; }
        if (line == "DEPOT_SECTION")      { section = DEPOT;  continue; }

        if (section == HEADER) {
            std::istringstream iss(line);
            std::string key, colon, value;
            iss >> key >> colon >> value;

            // Odstranime dvojbodku z kluca ak je priamo za nim (napr. "DIMENSION:")
            if (!key.empty() && key.back() == ':') {
                key.pop_back();
                value = colon;  // hodnota je v druhom tokene
            }

            //DEBUG
            //std::cerr << "HEADER: key='" << key << "' colon='" << colon << "' value='" << value << "'\n";

            if (key == "DIMENSION")
                inst.n = std::stoi(value);
            else if (key == "COST_LIMIT")
                inst.tMax = std::stod(value);
            else if (key == "EDGE_WEIGHT_TYPE") {
                if (value == "ATT")
                    inst.edgeWeightType = EdgeWeightType::ATT;
            }

            // Ked poznáme n, nastavíme velkost vektora
            if (inst.n > 0 && inst.nodes.empty())
                inst.nodes.resize(inst.n);
        }
        else if (section == COORDS) {
            std::istringstream iss(line);
            int idx;
            double x, y;
            iss >> idx >> x >> y;
            int i = idx - 1;  // OPLib indexuje od 1, my od 0
            if (i < 0 || i >= (int)inst.nodes.size()) {
                std::cerr << "CHYBA COORDS: idx=" << idx << " i=" << i 
                  << " nodes.size()=" << inst.nodes.size() 
                  << " n=" << inst.n << "\n";
                continue;
            }
            inst.nodes[i].index = i;
            inst.nodes[i].id    = idx; // OPLib ID je 1-based index
            inst.nodes[i].x     = x;
            inst.nodes[i].y     = y;
        }
        else if (section == SCORES) {
            std::istringstream iss(line);
            int idx, score;
            iss >> idx >> score;
            int i = idx - 1;
            inst.nodes[i].profit = score;
        }
        // DEPOT sekciu ignorujeme
    }
    // Start (vrchol 0) a ciel (vrchol n-1) maju profit 0
    if (!inst.nodes.empty()) {
        inst.nodes.front().profit = 0;
        inst.nodes.back().profit  = 0;
    }

    // Validacia Generation 2 formuly
    // p_j = 1 + (7141 * (j-1) + 73) mod 100, kde j je 1-based index
    // Start a ciel (index 0 a n-1) su vynechane
    bool gen2ok = true;
    for (int i = 1; i < inst.n - 1; ++i) {
        int expected = 1 + (7141 * i + 73) % 100;
        if (inst.nodes[i].profit != expected) {
            std::cerr << "VAROVANIE Gen2: node " << (i + 1)
                      << " profit=" << inst.nodes[i].profit
                      << " ocakavane=" << expected << "\n";
            gen2ok = false;
        }
    }
    if (gen2ok){
        //std::cout << "Gen2 validacia OK: " << filepath << "\n";
    }
    return inst;
}

// ── Nacitanie povodneho Tsiligirides formatu (.txt) ──
static Instance loadTXT(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Nepodarilo sa otvorit subor: " << filepath << "\n";
        exit(1);
    }

    Instance inst;
    inst.name           = filepath;
    inst.edgeWeightType = EdgeWeightType::EUC_2D;

    std::string line;
    bool firstLine = true;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (firstLine) {
            // Prvy riadok: "tMax\tm" – skus nacitat ako dve cisla
            // Ak prvy token je cislo (nie keyword), ide o Tsiligirides format
            try {
                inst.tMax = std::stod(token);
                int dummy; iss >> dummy;  // m – ignorujeme
                firstLine = false;
                continue;
            } catch (...) {
                // Nie je cislo – pokracuj ako keyword format
                firstLine = false;
            }
        }

        if      (token == "n")    iss >> inst.n;
        else if (token == "tmax") iss >> inst.tMax;
        else if (token == "m")    { int dummy; iss >> dummy; }  // ignorujeme m
        else {
            Node node;
            node.index  = (int)inst.nodes.size();
            node.x      = std::stod(token);
            iss >> node.y >> node.profit;
            node.id     = node.index; // OPLib ID je 1-based index
            inst.nodes.push_back(node);
        }
    }
    // n dopocitame z poctu nacitanych vrcholov, ak nebol zadany explicitne
    inst.n = (int)inst.nodes.size();

    // Detekcia Chao formatu: prvy aj druhy vrchol maju profit=0
    // V tom pripade je nodes[1] ciel (depot) – presunieme ho na koniec
    if (inst.n >= 2 &&
        inst.nodes[0].profit == 0 &&
        inst.nodes[1].profit == 0 &&
        inst.nodes[inst.n - 1].profit != 0) {

        // Vyberieme nodes[1] a dame ho na koniec
        Node endNode = inst.nodes[1];
        inst.nodes.erase(inst.nodes.begin() + 1);
        inst.nodes.push_back(endNode);

        // Preindexujeme
        for (int i = 0; i < inst.n; ++i)
            inst.nodes[i].index = i;
    }

    return inst;
}

// ── Hlavna funkcia – rozhodne podla pripony ──
Instance loadInstance(const std::string& filepath) {
    if (endsWith(filepath, ".oplib"))
        return loadOPLib(filepath);
    else
        return loadTXT(filepath);
}