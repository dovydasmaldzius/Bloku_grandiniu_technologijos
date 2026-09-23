#include <iostream>
#include <string>
#include <iomanip>
#include <cstdint>
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

while (true) {
cout << "įveskite žodį arba tekstą (b - baigti darbą): ";
    getline(cin, zodis);
if (zodis == "b") {
break;
}

uint32_t hash[8];
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