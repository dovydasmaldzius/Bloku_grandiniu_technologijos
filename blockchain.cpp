#include "blockchain.h"
#include "hash.h"

#include <chrono>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_map>

using namespace std;

vector<User> GenerateUsers(
int kiekis,
uint32_t seed) {
vector<User> users;
users.reserve(kiekis);

mt19937 generator(seed);
uniform_int_distribution<int64_t> balanceDist(100,1000000);

for (int i = 0; i < kiekis; ++i) {
ostringstream id;
id << "user_" << setw(4) << setfill('0') << i + 1;

User user;
user.id = id.str();
user.publicKey = "public_key_" + user.id;
user.balance = balanceDist(generator);
users.push_back(user); }
return users; }

vector<Transaction> GenerateTransactions(
const vector<User>& users,
int kiekis,
uint32_t seed) {
if (users.size() < 2) {
throw invalid_argument("Reikia bent dviejų vartotojų."); }

vector<Transaction> transactions;

transactions.reserve(kiekis);

mt19937 generator(seed);

uniform_int_distribution<size_t> userDist(0, users.size() - 1);

uniform_int_distribution<int64_t> amountDist(1, 1000000);

for (int i = 0; i < kiekis; ++i) {
size_t senderIndex = userDist(generator);

size_t receiverIndex = userDist(generator);

while (receiverIndex == senderIndex) {
receiverIndex = userDist(generator); }

Transaction tx;
tx.senderId = users[senderIndex].id;
tx.receiverId = users[receiverIndex].id;
tx.amount = amountDist(generator);
tx.sequence = static_cast<uint64_t>(i + 1);
string data = tx.senderId + "|" + tx.receiverId + "|" + to_string(tx.amount) + "|" + to_string(tx.sequence);
tx.transactionId = HashHex(data);
transactions.push_back(tx); }
return transactions; }

ValidationResult ValidateTransactions(
const vector<User>& users,
const vector<Transaction>& transactions) {
ValidationResult result;
result.finalUsers = users;

unordered_map<string, size_t> userIndex;

for (size_t i = 0; i < result.finalUsers.size(); ++i) {
userIndex[result.finalUsers[i].id] = i; }

unordered_map<string, bool> usedTransactionIds;

for (const auto& tx : transactions) {
if (userIndex.find(tx.senderId) == userIndex.end()) {
result.rejected.push_back(tx);
continue; }

if (userIndex.find(tx.receiverId) == userIndex.end()) {
result.rejected.push_back(tx);
continue; }

if (tx.senderId == tx.receiverId) {
result.rejected.push_back(tx);
continue; }

if (tx.amount <= 0) {
result.rejected.push_back(tx);
continue; }

string data = tx.senderId + "|" + tx.receiverId + "|" + to_string(tx.amount) + "|" + to_string(tx.sequence);

string calculatedId = HashHex(data);

if (tx.transactionId != calculatedId) {
result.rejected.push_back(tx);
continue; }

if (usedTransactionIds.find(tx.transactionId) != usedTransactionIds.end()) {
result.rejected.push_back(tx);
continue; }

size_t sender =  userIndex.at(tx.senderId);

size_t receiver = userIndex.at(tx.receiverId);

if (result.finalUsers[sender].balance < tx.amount) {
result.rejected.push_back(tx);
continue; }

result.finalUsers[sender].balance -= tx.amount;

result.finalUsers[receiver].balance += tx.amount;

usedTransactionIds[tx.transactionId] = true;
result.accepted.push_back(tx); }
return result; }

string CalculateTxRoot(
const vector<Transaction>& transactions) {
string combined;

for (const auto& tx : transactions) {
combined += tx.transactionId; }
return HashHex(combined); }

string GetCurrentTimestamp() {
auto now = chrono::system_clock::now();
time_t time = chrono::system_clock::to_time_t(now);
tm localTime{};
localtime_s(&localTime, &time);
ostringstream result;
result << put_time(&localTime, "%Y-%m-%d %H:%M:%S");
return result.str(); }

string CalculateBlockHash(
const Block& block) {
string header = block.prevHash + "|" + block.timestamp + "|" + to_string(block.version) + "|" + block.txRoot + "|" + to_string(block.nonce) + "|" + to_string(block.difficulty);
return HashHex(header); }

Block MineBlock(
int height,
const string& prevHash,
const vector<Transaction>& transactions,
int difficulty) {
Block block;
block.height = height;
block.prevHash = prevHash;
block.timestamp = GetCurrentTimestamp();
block.version = 1;
block.transactions = transactions;
block.txRoot = CalculateTxRoot(block.transactions);
block.nonce = 0;
block.difficulty = difficulty;
block.blockHash = "";
block.miningAttempts = 0;

auto start= chrono::steady_clock::now();

string target(difficulty,'0');

while (true) {
++block.miningAttempts;
block.blockHash = CalculateBlockHash(block);

if (block.blockHash.substr(0,difficulty) == target) {
break; }
++block.nonce; }

auto end = chrono::steady_clock::now();
chrono::duration<double> elapsed = end - start;

block.miningTime = elapsed.count();
return block; }

vector<Block> CreateBlockchain(
const vector<Transaction>& transactions,
int difficulty) {
vector<Block> blockchain;
Block genesis;
genesis.height = 0;
genesis.prevHash = "0000000000000000000000000000000000000000000000000000000000000000";
genesis.timestamp = "2026-01-01 00:00:00";
genesis.version = 1;
genesis.txRoot = HashHex("");
genesis.nonce = 0;
genesis.difficulty = 0;
genesis.transactions.clear();
genesis.miningAttempts = 0;
genesis.miningTime = 0.0;
genesis.blockHash = CalculateBlockHash(genesis);
blockchain.push_back(genesis);

const size_t maxTransactionsPerBlock = 100;
string previousHash =
genesis.blockHash;

int height = 1;

for (size_t start = 0; start < transactions.size(); start += maxTransactionsPerBlock) {
size_t end = min(start + maxTransactionsPerBlock, transactions.size() );
vector<Transaction>blockTransactions;

for (size_t i = start; i < end; ++i) {
blockTransactions.push_back (transactions[i]); }

Block block = MineBlock(height, previousHash, blockTransactions, difficulty);

blockchain.push_back (block);
previousHash = block.blockHash;
++height; }
return blockchain; }

bool VerifyBlock(
const Block& block) {
string calculatedTxRoot = CalculateTxRoot(block.transactions);
if (calculatedTxRoot != block.txRoot) {
return false; }

string calculatedHash = CalculateBlockHash(block);
if (calculatedHash != block.blockHash) {
return false; }

if (block.difficulty > 0) {
string target(block.difficulty, '0');

if (block.blockHash.substr(0, block.difficulty) != target) {
return false; } }
return true; }

bool VerifyBlockchain(
const vector<Block>& blockchain) {
if (blockchain.empty()) {
return false; }

if (blockchain[0].height != 0) {
return false; }

if (!VerifyBlock(blockchain[0])) {
return false; }

for (size_t i = 1; i < blockchain.size(); ++i) {
const Block& previous = blockchain[i - 1];
const Block& current = blockchain[i];

if (current.height != previous.height + 1) {
return false; }

if (current.prevHash != previous.blockHash) {
return false; }

if (!VerifyBlock(current)) {
return false; } }
return true; }