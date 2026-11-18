#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <utility>
#include <atomic>
#include <thread>
#include <algorithm>
#include <limits>
#include <cctype>
#include <unordered_map>
#include <unordered_set>
#include "merkle.h"
#include "hash.h"

using namespace std;

string HexToBytes(const string& hexValue) { // konvertuoja šešioliktainę simbolių eilutę į baitų eilutę
string bytes;

if (hexValue.length() % 2 != 0) { // jei šešioliktainė simbolių eilutė turi nelyginį skaičių simbolių, grąžinama tuščia eilutė
return ""; }
if (hexValue.empty()) return "";

bytes.reserve(hexValue.length() / 2); // rezervuojama vieta baitų eilutėje, kad būtų galima saugoti pusę šešioliktainės simbolių eilutės ilgio baitų

for (size_t i = 0; i < hexValue.length(); i += 2) {
if (!isxdigit(static_cast<unsigned char>(hexValue[i])) ||
!isxdigit(static_cast<unsigned char>(hexValue[i + 1]))) return "";
unsigned int value = 0;
stringstream ss;
ss << hex << hexValue.substr(i, 2);
ss >> value;
bytes.push_back(static_cast<char>(value)); }
return bytes; } // konvertuoja šešioliktainę simbolių eilutę į baitų eilutę

string CalculateMerkleRoot( // skaičiuojama merkle maiša
const vector<Transaction>& transactions) {
if (transactions.empty()) {
return HashHex(""); }

vector<string> level;
for (const auto& tx : transactions) {
level.push_back(HexToBytes(tx.transactionId)); } // pradedama nuo transakcijų ID, kurie yra šešioliktainės simbolių eilutės, konvertuojamos į baitų eilutes

while (level.size() > 1) {
vector<string> nextLevel;
for (size_t i = 0; i < level.size(); i += 2) {
string left = level[i];
string right;

if (i + 1 < level.size()) {
right = level[i + 1]; }
else {
right= left; }

string combined = left + right;
nextLevel.push_back(HashHex(combined));
nextLevel.back() = HexToBytes(nextLevel.back()); }
level = move(nextLevel); }

ostringstream result;
result << hex<< nouppercase << setfill('0'); // atspausdinamas galutinė merkle maiša šešioliktainėje simbolių eilutėje

for (unsigned char c : level[0]) {
result << setw(2) << static_cast<unsigned int>(c); }
return result.str(); } // grąžinama merkle maiša šešioliktainėje simbolių eilutėje

static string CalculateMerkleBlockHash(const Block& block) { // skaičiuojama bloko maiša
string data = block.prevHash + "|" + block.timestamp + "|" + to_string(block.version) + "|" + block.txRoot + "|" + to_string(block.nonce) + "|" + to_string(block.difficulty);
return HashHex(data); }

static string GetMerkleTimestamp() { // grąžina dabartinį laiką formatu "YYYY-MM-DD HH:MM:SS"
auto now = chrono::system_clock::now();
time_t time = chrono::system_clock::to_time_t(now);
tm tm{};
localtime_s(&tm, &time);

ostringstream oss; // atspausdinamas dabartinis laikas formatu "YYYY-MM-DD HH:MM:SS"
oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
return oss.str(); }

Block MineMerkleBlock( // sukuriamas (kasamas) vienas blokas su merkle maiša
int height,
const string& prevHash,
const vector<Transaction>& transactions,
int difficulty) {
Block block;
block.height = height;
block.prevHash = prevHash;
block.timestamp = GetMerkleTimestamp();
block.version = 1;
block.txRoot = CalculateMerkleRoot(transactions);
block.nonce = 0;
block.difficulty = difficulty;
block.transactions = transactions;
block.miningAttempts = 0;
block.miningTime = 0.0;

auto start = chrono::high_resolution_clock::now();
string target(difficulty, '0'); // jei difficulty yra 3, tai target bus "000"

while (true) {
block.blockHash = CalculateMerkleBlockHash(block); // skaičiuojama bloko maiša
block.miningAttempts++;
if (block.blockHash.substr(0, difficulty) == target) { // jei bloko maiša atitinka sudėtingumo reikalavimus, sustabdomas kasimas
break; }
block.nonce++; }

auto end = chrono::high_resolution_clock::now();
chrono::duration<double> elapsed = end - start;

block.miningTime = elapsed.count();
return block; }

vector<MiningCandidateResult> CreateMiningCandidates(
int height,
const string& prevHash,
const vector<Transaction>& poolTransactions,
const vector<User>& confirmedUsers,
int difficulty,
uint64_t attemptLimit) {
vector<MiningCandidateResult> candidates(5);
atomic<unsigned> nextCandidate(0);
vector<thread> workers;

const unsigned workerCount = min<unsigned>(5, thread::hardware_concurrency() ? 5 : 1); 
for (unsigned workerId = 0; workerId < workerCount; ++workerId) {
workers.emplace_back([&]() {
for (;;) {
const unsigned index = nextCandidate.fetch_add(1);
if (index >= 5) break;

vector<Transaction> selected;
vector<User> temporaryUsers = confirmedUsers;
unordered_map<string, size_t> userIndex;
unordered_set<string> selectedIds;
for (size_t i = 0; i < temporaryUsers.size(); ++i) {
userIndex[temporaryUsers[i].id] = i;
}

size_t begin = static_cast<size_t>(index) * 20;
size_t end = min(poolTransactions.size(), begin + 100);
if (begin >= poolTransactions.size()) {
begin = 0;
end = min(poolTransactions.size(), size_t(100));
}

for (size_t i = begin; i < end && selected.size() < 100; ++i) {
const Transaction& tx = poolTransactions[i];
const auto sender = userIndex.find(tx.senderId);
const auto receiver = userIndex.find(tx.receiverId);
if (sender == userIndex.end() || receiver == userIndex.end() ||
sender->first == receiver->first || tx.amount <= 0 ||
selectedIds.find(tx.transactionId) != selectedIds.end()) continue;

const string data = tx.senderId + "|" + tx.receiverId + "|" +
to_string(tx.amount) + "|" + to_string(tx.sequence);
if (tx.transactionId != HashHex(data)) continue;

User& senderUser = temporaryUsers[sender->second];
User& receiverUser = temporaryUsers[receiver->second];
if (senderUser.balance < tx.amount ||
receiverUser.balance > numeric_limits<int64_t>::max() - tx.amount) continue;

senderUser.balance -= tx.amount;
receiverUser.balance += tx.amount;
selectedIds.insert(tx.transactionId);
selected.push_back(tx);
}

Block block;
block.height = height;
block.prevHash = prevHash;
block.timestamp = "2026-01-01 00:00:00";
block.version = 1;
block.transactions = selected;
block.txRoot = CalculateMerkleRoot(selected);
block.nonce = 0;
block.difficulty = difficulty;
block.miningAttempts = 0;
block.miningTime = 0.0;

const auto start = chrono::steady_clock::now();
const string target(difficulty, '0');
for (uint64_t nonce = 0; nonce < attemptLimit; ++nonce) {
block.nonce = nonce;
block.blockHash = CalculateMerkleBlockHash(block);
++block.miningAttempts;
if (block.blockHash.compare(0, difficulty, target) == 0) break;
}

block.miningTime =
chrono::duration<double>(chrono::steady_clock::now() - start).count();
const bool found =
block.blockHash.compare(0, difficulty, target) == 0;

candidates[index].candidateNumber = index + 1;
candidates[index].block = move(block);
candidates[index].attemptedNonces = candidates[index].block.miningAttempts;
candidates[index].miningTime = candidates[index].block.miningTime;
candidates[index].found = found;
} });
}
for (thread& worker : workers) worker.join();
return candidates;
}

vector<Block> CreateMerkleBlockchain(
const vector<Transaction>& transactions,
int difficulty) {
vector<Block> blockchain;
Block genesis;
genesis.height = 0;
genesis.prevHash = string(64, '0');
genesis.timestamp = "2026-01-01 00:00:00";
genesis.version = 1;
genesis.txRoot = HashHex("");
genesis.nonce = 0; 
genesis.difficulty = 0;
genesis.blockHash = CalculateMerkleBlockHash(genesis);
genesis.transactions.clear();
genesis.miningAttempts = 0;
genesis.miningTime = 0.0;
blockchain.push_back(genesis);

const size_t BLOCK_SIZE = 100; // nustatomas maksimalus transakcijų skaičius viename bloke
for (size_t i = 0; i < transactions.size(); i += BLOCK_SIZE) {
size_t end = min(i + BLOCK_SIZE, transactions.size());
vector<Transaction> blockTransactions(transactions.begin() + i, transactions.begin() + end);

const string prevHash = blockchain.back().blockHash; // nustatoma ankstesnio bloko maiša
Block block = MineMerkleBlock(static_cast<int>(blockchain.size()), prevHash, blockTransactions, difficulty);
blockchain.push_back(block); }
return blockchain; }

bool VerifyMerkleBlock(const Block& block) { // patikrinamas vienas blokas su merkle maiša
string calculatedRoot = CalculateMerkleRoot(block.transactions);
if (calculatedRoot != block.txRoot) {
return false; }

string calculatedHash = CalculateMerkleBlockHash(block); // patikrinama, ar bloko maiša atitinka skaičiuojamą bloko maišą
if (calculatedHash != block.blockHash) {
return false; }

if (block.difficulty > 0) {
string target(block.difficulty, '0');

if (block.blockHash.substr(0, block.difficulty) != target) { // patikrinama, ar bloko maiša atitinka sudėtingumo reikalavimus
return false; } }
return true; }

bool VerifyMerkleBlockchain( // patikrinama visa blokų grandinė su merkle maiša
const vector<Block>& blockchain) {
if (blockchain.empty()) {
return false; }
if (blockchain[0].height != 0) {
return false; }
if (!VerifyMerkleBlock(blockchain[0])) {
return false; }

for (size_t i = 1; i < blockchain.size();i++) { // pradedama nuo antrojo bloko, nes pirmasis blokas yra genesis blokas
const Block& previous = blockchain[i - 1];
const Block& current = blockchain[i];
if (current.height != previous.height + 1) {
return false; }
if (current.prevHash != previous.blockHash) {
return false; }
if (!VerifyMerkleBlock(current)) {
return false; } }
return true; }