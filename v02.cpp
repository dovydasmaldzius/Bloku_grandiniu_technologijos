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
            hash[j] = hash[j] * daugikliai[j] + simbolis + static_cast<uint32_t>(i + 1) * (j + 1);
        }
    }
}

void Eksperimentas() {
    ifstream failas("konstitucija.txt", ios::binary);
    if (!failas) {
        cerr << "Nepavyko atidaryti failo konstitucija.txt" << endl;
        return;
    }

    string tekst((istreambuf_iterator<char>(failas)), istreambuf_iterator<char>());

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
    cout << "Tuščias tekstas: " << (emptyOK ? "PASS" : "FAIL") << "\n";

    uint32_t testHash[8] = {};
    HashFunkcija("a", testHash);
    bool aOK = (testHash[0] == 98 && testHash[1] == 99);
    cout << "\"a\": " << (aOK ? "PASS" : "FAIL") << "\n\n";

    if (!emptyOK || !aOK) {
        return;
    }

    cout << "4 eksperimentas: failo pradžios ištraukų matavimas\n";
    cout << "Matavimo vienetas: sekundės. Apšilimas: 3. Kiekvienam dydžiui: 5 matavimai.\n";
    cout << "Matuojamas tik hash skaičiavimas; failų įvedimas / išvedimas iš čia pašalinti.\n\n";

    for (size_t lines : dydziai) {
        size_t bytes = (lines <= eiluciuPabaigos.size()) ? eiluciuPabaigos[lines - 1] : tekst.size();
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
            ? static_cast<size_t>(5.0 / std::max(1e-12, probeSeconds))
            : 1;

        repetitions = std::max<size_t>(1, std::min<size_t>(repetitions, 100000));

        double sum = 0.0;
        double minv = std::numeric_limits<double>::max();
        double maxv = 0.0;

        cout << "Dydis: eilutės = " << lines << ", baitai = " << bytes << "\n";

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
            minv = std::min(minv, sPerHash);
            maxv = std::max(maxv, sPerHash);

            cout << "  Matuavimas " << sample << ": " << sPerHash << " s\n";
        }

        double mean = sum / 5.0;

        cout << "  Vidurkis: " << mean << " s\n";
        cout << "  Min: " << minv << " s\n";
        cout << "  Max: " << maxv << " s\n\n";
    }
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    string zodis;
    string pasirinkimas;

    cout << "Kaip norite įvesti žodį arba tekstą?\n";
    cout << "1 - įvesti ranka\n";
    cout << "2 - nuskaityti iš failo\n";
    cout << "4 - 4 eksperimentas (konstitucija.txt)\n";
    cout << "Pasirinkimas: ";
    getline(cin, pasirinkimas);

    if (pasirinkimas == "4" || pasirinkimas == "3") {
        Eksperimentas();
        return 0;
    }

    if (pasirinkimas == "2") {
        ifstream failas("abc.txt");
        if (!failas) {
            cerr << "Nepavyko atidaryti failo" << endl;
            return 1;
        }

        zodis.assign(istreambuf_iterator<char>(failas), istreambuf_iterator<char>());
        uint32_t hash[8] = {};

        auto start = chrono::steady_clock::now();
        HashFunkcija(zodis, hash);
        auto end = chrono::steady_clock::now();

        chrono::duration<double> laikas = end - start;

        cout << "hash: ";
        cout << hex << setfill('0');
        for (int i = 0; i < 8; i++) {
            cout << setw(8) << hash[i];
        }
        cout << endl << endl;
        cout << "Hashavimo laikas: " << fixed << setprecision(6) << laikas.count() << " sekundžių" << endl;
        cout << "Programa baigta" << endl;
        return 0;
    }

    if (pasirinkimas != "1") {
        cerr << "Prašome pasirinkti tik 1, 2 arba 4!" << endl;
        return 1;
    }

    while (true) {
        cout << "įveskite žodį arba tekstą (b - baigti darbą): ";
        getline(cin, zodis);

        if (zodis == "b") {
            break;
        }

        uint32_t hash[8] = {};

        auto start = chrono::steady_clock::now();
        HashFunkcija(zodis, hash);
        auto end = chrono::steady_clock::now();

        chrono::duration<double> laikas = end - start;

        cout << "žodis/tekstas: " << zodis << endl;
        cout << "hash: ";
        cout << hex << setfill('0');

        for (int i = 0; i < 8; i++) {
            cout << setw(8) << hash[i];
        }
        cout << endl;
        cout << "Hashavimo laikas: " << fixed << setprecision(10) << laikas.count() << " s" << endl << endl;
    }

    return 0;
}