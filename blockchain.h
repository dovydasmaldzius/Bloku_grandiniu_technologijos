#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <cstdint>
#include <string>
#include <vector>

using namespace std;

struct User {
string id;
string publicKey;
int64_t balance; };

struct Transaction {
string senderId;
string receiverId;
int64_t amount;
uint64_t sequence;
string transactionId; };

struct ValidationResult {
vector<Transaction> accepted;
vector<Transaction> rejected;
vector<User> finalUsers; };

struct Block {
int height;
string prevHash;
string timestamp;
int version;
string txRoot;
uint64_t nonce;
int difficulty;
string blockHash;
vector<Transaction> transactions;
uint64_t miningAttempts;
double miningTime; };

vector<User> GenerateUsers(
int kiekis,
uint32_t seed);

vector<Transaction> GenerateTransactions(
const vector<User>& users,
int kiekis,
uint32_t seed);

ValidationResult ValidateTransactions(
const vector<User>& users,
const vector<Transaction>& transactions);

string CalculateTxRoot(
const vector<Transaction>& transactions);

string CalculateBlockHash(
const Block& block);

string GetCurrentTimestamp();

Block MineBlock(
int height,
const string& prevHash,
const vector<Transaction>& transactions,
int difficulty);

vector<Block> CreateBlockchain(
const vector<Transaction>& transactions,
int difficulty);

bool VerifyBlock(
const Block& block);

bool VerifyBlockchain(
const vector<Block>& blockchain);

#endif