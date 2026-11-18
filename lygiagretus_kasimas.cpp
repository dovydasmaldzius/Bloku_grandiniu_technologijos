#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#include "hash.h"

using namespace std;

constexpr int DIFFICULTY = 1; // kiek nulių turi būti bloko maišoje
constexpr unsigned CANDIDATES = 5; // kiek kandidatų bus kasama vienu metu
constexpr unsigned THREADS = 5; // po vieną kandidatą kiekvienoje gijoje
constexpr uint64_t MAX_ATTEMPTS = 200000; // bandymų biudžetas vienam kandidatui

struct Result { // rezultatas, gautas kasant kandidatą
uint64_t nonce, attempts;
string hash;
double seconds;
bool found;
Result() : nonce(0), attempts(0),seconds(0), found(false){} };

bool valid(const string& hash) { // patikrinama, ar maiša atitinka sudėtingumą
return hash.size() >= DIFFICULTY && hash.compare(0, DIFFICULTY, string(DIFFICULTY, '0')) == 0; }

Result mine(const string& header) { // nuoseklus vieno kandidato kasimas
Result result;
const auto start = chrono::steady_clock::now();
for (uint64_t nonce = 0; nonce < MAX_ATTEMPTS; ++nonce) {
string hash = HashHex(header + to_string(nonce));
++result.attempts;
if (valid(hash)) {
result.nonce = nonce;
result.hash = hash;
result.found = true;
break; } }

result.seconds = chrono::duration<double> (chrono::steady_clock::now() - start).count();
return result; }

Result mineParallelCandidate(const string& header) { // viena kandidato lygiagretus kasimas
Result result;
const auto start = chrono::steady_clock::now();
for (uint64_t nonce = 0; nonce < MAX_ATTEMPTS; ++nonce) {
string hash = HashHex(header + to_string(nonce));
++result.attempts;
if (valid(hash)) {
result.nonce = nonce;
result.hash = hash;
result.found = true;
break; } }

result.seconds = chrono::duration<double>(chrono::steady_clock::now() - start).count();
return result; }

vector<Result> mineAllCandidates( // visų kandidatų lygiagretus kasimas
const vector<string>& headers,
uint64_t firstNonce,
uint64_t lastNonce) {
atomic<unsigned> nextCandidate(0);
vector<Result> results(headers.size());
vector<thread> workers;
const unsigned workerCount = min<unsigned>(THREADS, static_cast<unsigned>(headers.size()));
for (unsigned id = 0; id < workerCount; ++id) {
workers.emplace_back([&, id] {
for (;;) {
const unsigned candidate = nextCandidate.fetch_add(1);
if (candidate >= headers.size()) return;
const auto start = chrono::steady_clock::now();
Result result;
for (uint64_t nonce = firstNonce; nonce < lastNonce; ++nonce) {
string hash = HashHex(headers[candidate] + to_string(nonce));
++result.attempts;
if (valid(hash)) {
result.nonce = nonce;
result.hash = hash;
result.found = true;
break; } }
result.seconds = chrono::duration<double>(chrono::steady_clock::now() - start).count();
results[candidate] = result;
} }); }
for (thread& worker : workers) worker.join();
return results; }

void print(const char* label, const Result& result) {
if (label && label[0] != '\0') cout << label << ": ";
cout << (result.found ? "rastas": "nerastas") << ", nonce reikšmė = "<< result.nonce << ", bandymų skaičius = " << result.attempts << ", trukmė = " << fixed << setprecision(6) << result.seconds << " s\n"; }

int main() {
#ifdef _WIN32
SetConsoleOutputCP(CP_UTF8);
#endif

const string previousHash = "0001ca64d8901030ece549d0030470b429cd846432f6f670847570a4f1c1dd50";
vector<string> headers;
for (unsigned i = 1; i <= CANDIDATES; ++i) {
const string txRoot = HashHex("candidate_" + to_string(i));
headers.push_back("2|100|" + previousHash + "|" + txRoot + "|1731000000|");
}

cout << "Lygiagretus kasimas v0.2 | kandidatų = " << CANDIDATES
<< " | gijų = " << THREADS << " | sudėtingumas = " << DIFFICULTY
<< " | bandymų biudžetas/kandidatui = " << MAX_ATTEMPTS << "\n";
cout << "Taisyklė: tikrinimo tvarka – Kandidatas 1, 2, 3, 4, 5; laimėtojas – pirmasis patvirtintas kandidatas.\n";

vector<Result> results = mineAllCandidates(headers, 0, MAX_ATTEMPTS);
bool initialWinnerFound = false;
for (const Result& result : results) {
if (result.found) { initialWinnerFound = true; break; } }
if (!initialWinnerFound) {
cout << "Pirminis 200000 nonce biudžetas išnaudotas; tęsiama nuo nonce 200000 iki 400000.\n";
vector<Result> continuation = mineAllCandidates(
headers, MAX_ATTEMPTS, MAX_ATTEMPTS * 2);
for (size_t i = 0; i < results.size(); ++i) {
const double combinedSeconds = results[i].seconds + continuation[i].seconds;
const uint64_t combinedAttempts = results[i].attempts + continuation[i].attempts;
if (continuation[i].found) {
results[i] = continuation[i];
results[i].seconds = combinedSeconds;
results[i].attempts = combinedAttempts;
} else {
results[i].seconds = combinedSeconds;
results[i].attempts = combinedAttempts;
}
}
}
size_t winner = CANDIDATES;
bool ok = true;
double totalParallelSeconds = 0.0;
for (unsigned i = 0; i < CANDIDATES; ++i) {
cout << "Kandidatas " << i + 1 << " | ";
print("", results[i]);
totalParallelSeconds += results[i].seconds;
if (winner == CANDIDATES && results[i].found) winner = i;
if (results[i].found) {
const string calculated = HashHex(headers[i] + to_string(results[i].nonce));
ok = ok && valid(results[i].hash) && calculated == results[i].hash;
}
}

if (winner < CANDIDATES) {
cout << "Pasirinktas laimėtojas: Kandidatas " << winner + 1
<< ", nonce = " << results[winner].nonce << ", maiša = " << results[winner].hash << "\n";
cout << "Nelaimėjusių kandidatų balanso ir pool poveikio nėra.\n";
} else {
cout << "Laimėtojas nerastas per bendrą kandidatų biudžetą.\n";
ok = false;
}

return ok ? 0 : 1; }
