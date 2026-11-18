#include <iostream>
#include <cstdint>
#include <iomanip>
#include <chrono>
#ifdef _WIN32
#include <windows.h>
#endif
#include "hash.h"
#include "blockchain.h"

using namespace std;

int main() {
#ifdef _WIN32 
SetConsoleOutputCP(CP_UTF8);
#endif

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

cout << "                             \n";
cout << "Supaprastinta blokų grandinė:\n";
cout << "                             \n";
cout << "Vartotojų skaičius: " << users.size() << '\n';
cout << "Sugeneruotų transakcijų skaičius: " <<transactions.size()<< '\n';
cout << "Priimtų transakcijų: " << validation.accepted.size() << '\n';
cout << "Atmestų transakcijų: "<< validation.rejected.size() << '\n';
cout << "Pradinė bendra pinigų suma: " << initialFunds << '\n';
cout << "Galutinė bendra pinigų suma: " << finalFunds << '\n';
cout << "Bendra pinigų suma nepasikeitė: " << (initialFunds == finalFunds? "TAIP" : "NE") << "\n\n";
cout << "                             \n";
cout << "Blokų grandinė:\n";
cout << "                             \n";

for (const auto& block : blockchain) {
cout << "\nBloko aukštis: " << block.height << '\n';
cout << "Transakcijų skaičius: " << block.transactions.size() << '\n';
cout << "Ankstesnio bloko maiša: " << block.prevHash << '\n';
cout << "Merkle šaknis: "<< block.txRoot << '\n';
cout << "Bloko maiša: " << block.blockHash << '\n';
cout << "Sudėtingumas: " << block.difficulty << '\n';
cout << "Nonce reikšmė: " << block.nonce << '\n';
cout << "Kasimo bandymų skaičius: " << block.miningAttempts << '\n';
cout << "Kasimo trukmė: " << fixed << setprecision(6) << block.miningTime << " s\n"; }

bool blockchainValid = VerifyBlockchain(blockchain);

cout <<"\n                            \n";
cout << "Grandinės tikrinimas:\n";
cout << "Blokų grandinė galiojanti: " << (blockchainValid ? "TAIP" : "NE") << '\n';
cout <<"\n                            \n";
cout << "Galutinė suvestinė:\n";
cout <<"\n                            \n";
cout << "Vartotojų: " << users.size() << '\n';
cout << "Sugeneruotų transakcijų: " << transactions.size() << '\n';
cout << "Priimtų transakcijų: " << validation.accepted.size() << '\n';
cout << "Atmestų transakcijų: " << validation.rejected.size() << '\n';
cout << "Blokų, įskaitant pradinį bloką: " << blockchain.size() << '\n';
cout << "Sudėtingumas: " << difficulty << '\n';
cout << "Pradinė pinigų suma: "<< initialFunds<< '\n';
cout << "Galutinė pinigų suma: " << finalFunds << '\n';
cout << "Pinigų suma nepasikeitė: " << (initialFunds == finalFunds ? "TAIP" : "NE") << '\n';
cout<< "Blokų grandinė galiojanti: "<< (blockchainValid ? "TAIP" : "NE") << '\n';
cout << "Visos programos veikimo trukmė: " << fixed << setprecision(6) << totalTime.count() << " s\n";

return 0;
}