#include <iostream>
#include <cstdint>
#include <iomanip>
#include <chrono>

#include "hash.h"
#include "blockchain.h"

using namespace std;

int main()
{
    const uint32_t userSeed = 12345;
    const uint32_t transactionSeed = 12346;

    const int userCount = 1000;
    const int transactionCount = 10000;

    const int difficulty = 3;

auto programStart = chrono::steady_clock::now();

auto users = GenerateUsers(userCount, userSeed );

auto transactions = GenerateTransactions(users, transactionCount, transactionSeed);

int64_t initialFunds = 0;

for (const auto& user : users) {
initialFunds += user.balance; }

auto validation = ValidateTransactions(users, transactions);

int64_t finalFunds = 0;

for (const auto& user : validation.finalUsers) {
finalFunds += user.balance; }

auto blockchain = CreateBlockchain(validation.accepted, difficulty);

auto programEnd = chrono::steady_clock::now();

chrono::duration<double>totalTime = programEnd - programStart;

cout << "==============================\n";
cout << "SUPAPRASTINTA BLOKU GRANDINE\n";
cout<< "==============================\n\n";
cout << "Vartotoju skaicius: " << users.size() << '\n';
cout << "Sugeneruotu transakciju: " <<transactions.size()<< '\n';
cout << "Priimtu transakciju: " << validation.accepted.size() << '\n';
cout << "Atmestu transakciju: "<< validation.rejected.size() << '\n';
cout << "Pradine bendra pinigu suma: " << initialFunds << '\n';
cout << "Galutine bendra pinigu suma: " << finalFunds << '\n';
cout << "Bendra suma nepakito: " << (initialFunds == finalFunds? "TAIP" : "NE") << "\n\n";
cout << "==============================\n";
cout << "BLOKU GRANDINE\n";
cout << "==============================\n";

for (const auto& block : blockchain) {
cout << "\nBloko aukstis: " << block.height << '\n';
cout << "Transakciju skaicius: " << block.transactions.size() << '\n';
cout << "Prev hash: " << block.prevHash << '\n';
cout << "Tx root: "<< block.txRoot << '\n';
cout << "Block hash: " << block.blockHash << '\n';
cout << "Difficulty: " << block.difficulty << '\n';
cout << "Nonce: " << block.nonce << '\n';
cout << "Mining attempts: " << block.miningAttempts << '\n';
cout << "Mining time: " << fixed << setprecision(6) << block.miningTime << " s\n"; }

bool blockchainValid = VerifyBlockchain(blockchain);

cout << "\n==============================\n";
cout << "GRANDINES TIKRINIMAS\n";
cout<< "==============================\n";
cout << "Blockchain validi: " << (blockchainValid ? "TAIP" : "NE") << '\n';
cout << "\n==============================\n";
cout << "GALUTINE SANTRAUKA\n";
cout << "==============================\n";
cout << "Vartotoju: " << users.size() << '\n';
cout << "Sugeneruotu transakciju: " << transactions.size() << '\n';
cout << "Priimtu transakciju: " << validation.accepted.size() << '\n';
cout << "Atmestu transakciju: " << validation.rejected.size() << '\n';
cout << "Bloku iskaitant genesis: " << blockchain.size() << '\n';
cout << "Difficulty: " << difficulty << '\n';
cout << "Pradine pinigu suma: "<< initialFunds<< '\n';
cout << "Galutine pinigu suma: " << finalFunds << '\n';
cout << "Pinigai nepradingo: " << (initialFunds == finalFunds ? "TAIP" : "NE") << '\n';
cout<< "Blockchain validi: "<< (blockchainValid ? "TAIP" : "NE") << '\n';
cout << "Visa programa truko: " << fixed << setprecision(6) << totalTime.count() << " s\n";

return 0;
}