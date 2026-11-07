#include "hash.h"

#include <iomanip>
#include <sstream>

using namespace std;

void HashFunkcija(const string& zodis, uint32_t hash[8]) {

const uint32_t daugikliai[8] = {31, 37, 41, 43, 47, 53, 59, 61};

for (int i = 0; i < zodis.length(); i++) {
uint32_t simbolis = static_cast<unsigned char>(zodis[i]);

for (int j = 0; j < 8; j++) {
hash[j] = hash[j] * daugikliai[j] + simbolis + static_cast<uint32_t>(i + 1) * (j + 1);
}
}
}

string HashHex(const string& tekstas) {
uint32_t hash[8] = {};

HashFunkcija(tekstas, hash);

ostringstream rezultatas;
rezultatas << hex << nouppercase << setfill('0');

for (int i = 0; i < 8; i++) {
rezultatas << setw(8) << hash[i]; }

return rezultatas.str();
}