#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdint>

#include "blockchain.h"
#include "merkle.h"
#include "hash.h"


using namespace std;

void TestMerkleCases(
const vector<Transaction>& transactions) {
cout << "\n";
cout << "==============================\n";
cout << "MERKLE MEDZIO TESTAI\n";
cout << "==============================\n";

vector<int> testSizes = {1, 2, 3, 5};

for (int count : testSizes) {
if (transactions.size() < static_cast<size_t>(count)) {
continue; }

vector<Transaction> testTransactions(transactions.begin(),transactions.begin() + count);

string root = CalculateMerkleRoot(testTransactions);

cout << "\n" << "Transakciju skaicius: " << count << "\n";

cout <<"Merkle root:"<< root << "\n"; }

if (transactions.size() >= 2) {

vector<Transaction> original = {
transactions[0],transactions[1] };
string rootOriginal = CalculateMerkleRoot(original);

vector<Transaction> changed = original;
changed[0].amount++;
string data = changed[0].senderId + "|" + changed[0].receiverId + "|" + to_string(changed[0].amount) + "|" + to_string(changed[0].sequence);
changed[0].transactionId = HashHex(data);
string rootChanged = CalculateMerkleRoot(changed);

cout << "\n";
cout << "Transakcijos turinio pakeitimo testas:\n";
cout <<"Senas root:   "<< rootOriginal << "\n";
cout<< "Naujas root:   " << rootChanged << "\n";
cout << "Root pasikeite: " << (rootOriginal != rootChanged ? "TAIP" : "NE")<< "\n"; }

if (transactions.size() >= 2) {
vector<Transaction> original = {
transactions[0], transactions[1] };
string rootOriginal = CalculateMerkleRoot(original);

vector<Transaction> reversed = {
transactions[1], transactions[0] };
string rootReversed = CalculateMerkleRoot(reversed);

cout <<"\n";
cout << "Transakciju tvarkos pakeitimo testas:\n";
cout << "Senas root:   "<< rootOriginal << "\n";
cout << "Naujas root:   " << rootReversed << "\n";
cout << "Root pasikeite: " << (rootOriginal != rootReversed ? "TAIP" : "NE") << "\n"; } }

int main() {

cout << "==============================\n";
cout << "SUPAPRASTINTA BLOKU GRANDINE\n";
cout << "V0.2 - MERKLE MEDIS\n";
cout << "==============================\n";

const int USER_COUNT = 1000;
const int TRANSACTION_COUNT = 10000;
const uint32_t USER_SEED = 12345;
const uint32_t TRANSACTION_SEED = 12346;
const int DIFFICULTY = 3;

vector<User> users = GenerateUsers(USER_COUNT, USER_SEED);

vector<Transaction> transactions = GenerateTransactions(users, TRANSACTION_COUNT, TRANSACTION_SEED);

ValidationResult validation = ValidateTransactions(users, transactions);
TestMerkleCases(validation.accepted);

vector<Block> blockchain = CreateMerkleBlockchain(validation.accepted, DIFFICULTY);

cout << "\n";
cout << "==============================\n";
cout << "BLOKU GRANDINE\n";
cout << "==============================\n";

for (const auto& block : blockchain) {
cout << "\n";
cout<< "Bloko aukstis: " << block.height << "\n";
cout <<"Transakciju skaicius: " << block.transactions.size() << "\n";
cout << "Prev hash: " << block.prevHash << "\n";
cout << "Merkle root: " << block.txRoot << "\n";
cout << "Block hash: " << block.blockHash << "\n";
cout << "Difficulty: "<< block.difficulty << "\n";
cout << "Nonce: " << block.nonce << "\n";
cout << "Mining attempts: "<< block.miningAttempts << "\n";
cout << fixed<< setprecision(6);
cout << "Mining time: "<< block.miningTime << " s\n"; }

bool valid = VerifyMerkleBlockchain(blockchain);

int64_t initialTotal= 0;
int64_t finalTotal = 0;

for (const auto& user : users) {
initialTotal += user.balance; }

for (const auto& user : validation.finalUsers) {
finalTotal += user.balance; }

cout << "\n";
cout << "==============================\n";
cout << "GALUTINE SUVESTINE\n";
cout <<"==============================\n";
cout << "Vartotoju skaicius: "<< users.size() << "\n";
cout << "Sugeneruotu transakciju: " << transactions.size() << "\n";
cout << "Priimtu transakciju: " << validation.accepted.size() << "\n";
cout << "Atmestu transakciju: "<< validation.rejected.size() << "\n";
cout << "Pradine bendra pinigu suma: " << initialTotal <<"\n";
cout << "Galutine bendra pinigu suma: " << finalTotal << "\n";
cout << "Bendra suma nepakito: " << (initialTotal == finalTotal? "TAIP": "NE")<< "\n";
cout << "Bloku iskaitant genesis: " << blockchain.size() << "\n";
cout << "I blockchain ideta transakciju: " << validation.accepted.size() << "\n";
cout << "Blockchain validi: " << (valid ? "TAIP" : "NE") << "\n";

return 0;
}