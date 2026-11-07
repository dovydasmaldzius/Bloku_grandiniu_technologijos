#ifndef HASH_H
#define HASH_H

#include <cstdint>
#include <string>

using namespace std;

void HashFunkcija(const string& tekstas, uint32_t hash[8]);

string HashHex(const string& tekstas);

#endif