#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdint>
#include <limits>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "blockchain.h"
#include "merkle.h"
#include "hash.h"


using namespace std;

void TestMerkleCases(
const vector<Transaction>& transactions) {
cout << "\n";
cout << "                    \n";
cout << "Merkle medžio testai\n";
cout << "                    \n";

vector<int> testSizes = {1, 2, 3, 5};

for (int count : testSizes) {
if (transactions.size() < static_cast<size_t>(count)) {
continue; }

vector<Transaction> testTransactions(transactions.begin(),transactions.begin() + count);

string root = CalculateMerkleRoot(testTransactions);

cout << "\n" << "Transakcijų skaičius: " << count << "\n";

cout <<"Merkle šaknis:"<< root << "\n"; }

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
cout <<"Sena šaknis: "<< rootOriginal << "\n";
cout<< "Nauja šaknis: " << rootChanged << "\n";
cout << "Šaknis pasikeitė: " << (rootOriginal != rootChanged ? "TAIP" : "NE")<< "\n"; }

if (transactions.size() >= 2) {
vector<Transaction> original = {
transactions[0], transactions[1] };
string rootOriginal = CalculateMerkleRoot(original);

vector<Transaction> reversed = {
transactions[1], transactions[0] };
string rootReversed = CalculateMerkleRoot(reversed);

cout <<"\n";
cout << "Transakcijų tvarkos pakeitimo testas:\n";
cout << "Sena šaknis: "<< rootOriginal << "\n";
cout << "Nauja šaknis: " << rootReversed << "\n";
cout << "Šaknis pasikeitė: "<< (rootOriginal != rootReversed ? "TAIP" : "NE") << "\n"; } }

void TestTransactionValidation() {
cout << "\nTransakcijų validavimo testai: \n";
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
cout <<"Galiojanti transakcija: " << (result.accepted.size() == 1 ? "TAIP" : "NE") << "\n";

Transaction unknownSender = validTx;
unknownSender.senderId = "user_9999";
data = unknownSender.senderId + "|" + unknownSender.receiverId + "|" + to_string(unknownSender.amount) + "|" + to_string(unknownSender.sequence);
unknownSender.transactionId = HashHex(data);
result = ValidateTransactions(users, {unknownSender});
cout<<"Nežinomas siuntėjas: " << (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n";

Transaction unknownReceiver = validTx;
unknownReceiver.receiverId = "user_9999";
data = unknownReceiver.senderId + "|" + unknownReceiver.receiverId + "|" + to_string(unknownReceiver.amount) + "|" + to_string(unknownReceiver.sequence);
unknownReceiver.transactionId = HashHex(data);
result = ValidateTransactions(users, {unknownReceiver});
cout << "Nežinomas gavėjas: "<< (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n";

Transaction zeroAmount = validTx;
zeroAmount.amount = 0;
data = zeroAmount.senderId + "|" + zeroAmount.receiverId + "|" + to_string(zeroAmount.amount) + "|" + to_string(zeroAmount.sequence);
zeroAmount.transactionId = HashHex(data);
result = ValidateTransactions(users, {zeroAmount});
cout <<"Neteisinga suma (0): " << (result.rejected.size() == 1 ? "TAIP" :"NE") << "\n";

Transaction invalidId = validTx;
invalidId.transactionId = "1234567890abcdef";
result= ValidateTransactions(users, {invalidId});
cout << "Neteisingas transakcijos ID: " << (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n";

Transaction insufficient = validTx;
insufficient.amount = 5000;
data = insufficient.senderId + "|" + insufficient.receiverId + "|" + to_string(insufficient.amount) + "|" + to_string(insufficient.sequence);
insufficient.transactionId = HashHex(data);
result = ValidateTransactions(users, {insufficient});
cout << "Nepakankamas balansas: "<< (result.rejected.size() == 1 ? "TAIP": "NE")<< "\n";

Transaction duplicate1 = validTx;
Transaction duplicate2 = validTx;
duplicate2.sequence = 2;
duplicate2.transactionId = duplicate1.transactionId;
result = ValidateTransactions(users, {duplicate1, duplicate2});
cout << "Pasikartojantis transakcijos ID: " << (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n"; 

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
cout << "Sveikojo skaičiaus perpildymas: " << (result.rejected.size() == 1 ? "TAIP" : "NE") << "\n"; }



int main() {

#ifdef _WIN32
SetConsoleOutputCP(CP_UTF8);
#endif

cout << "                    \n";
cout << "Supaprastinta blokų grandinė: \n";
cout << "Merkle medis: \n";
cout << "                    \n";

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
cout << "                    \n";
cout << "Blokų grandinė: \n";
cout << "                    \n";

for (const auto& block : blockchain) {
cout << "\n";
cout << "Bloko aukštis: " << block.height << "\n";
cout << "Transakcijų skaičius: " << block.transactions.size() << "\n";
cout << "Ankstesnio bloko maiša: " << block.prevHash << "\n";
cout << "Merkle šaknis: " << block.txRoot << "\n";
cout << "Bloko maiša: " << block.blockHash << "\n";
cout << "Sudėtingumas: "<< block.difficulty << "\n";
cout << "Nonce reikšmė: " << block.nonce << "\n";
cout << "Kasimo bandymų skaičius: "<< block.miningAttempts << "\n";
cout <<fixed<< setprecision(6);
cout << "Kasimo trukmė: "<< block.miningTime << " s\n"; }

bool valid = VerifyMerkleBlockchain(blockchain);

int64_t initialTotal= 0;
int64_t finalTotal = 0;

for (const auto& user : users) {
initialTotal += user.balance; }

for (const auto& user : validation.finalUsers) {
finalTotal += user.balance; }

cout << "\n";
cout << "                    \n";
cout << "GALUTINĖ SUVESTINĖ\n";
cout << "                    \n";
cout << "Vartotojų skaičius: "<< users.size() << "\n";
cout << "Sugeneruotų transakcijų: " << transactions.size() << "\n";
cout << "Priimtų transakcijų: " << validation.accepted.size() << "\n";
cout << "Atmestų transakcijų: "<< validation.rejected.size() << "\n";
cout << "Pradinė bendra pinigų suma: " << initialTotal <<"\n";
cout << "Galutinė bendra pinigų suma: " << finalTotal << "\n";
cout << "Bendra suma nepakito: " << (initialTotal == finalTotal? "TAIP": "NE")<< "\n";
cout << "Blokų, įskaitant pradinį bloką: " << blockchain.size() << "\n";
cout << "Į blokų grandinę įtrauktų transakcijų: " << validation.accepted.size() << "\n";
cout << "Blokų grandinė galiojanti: " << (valid ? "TAIP" : "NE") << "\n";

return 0;
}