#include <iostream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <iterator>
#include <windows.h>
#include <vector>
#include <algorithm>
#include <limits>
#include <random>
#include <set>
#include <unordered_map>
#include <sstream>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

using namespace std;

volatile uint32_t g_hash_guard = 0;

void HashFunkcija(const string& zodis, uint32_t hash[8]) {
    const uint32_t daugikliai[8] = {31, 37, 41, 43, 47, 53, 59, 61};

    for (int i = 0; i < static_cast<int>(zodis.length()); ++i) {
        uint32_t simbolis = static_cast<unsigned char>(zodis[i]);

        for (int j = 0; j < 8; ++j) {
            hash[j] = hash[j] * daugikliai[j] + simbolis
                    + static_cast<uint32_t>(i + 1) * (j + 1);
        }
    }
}

bool HashTiksliaiLygu(const uint32_t a[8], const uint32_t b[8]) {
    for (int i = 0; i < 8; ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

string HashKey(const uint32_t hash[8]) {
    ostringstream oss;
    for (int i = 0; i < 8; ++i) {
        oss << hash[i] << ":";
    }
    return oss.str();
}

void Eksperimentas4() {
    ifstream failas("konstitucija.txt", ios::binary);
    if (!failas) {
        cerr << "Nepavyko atidaryti failo konstitucija.txt" << endl;
        return;
    }

    string tekst((istreambuf_iterator<char>(failas)),
                 istreambuf_iterator<char>());

    vector<size_t> eiluciuPabaigos;
    size_t pos = 0;

    while (pos < tekst.size()) {
        size_t next = tekst.find('\n', pos);
        if (next == string::npos) {
            eiluciuPabaigos.push_back(tekst.size());
            break;
        }
        eiluciuPabaigos.push_back(next + 1);
        pos = next + 1;
    }

    if (eiluciuPabaigos.empty()) {
        eiluciuPabaigos.push_back(tekst.size());
    }

    vector<size_t> dydziai;
    size_t totalLines = eiluciuPabaigos.size();

    for (size_t n = 1; n < totalLines; n *= 2) {
        dydziai.push_back(n);
    }

    if (dydziai.empty() || dydziai.back() != totalLines) {
        dydziai.push_back(totalLines);
    }

    sort(dydziai.begin(), dydziai.end());
    dydziai.erase(unique(dydziai.begin(), dydziai.end()), dydziai.end());

    cout << "Teisingumo testai:\n";

    uint32_t emptyHash[8] = {};
    HashFunkcija("", emptyHash);

    bool emptyOK = true;
    for (int i = 0; i < 8; ++i) {
        emptyOK = emptyOK && (emptyHash[i] == 0);
    }
    cout << "Tuscias tekstas: " << (emptyOK ? "PASS" : "FAIL") << "\n";

    uint32_t testHash[8] = {};
    HashFunkcija("a", testHash);
    bool aOK = (testHash[0] == 98 && testHash[1] == 99);
    cout << "\"a\": " << (aOK ? "PASS" : "FAIL") << "\n\n";

    if (!emptyOK || !aOK) {
        return;
    }

    cout << "4 eksperimentas: failo pradzios istrauku matavimas\n";
    cout << "Matavimo vienetas: sekundes. Apšilimas: 3. "
         << "Kiekvienam dydziui: 5 matavimai.\n";
    cout << "Matuojamas tik hash skaiciavimas; failu I/O ir konsoles isvedimas "
         << "neitraukti i matavima.\n\n";

    for (size_t lines : dydziai) {
        size_t bytes = eiluciuPabaigos[lines - 1];
        string fragment = tekst.substr(0, bytes);

        for (int i = 0; i < 3; ++i) {
            uint32_t warmup[8] = {};
            HashFunkcija(fragment, warmup);
            g_hash_guard ^= warmup[0];
        }

        uint32_t probe[8] = {};
        auto start = chrono::steady_clock::now();
        HashFunkcija(fragment, probe);
        g_hash_guard ^= probe[0];
        auto end = chrono::steady_clock::now();

        chrono::duration<double> probeDuration = end - start;
        double probeSeconds = probeDuration.count();

        size_t repetitions = probeSeconds > 0.0
            ? static_cast<size_t>(5.0 / max(1e-12, probeSeconds))
            : 1;

        repetitions = max<size_t>(1, min<size_t>(repetitions, 100000));

        double sum = 0.0;
        double minv = numeric_limits<double>::max();
        double maxv = 0.0;

        cout << "Dydis: eilutes = " << lines
             << ", baitai = " << bytes
             << ", kartojimai = " << repetitions << "\n";

        for (int sample = 1; sample <= 5; ++sample) {
            uint32_t total = 0;

            auto sampleStart = chrono::steady_clock::now();
            for (size_t r = 0; r < repetitions; ++r) {
                uint32_t current[8] = {};
                HashFunkcija(fragment, current);
                total ^= current[0];
            }
            g_hash_guard ^= total;
            auto sampleEnd = chrono::steady_clock::now();

            chrono::duration<double> elapsed = sampleEnd - sampleStart;
            double sPerHash = elapsed.count() / repetitions;

            sum += sPerHash;
            minv = min(minv, sPerHash);
            maxv = max(maxv, sPerHash);

            cout << "  Matavimas " << sample << ": "
                 << sPerHash << " s\n";
        }

        double mean = sum / 5.0;

        cout << "  Vidurkis: " << mean << " s\n";
        cout << "  Min: " << minv << " s\n";
        cout << "  Max: " << maxv << " s\n\n";
    }
}

void Eksperimentas5() {
    const vector<size_t> ilgiai = {10, 100, 500, 1000};
    const size_t poruKiekis = 100000;
    const uint64_t seed = 123456789ULL;

    cout << "5 eksperimentas: koliziju paieska\n";
    cout << "Abcele: ASCII simboliai 32..126 (94 spausdinami simboliai; "
         << "vienas simbolis = vienas baitas).\n";
    cout << "Generatoriaus pradine reiksme: " << seed << "\n";
    cout << "Kiekvienam ilgiui: 100 000 skirtingu ivesciu poru.\n\n";

    mt19937_64 rng(seed);
    uniform_int_distribution<int> dist(32, 126);

    auto generuotiEilute = [&](size_t ilgis) {
        string s;
        s.reserve(ilgis);

        for (size_t i = 0; i < ilgis; ++i) {
            s.push_back(static_cast<char>(dist(rng)));
        }
        return s;
    };

    auto generuotiUnikalius = [&](size_t ilgis, size_t kiekis) {
        set<string> unikalios;
        vector<string> rezultatas;
        rezultatas.reserve(kiekis);

        while (rezultatas.size() < kiekis) {
            string s = generuotiEilute(ilgis);
            if (unikalios.insert(s).second) {
                rezultatas.push_back(s);
            }
        }
        return rezultatas;
    };

    auto spausdintiHash = [](const uint32_t h[8]) {
        cout << hex << setfill('0');
        for (int i = 0; i < 8; ++i) {
            cout << setw(8) << h[i];
        }
        cout << dec << setfill(' ') << "\n";
    };

    for (size_t ilgis : ilgiai) {
        // Sugeneruojama 200 000 unikaliu eiluciu:
        // kiekvienos poros ivesciai skiriasi.
        vector<string> eilutes = generuotiUnikalius(ilgis, poruKiekis * 2);

        size_t patikrintaPoru = 0;
        size_t kolizijuPoromis = 0;
        int parodytaPoruPavyzdziu = 0;

        cout << "Ilgis " << ilgis << " baitu:\n";

        for (size_t i = 0; i < poruKiekis; ++i) {
            const string& a = eilutes[i * 2];
            const string& b = eilutes[i * 2 + 1];

            uint32_t ha[8] = {};
            uint32_t hb[8] = {};
            HashFunkcija(a, ha);
            HashFunkcija(b, hb);

            ++patikrintaPoru;

            if (a != b && HashTiksliaiLygu(ha, hb)) {
                ++kolizijuPoromis;

                if (parodytaPoruPavyzdziu < 3) {
                    cout << "  Kolizijos pavyzdys:\n";
                    cout << "    A: " << a << "\n";
                    cout << "    B: " << b << "\n";
                    cout << "    Hash A: ";
                    spausdintiHash(ha);
                    cout << "    Hash B: ";
                    spausdintiHash(hb);
                    ++parodytaPoruPavyzdziu;
                }
            }
        }

        cout << "  Poru patikra: " << patikrintaPoru
             << ", koliziju: " << kolizijuPoromis << "\n";

        unordered_map<string, vector<string> > hashGroups;

        for (const string& s : eilutes) {
            uint32_t h[8] = {};
            HashFunkcija(s, h);
            hashGroups[HashKey(h)].push_back(s);
        }

        size_t grupiuSuKolizijomis = 0;
        size_t kolizijuVisameRinkinyje = 0;
        size_t parodytaGrupiu = 0;

        for (const auto& entry : hashGroups) {
            const string& key = entry.first;
            const vector<string>& group = entry.second;

            if (group.size() > 1) {
                ++grupiuSuKolizijomis;
                kolizijuVisameRinkinyje +=
                    group.size() * (group.size() - 1) / 2;

                if (parodytaGrupiu < 3) {
                    cout << "  Hash grupes pavyzdys: " << key << "\n";
                    cout << "    Grupes dydis: " << group.size() << "\n";
                    cout << "    Ivestys: " << group[0]
                         << " / " << group[1] << "\n";
                    ++parodytaGrupiu;
                }
            }
        }

        cout << "  Skirtingu ivesciu skaicius: " << eilutes.size() << "\n";
        cout << "  Skirtingu hash grupiu su kolizijomis: "
             << grupiuSuKolizijomis << "\n";
        cout << "  Koliziju poru visame rinkinyje: "
             << kolizijuVisameRinkinyje << "\n\n";
    }

    cout << "Strukturuotu ivesciu testai:\n";

    vector<string> strukturuotos;
    const size_t strukturuotiIlgiai[] = {10, 100};

    for (size_t lengthIndex = 0; lengthIndex < 2; ++lengthIndex) {
        size_t ilgis = strukturuotiIlgiai[lengthIndex];

        string vienodiA(ilgis, 'A');
        string vienodiB(ilgis, 'B');
        string didejimas;
        string mazejimas;
        string periodinis;

        didejimas.reserve(ilgis);
        mazejimas.reserve(ilgis);
        periodinis.reserve(ilgis);

        for (size_t i = 0; i < ilgis; ++i) {
            didejimas.push_back(static_cast<char>(32 + (i % 94)));
            mazejimas.push_back(static_cast<char>(32 + ((93 - (i % 94)) % 94)));
            periodinis.push_back("ABCD"[i % 4]);
        }

        strukturuotos.push_back(vienodiA);
        strukturuotos.push_back(vienodiB);
        strukturuotos.push_back(didejimas);
        strukturuotos.push_back(mazejimas);
        strukturuotos.push_back(periodinis);
    }

    unordered_map<string, vector<string> > strukturuotosGrupes;

    for (const string& s : strukturuotos) {
        uint32_t h[8] = {};
        HashFunkcija(s, h);
        strukturuotosGrupes[HashKey(h)].push_back(s);
    }

    size_t strukturuotuKoliziju = 0;

    for (const auto& entry : strukturuotosGrupes) {
        const string& key = entry.first;
        const vector<string>& group = entry.second;

        if (group.size() > 1) {
            ++strukturuotuKoliziju;
            cout << "  Kolizijos grupe: " << key
                 << ", skirtingu ivesciu: " << group.size() << "\n";
            cout << "    Pavyzdys: " << group[0]
                 << " / " << group[1] << "\n";
        }
    }

    if (strukturuotuKoliziju == 0) {
        cout << "  Koliziju strukturuotu ivesciu rinkinyje nerasta.\n";
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    string zodis;
    string pasirinkimas;

    cout << "Kaip norite ivesti zodi arba teksta?\n";
    cout << "1 - ivesti ranka\n";
    cout << "2 - nuskaityti is failo\n";
    cout << "4 - 4 eksperimentas (konstitucija.txt)\n";
    cout << "5 - 5 eksperimentas (koliziju paieska)\n";
    cout << "Pasirinkimas: ";
    getline(cin, pasirinkimas);

    if (pasirinkimas == "5") {
        Eksperimentas5();
        return 0;
    }

    if (pasirinkimas == "4" || pasirinkimas == "3") {
        Eksperimentas4();
        return 0;
    }

    if (pasirinkimas == "2") {
        ifstream failas("abc.txt");
        if (!failas) {
            cerr << "Nepavyko atidaryti failo" << endl;
            return 1;
        }

        zodis.assign(istreambuf_iterator<char>(failas),
                     istreambuf_iterator<char>());

        uint32_t hash[8] = {};

        auto start = chrono::steady_clock::now();
        HashFunkcija(zodis, hash);
        auto end = chrono::steady_clock::now();

        chrono::duration<double> laikas = end - start;

        cout << "Hash: " << hex << setfill('0');
        for (int i = 0; i < 8; ++i) {
            cout << setw(8) << hash[i];
        }

        cout << dec << setfill(' ') << endl;
        cout << "Hashavimo laikas: " << fixed << setprecision(6)
             << laikas.count() << " sekundziu" << endl;
        cout << "Programa baigta" << endl;
        return 0;
    }

    if (pasirinkimas != "1") {
        cerr << "Pasirinkite 1, 2, 4 arba 5!" << endl;
        return 1;
    }

    while (true) {
        cout << "Iveskite zodi arba teksta (b - baigti darba): ";
        getline(cin, zodis);

        if (zodis == "b") {
            break;
        }

        uint32_t hash[8] = {};

        auto start = chrono::steady_clock::now();
        HashFunkcija(zodis, hash);
        auto end = chrono::steady_clock::now();

        chrono::duration<double> laikas = end - start;

        cout << "Zodis/tekstas: " << zodis << endl;
        cout << "Hash: " << hex << setfill('0');

        for (int i = 0; i < 8; ++i) {
            cout << setw(8) << hash[i];
        }

        cout << dec << setfill(' ') << endl;
        cout << "Hashavimo laikas: " << fixed << setprecision(10)
             << laikas.count() << " s" << endl << endl;
    }

    return 0;
}