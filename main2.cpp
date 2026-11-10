#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdint>
#include <limits>

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

void TestTransactionValidation() {
cout << "\n========== V0.2 TRANSAKCIJU VALIDAVIMO TESTAI ==========\n";
vector<User> users = {
{"user_0001", "key1", 1000},
{"user_0002", "key2", 500},
{"user_0003", "key3", 100} };

Transaction validTx;
validTx.senderId = "user_0001";
validTx.receiverId = "user_0002";
validTx.amount = 100;
validTx.sequence = 1;
string data = validTx.senderId + "|" +
validTx.receiverId + "|" + to_string(validTx.amount) + "|" + to_string(validTx.sequence);
validTx.transactionId = HashHex(data);

auto result = ValidateTransactions(users, {validTx});
cout <<"Teisinga transakcija: " << (result.accepted.size() == 1 ? "TAIP" : "NE") << "\n";

Transaction unknownSender = validTx;
unknownSender.senderId = "user_9999";
data = unknownSender.senderId + "|" + unknownSender.receiverId + "|" + to_string(unknownSender.amount) + "|" + to_string(unknownSender.sequence);
unknownSender.transactionId = HashHex(data);
result = ValidateTransactions(users, {unknownSender});
cout<< "Nezinomas siuntejas: " << (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n";

Transaction unknownReceiver = validTx;
unknownReceiver.receiverId = "user_9999";
data = unknownReceiver.senderId + "|" + unknownReceiver.receiverId + "|" + to_string(unknownReceiver.amount) + "|" + to_string(unknownReceiver.sequence);
unknownReceiver.transactionId = HashHex(data);
result = ValidateTransactions(users, {unknownReceiver});
cout << "Nezinomas gavejas: "<< (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n";

Transaction zeroAmount = validTx;
zeroAmount.amount = 0;
data = zeroAmount.senderId + "|" + zeroAmount.receiverId + "|" + to_string(zeroAmount.amount) + "|" + to_string(zeroAmount.sequence);
zeroAmount.transactionId = HashHex(data);
result = ValidateTransactions(users, {zeroAmount});
cout << "Neteisinga suma (0): " << (result.rejected.size() == 1 ? "TAIP" :"NE") << "\n";

Transaction invalidId = validTx;
invalidId.transactionId = "1234567890abcdef";
result = ValidateTransactions(users, {invalidId});
cout << "Neteisingas transaction ID: " << (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n";

Transaction insufficient = validTx;
insufficient.amount = 5000;
data = insufficient.senderId + "|" + insufficient.receiverId + "|" + to_string(insufficient.amount) + "|" + to_string(insufficient.sequence);
insufficient.transactionId = HashHex(data);
result = ValidateTransactions(users, {insufficient});
cout << "Nepakankamas balansas: " << (result.rejected.size() == 1 ? "TAIP": "NE")<< "\n";

Transaction duplicate1 = validTx;
Transaction duplicate2 = validTx;
duplicate2.sequence = 2;
duplicate2.transactionId = duplicate1.transactionId;
result = ValidateTransactions(users, {duplicate1, duplicate2});
cout << "Dubliuotas transaction ID: " << (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n"; 

vector<User> overflowUsers = {
{"user_0001", "key1", 1000},
{"user_0002", "key2", numeric_limits<int64_t>::max()} };

Transaction overflowTx;
overflowTx.senderId = "user_0001";
overflowTx.receiverId = "user_0002";
overflowTx.amount = 100;
overflowTx.sequence = 1;
data = overflowTx.senderId + "|" + overflowTx.receiverId + "|" + to_string(overflowTx.amount) + "|" + to_string(overflowTx.sequence);
overflowTx.transactionId = HashHex(data);
result = ValidateTransactions(overflowUsers, {overflowTx});
cout << "Integer overflow: " << (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n"; }



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

TestTransactionValidation();
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