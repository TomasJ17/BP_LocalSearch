#pragma once

#include <map>
#include <string>

// Referenčné hodnoty pre Set 1, 2, 3 z dvoch zdrojov:
//
//   TSILI = Tsiligirides (1984) – max zo 4 algoritmov: D, S, D(R-I), S(R-I)
//           Journal of the Operational Research Society 35(9), 797-809
//           CPU čas nie je uvedený
//
//   CHAO  = Chao, Golden, Wasil (1996b) – kolumna OH
//           European Journal of Operational Research 88(3), 475-489
//           CPU čas v sekundách na SUN 4/370 (1996)
//
// POZOR: obaja používali tMax_total = tMax_subor * m
// Náš algoritmus používa tMax_subor (per-vehicle)
//
// Kľúč: "1.2.c", "2.3.b", "3.4.t" (bez "p" a bez ".txt")

struct RefValues {
    int    tsili;     // Tsiligirides (1984) – best of D/S/D(R-I)/S(R-I)
    int    chao;      // Chao OH (1996)
    double cpuChao;   // CPU čas Chao v sekundách na SUN 4/370 (-1 ak neznámy)
};

inline std::map<std::string, RefValues> buildReferenceMap() {
    std::map<std::string, RefValues> ref;

    const char* L1[] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r"};
    const char* L2[] = {"a","b","c","d","e","f","g","h","i","j","k"};
    const char* L3[] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t"};

    // Set 1 (32 vrcholov)
    const int    ts1[]  = {10,15,45,65,90,110,135,150,175,190,205,220,240,260,265,275,280,285};
    const int    ch1[]  = {10,15,45,65,90,110,135,155,175,190,205,225,240,260,265,270,280,285};
    const double cpu1[] = {0.67,0.80,2.28,17.49,9.01,31.92,25.25,16.76,21.58,24.91,
                           24.67,24.28,23.26,25.09,25.24,28.53,26.84,21.71};
    for (int i = 0; i < 18; ++i)
        for (int m : {2, 3, 4})
            ref["1." + std::to_string(m) + "." + L1[i]] = {ts1[i], ch1[i], cpu1[i]};

    // Set 2 (21 vrcholov)
    const int    ts2[]  = {120,200,210,230,230,265,300,320,355,395,450};
    const int    ch2[]  = {120,200,210,230,230,265,300,320,360,395,450};
    const double cpu2[] = {1.29,2.24,4.45,5.65,6.37,6.18,7.21,7.81,6.84,7.14,0.61};
    for (int i = 0; i < 11; ++i)
        for (int m : {2, 3, 4})
            ref["2." + std::to_string(m) + "." + L2[i]] = {ts2[i], ch2[i], cpu2[i]};

    // Set 3 (33 vrcholov)
    const int    ts3[]  = {120,140,190,240,290,340,370,420,460,500,530,560,600,640,670,700,740,770,790,800};
    const int    ch3[]  = {170,200,260,320,390,430,470,520,550,580,610,640,670,710,740,770,790,800,800,800};
    const double cpu3[] = {4.37,5.16,9.40,9.96,15.38,18.65,26.84,28.74,30.27,27.68,
                           25.02,29.82,29.25,30.14,28.30,24.43,22.33,0.67,0.60,0.72};
    for (int i = 0; i < 20; ++i)
        for (int m : {2, 3, 4})
            ref["3." + std::to_string(m) + "." + L3[i]] = {ts3[i], ch3[i], cpu3[i]};

    return ref;
}

inline const std::map<std::string, RefValues>& getReferenceData() {
    static const std::map<std::string, RefValues> ref = buildReferenceMap();
    return ref;
}

inline RefValues getReference(const std::string& key) {
    const auto& ref = getReferenceData();
    auto it = ref.find(key);
    if (it != ref.end()) return it->second;
    return {-1, -1, -1.0};
}