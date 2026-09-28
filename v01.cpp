#include <iostream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <windows.h>

using namespace std;



void HashFunkcija(const string& zodis, uint32_t hash[8]) {

const uint32_t daugikliai[8] = {31, 37, 41, 43, 47, 53, 59, 61};

for (int i = 0; i < zodis.length(); i++) {
    uint32_t simbolis = static_cast<uint32_t>(static_cast<unsigned char>(zodis[i]));

for (int j = 0; j < 8; j++) {
    hash[j] = hash[j] * daugikliai[j] + simbolis + static_cast<uint32_t>(i + 1) * (j + 1);
}
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
cout << "Pasirinkimas: ";
getline(cin, pasirinkimas);

if (pasirinkimas == "2") {
    ifstream failas("duomenufailas.txt");
if (!failas) {
    cerr << "Nepavyko atidaryti failo" << endl;
return 1;
}

zodis.assign(istreambuf_iterator<char>(failas), istreambuf_iterator<char>());
uint32_t hash[8] = {};
HashFunkcija(zodis, hash);

cout << "hash: ";
cout << hex << setfill('0');
for (int i = 0; i < 8; i++) {
    cout << setw(8) << hash[i];
}
cout << endl << endl;
cout << "Programa baigta" << endl;
return 0;
}

if (pasirinkimas != "1") {
    cerr << "Prašome paritinkti tik 1 arba 2!" << endl;
return 1;
}

while (true) {
cout << "įveskite žodį arba tekstą (b - baigti darbą): ";
    getline(cin, zodis);
if (zodis == "b") {
break;
}

uint32_t hash[8] = {};
HashFunkcija(zodis, hash);

cout << "žodis/tekstas: " << zodis << endl;
cout << "hash: ";
cout << hex << setfill('0');

for (int i = 0; i < 8; i++) {
cout << setw(8) << hash[i];
}
cout << endl << endl;
}

return 0;
}