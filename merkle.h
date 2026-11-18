#ifndef MERKLE_H
#define MERKLE_H

#include <string>
#include <vector>
#include "blockchain.h"

using namespace std;

string CalculateMerkleRoot(
const vector<Transaction>& transactions); // skaičiuojama merkle maiša

string HexToBytes(const std::string& hex); // konvertuoja šešioliktainę simbolių eilutę į baitų eilutę

Block MineMerkleBlock( // sukuriamas (kasamas) vienas blokas su merkle maiša
int height,
const string& prevHash,
const vector<Transaction>& transactions,
int difficulty);

vector<Block> CreateMerkleBlockchain( // sukuriama visa blokų grandinė su merkle maiša
const vector<Transaction>& transactions,
int difficulty);

bool VerifyMerkleBlock(const Block& block); // patikrinamas vienas blokas su merkle maiša

bool VerifyMerkleBlockchain(const vector<Block>& blockchain); // patikrinama visa blokų grandinė su merkle maiša

#endif