#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "hash.h"

using namespace std;

constexpr int DIFFICULTY = 1;
constexpr unsigned THREADS = 4;
constexpr uint64_t MAX_NONCE = 200000;

struct Result {
uint64_t nonce, attempts;
string hash;
double seconds;
bool found;
Result() : nonce(0), attempts(0),seconds(0), found(false){} };

bool valid(const string& hash) {
return hash.size() >= DIFFICULTY && hash.compare(0, DIFFICULTY, string(DIFFICULTY, '0')) == 0; }

Result mine(const string& header) {
Result result;
const auto start = chrono::steady_clock::now();

for (uint64_t nonce = 0; nonce <= MAX_NONCE; ++nonce) {
string hash = HashHex(header + to_string(nonce));
++result.attempts;
if (valid(hash)) {
result.nonce = nonce;
result.hash = hash;
result.found = true;
break; } }

result.seconds = chrono::duration<double> (chrono::steady_clock::now() - start).count();
return result; }

Result mineParallel(const string& header) {
atomic<bool> stop(false);
atomic<uint64_t> attempts(0);
Result winner;
vector<thread> workers;
const auto start = chrono::steady_clock::now();

for (unsigned id = 0; id < THREADS; ++id) {
workers.emplace_back([&, id] {

for (uint64_t nonce = id; nonce <= MAX_NONCE; nonce += THREADS) {
if (stop.load()) return;
string hash = HashHex(header + to_string(nonce));
++attempts;
if (valid(hash)) {
bool expected = false;
if (stop.compare_exchange_strong(expected, true)) {
winner.nonce = nonce;
winner.hash = hash;
winner.found = true; }
return; } } }); }

for (thread& worker : workers) worker.join();
winner.attempts = attempts.load();
winner.seconds = chrono::duration<double>(chrono::steady_clock::now() - start).count();
return winner; }

void print(const char* label, const Result& result) {
cout << label << ": " << (result.found ? "rastas": "nerastas") << ", nonce="<< result.nonce << ", bandymai=" << result.attempts << ", laikas=" << fixed << setprecision(6) << result.seconds << " s\n"; }

int main() {
const string previousHash = "0001ca64d8901030ece549d0030470b429cd846432f6f670847570a4f1c1dd50";
const string header = "2|5|" + previousHash + "|" + HashHex("candidate_5") + "|1731000000|";
cout << "Lygiagretus kasimas v0.2 | gijos=" << THREADS << " | difficulty=" << DIFFICULTY << " | nonce=0.." << MAX_NONCE << "\n";

Result sequential = mine(header);
Result parallel = mineParallel(header);
const bool ok = sequential.found && parallel.found &&
valid(sequential.hash) && valid(parallel.hash) &&
HashHex(header + to_string(sequential.nonce)) == sequential.hash &&
HashHex(header + to_string(parallel.nonce)) == parallel.hash;

cout << "\nTas pats kandidatas ir nonce intervalas: " << (ok ? "patikrinta [OK]\n" : "patikra nepavyko [FAIL]\n");
print("Nuoseklus", sequential);
print("Lygiagretus", parallel);
if (parallel.seconds > 0)

cout << "Pagreitis: " << sequential.seconds / parallel.seconds << "x\n";

return ok ? 0 :1; }
