#ifndef MERKLE_H
#define MERKLE_H

#include <string>
#include <vector>
#include "blockchain.h"

using namespace std;

string CalculateMerkleRoot(
const vector<Transaction>& transactions);

string HexToBytes(const std::string& hex);

Block MineMerkleBlock(
int height,
const string& prevHash,
const vector<Transaction>& transactions,
int difficulty);

vector<Block> CreateMerkleBlockchain(
const vector<Transaction>& transactions,
int difficulty);

bool VerifyMerkleBlock(const Block& block);

bool VerifyMerkleBlockchain(
const vector<Block>& blockchain);

#endif