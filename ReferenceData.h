#pragma once

#include <map>
#include <string>
// Referenčné hodnoty pre testovacie sady
//
// Tsiligirides Set 1,2,3 (m=1):
//   opt   = optimálne riešenie (Vansteenwegen et al., 2009, Table 3, stĺpec OPT)
//   gls   = výsledok GLS heuristiky (Vansteenwegen et al., 2009, Table 3, stĺpec GLS)
//   chao  = výsledok Chao OH heuristiky (Chao et al., 1996b, Tables 6-8)
//   fischetti = -1 (neexistuje pre Tsiligirides sady, len pro TSPLIB)
//   ea4op     = -1 (neexistuje pro Tsiligirides sady, len pro TSPLIB)
//
// TSPLIB Generation 2 (alpha=0.5):
//   opt       = Fischetti et al. (1998), Table IV, stlpec opt-val
//   gls       = -1 (netestovane)
//   chao      = -1 (netestovane)
//   fischetti = optimálne riešenie branch-and-cut (Fischetti et al., 1998, Table IV, stlpec opt-val)
//               profity podľa vzorca p_j = 1 + (7141*(j-1) + 73) mod 100
//   ea4op     = najlepšie známe heuristické riešenie (OPLib, ea4op algoritmus)
//
// Kľúč pre Tsiligirides: "tsili1_5", "tsili2_15", "tsili3_110" (set_tmax)
// Kľúč pre TSPLIB:       "att48", "eil51", "eil76", "rat99", "eil101"

struct RefValues {
    int opt;       // optimálne alebo najlepšie známe riešenie
    int gls;       // Vansteenwegen GLS
    int chao;      // Chao OH (1996) – len pre Tsiligirides sady, inak -1
    int fischetti; // Fischetti B&C  inak -1
    int ea4op;     // OPLib ea4op – len pre TSPLIB, inak -1
};

inline std::map<std::string, RefValues> buildReferenceMap() {
    std::map<std::string, RefValues> ref;

    // ── Tsiligirides Set 1 (n=32) – Vansteenwegen Tab.3 ──
    // Chao hodnoty z Chao et al. (1996b) Table 6
    // {opt, gls, chao, fischetti, ea4op}
    ref["tsili1_5"]  = {10,  10,  10,  -1, -1};
    ref["tsili1_10"] = {15,  15,  15,  -1, -1};
    ref["tsili1_15"] = {45,  45,  45,  -1, -1};
    ref["tsili1_20"] = {65,  55,  65,  -1, -1};
    ref["tsili1_25"] = {90,  90,  90,  -1, -1};
    ref["tsili1_30"] = {110, 80,  110, -1, -1};
    ref["tsili1_35"] = {135, 135, 135, -1, -1};
    ref["tsili1_40"] = {155, 145, 155, -1, -1};
    ref["tsili1_46"] = {175, 175, 175, -1, -1};
    ref["tsili1_50"] = {190, 180, 190, -1, -1};
    ref["tsili1_55"] = {205, 200, 205, -1, -1};
    ref["tsili1_60"] = {225, 220, 220, -1, -1};
    ref["tsili1_65"] = {240, 240, 240, -1, -1};
    ref["tsili1_70"] = {260, 260, 260, -1, -1};
    ref["tsili1_73"] = {265, 265, 265, -1, -1};
    ref["tsili1_75"] = {275, 270, 275, -1, -1};
    ref["tsili1_80"] = {280, 280, 280, -1, -1};
    ref["tsili1_85"] = {285, 285, 285, -1, -1};

    // ── Tsiligirides Set 2 (n=21) – Vansteenwegen Tab.3 ──
    ref["tsili2_15"] = {120, 120, 120, -1, -1};
    ref["tsili2_20"] = {200, 200, 200, -1, -1};
    ref["tsili2_23"] = {210, 210, 210, -1, -1};
    ref["tsili2_25"] = {230, 230, 230, -1, -1};
    ref["tsili2_27"] = {230, 220, 230, -1, -1};
    ref["tsili2_30"] = {265, 260, 265, -1, -1};
    ref["tsili2_32"] = {300, 300, 300, -1, -1};
    ref["tsili2_35"] = {320, 305, 320, -1, -1};
    ref["tsili2_38"] = {360, 360, 360, -1, -1};
    ref["tsili2_40"] = {395, 380, 395, -1, -1};
    ref["tsili2_45"] = {450, 450, 450, -1, -1};

    // ── Tsiligirides Set 3 (n=33) – Vansteenwegen Tab.3 ──
    ref["tsili3_15"]  = {170, 170, 170, -1, -1};
    ref["tsili3_20"]  = {200, 200, 200, -1, -1};
    ref["tsili3_25"]  = {260, 250, 260, -1, -1};
    ref["tsili3_30"]  = {320, 310, 320, -1, -1};
    ref["tsili3_35"]  = {390, 390, 390, -1, -1};
    ref["tsili3_40"]  = {430, 430, 430, -1, -1};
    ref["tsili3_45"]  = {470, 470, 470, -1, -1};
    ref["tsili3_50"]  = {520, 520, 520, -1, -1};
    ref["tsili3_55"]  = {550, 540, 550, -1, -1};
    ref["tsili3_60"]  = {580, 570, 580, -1, -1};
    ref["tsili3_65"]  = {610, 610, 610, -1, -1};
    ref["tsili3_70"]  = {640, 630, 640, -1, -1};
    ref["tsili3_75"]  = {670, 670, 670, -1, -1};
    ref["tsili3_80"]  = {710, 710, 710, -1, -1};
    ref["tsili3_85"]  = {740, 740, 740, -1, -1};
    ref["tsili3_90"]  = {770, 770, 770, -1, -1};
    ref["tsili3_95"]  = {790, 790, 790, -1, -1};
    ref["tsili3_100"] = {800, 800, 800, -1, -1};
    ref["tsili3_105"] = {800, 800, 800, -1, -1};
    ref["tsili3_110"] = {800, 800, 800, -1, -1};

     // ── Chao Diamond-shaped (n=64, set_64_1) – Chao et al. (1996b) Table 11 ──
    // gls = -1 (Vansteenwegen testoval ine tMax)
    // {opt, gls, chao, fischetti, ea4op}
    // Sillberholtz sady 5 a 6 su pre Chaa
   
    ref["chao64_15"] = {96,   -1, 96,   -1, -1};
    ref["chao64_20"] = {294,  -1, 294,  -1, -1};
    ref["chao64_25"] = {390,  -1, 390,  -1, -1};
    ref["chao64_30"] = {474,  -1, 474,  -1, -1};
    ref["chao64_35"] = {570,  -1, 570,  -1, -1};
    ref["chao64_40"] = {714,  -1, 714,  -1, -1};
    ref["chao64_45"] = {816,  -1, 816,  -1, -1};
    ref["chao64_50"] = {900,  -1, 900,  -1, -1};
    ref["chao64_55"] = {984,  -1, 984,  -1, -1};
    ref["chao64_60"] = {1062, -1, 1044, -1, -1};// BKS (Silberholtz, 2010: 1062) > Chao (1996: 1044)
    ref["chao64_65"] = {1116, -1, 1116, -1, -1};
    ref["chao64_70"] = {1188, -1, 1176, -1, -1}; // BKS (Silberholtz, 2010: 1188) > Chao (1996: 1176)
    ref["chao64_75"] = {1236, -1, 1224, -1, -1}; // BKS (Silberholtz, 2010: 1236) > Chao (1996: 1224)
    ref["chao64_80"] = {1284, -1, 1272, -1, -1}; // BKS (Silberholtz, 2010: 1284) > Chao (1996: 1272)

        // ── Chao Square-shaped (n=66, set_66_1) – Chao et al. (1996b) Table 10 ──
    ref["chao66_5"]   = {10,   -1, 10,   -1, -1};
    ref["chao66_10"]  = {40,   -1, 40,   -1, -1};
    ref["chao66_15"]  = {120,  -1, 120,  -1, -1};
    ref["chao66_20"]  = {205,  -1, 195,  -1, -1}; // (Silberholtz, 2010: 205) > Chao (1996: 195)
    ref["chao66_25"]  = {290,  -1, 290,  -1, -1};
    ref["chao66_30"]  = {400,  -1, 400,  -1, -1};
    ref["chao66_35"]  = {465,  -1, 460,  -1, -1}; // (Silberholtz, 2010: 465) > Chao (1996: 460)
    ref["chao66_40"]  = {575,  -1, 575,  -1, -1};
    ref["chao66_45"]  = {650,  -1, 650,  -1, -1};
    ref["chao66_50"]  = {730,  -1, 730,  -1, -1};
    ref["chao66_55"]  = {825,  -1, 825,  -1, -1};
    ref["chao66_60"]  = {915,  -1, 915,  -1, -1};
    ref["chao66_65"]  = {980,  -1, 980,  -1, -1};
    ref["chao66_70"]  = {1070, -1, 1070, -1, -1};
    ref["chao66_75"]  = {1140, -1, 1140, -1, -1};
    ref["chao66_80"]  = {1215, -1, 1215, -1, -1};
    ref["chao66_85"]  = {1270, -1, 1270, -1, -1};
    ref["chao66_90"]  = {1340, -1, 1340, -1, -1};
    ref["chao66_95"]  = {1395, -1, 1380, -1, -1}; // (Silberholtz, 2010: 1395) > Chao (1996: 1380)
    ref["chao66_100"] = {1465, -1, 1435, -1, -1}; // (Silberholtz, 2010: 1465) > Chao (1996: 1435)
    ref["chao66_105"] = {1520, -1, 1510, -1, -1}; // (Silberholtz, 2010: 1520) > Chao (1996: 1510)
    ref["chao66_110"] = {1560, -1, 1550, -1, -1}; // (Silberholtz, 2010: 1560) > Chao (1996: 1550)
    ref["chao66_115"] = {1595, -1, 1595, -1, -1};
    ref["chao66_120"] = {1635, -1, 1635, -1, -1};
    ref["chao66_125"] = {1670, -1, 1655, -1, -1}; // (Silberholtz, 2010: 1670) > Chao (1996: 1655)
    ref["chao66_130"] = {1680, -1, 1680, -1, -1};

    // ── TSPLIB Generation 2 (alpha=0.5) ──
    // fischetti = Fischetti et al. (1998) Table IV, opt-val
    // ea4op     = OPLib solutions/ea4op/gen2/*.sol, ROUTE_SCORE
    ref["att48"]  = {1717, -1, -1, 1717, 1717};
    ref["eil51"]  = {1674, -1, -1, 1674, 1668}; // OPLib ea4op má 1668, ale Fischetti B&C má 1674, takže dávame 1674 ako referenciu
    ref["eil76"]  = {2550, -1, -1, 2550, 2550};
    ref["rat99"]  = {2944, -1, -1, 2944, 2944};
    ref["eil101"] = {3655, -1, -1, 3655, 3655};
    ref["bier127"] = {5387,  -1, -1, -1, -1}; // 5387 má Sillberholtz, 5383 má Fischetti, ale OPLib ea4op má 5387, takže dávame 5387 ako referenciu
    ref["d198"] = {6670, -1, -1, 6670, 6660}; //6670 ma Fischetti, Sillberholtz ma 6670 tiez (kontrola s Claudom) !!!!

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
    return {-1, -1, -1, -1, -1};
}