#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>
#include <utility>
#include "merkle.h"
#include "hash.h"

using namespace std;

string HexToBytes(const string& hexValue) { // konvertuoja šešioliktainę simbolių eilutę į baitų eilutę
string bytes;

if (hexValue.length() % 2 != 0) { // jei šešioliktainė simbolių eilutė turi nelyginį skaičių simbolių, grąžinama tuščia eilutė
return ""; }

bytes.reserve(hexValue.length() / 2); // rezervuojama vieta baitų eilutėje, kad būtų galima saugoti pusę šešioliktainės simbolių eilutės ilgio baitų

for (size_t i = 0; i < hexValue.length(); i += 2) {
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