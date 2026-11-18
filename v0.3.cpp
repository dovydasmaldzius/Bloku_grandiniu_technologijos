#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "blockchain.h"
#include "hash.h"
#include "merkle.h"

/*
AI review record
Tool: Copilot SDK in VS Code
Model: not exposed by this session
Date: 2026-11-11
Prompt summary: Review the existing v0.2 transaction validation and Merkle
implementation against the supplied correctness requirements; propose safe,
measurable improvements while keeping v0.2 files and behavior untouched.
Accepted: report a specific reason for every rejected transaction; validate
Merkle leaf hex and reuse one tree-level buffer instead of allocating a new
level at every depth.
Verification: executable checks cover rejection reasons, ordered balances,
duplicate IDs, block tampering/PoW, and Merkle sizes 1/2/3/5 against v0.2.
Rejected proposals: none.
Earlier-stage AI assistance: unknown from this session; disclose any prior use
separately if applicable.
*/

using namespace std;

struct RejectedTransactionV03 {
    Transaction transaction;
    string reason;
};

struct ValidationResultV03 {
    vector<Transaction> accepted;
    vector<RejectedTransactionV03> rejected;
    vector<User> finalUsers;
};

static string TransactionDataV03(const Transaction& tx) {
    return tx.senderId + "|" + tx.receiverId + "|" + to_string(tx.amount) +
        "|" + to_string(tx.sequence);
}

static Transaction MakeTransactionV03(
    const string& sender,
    const string& receiver,
    int64_t amount,
    uint64_t sequence) {
    Transaction tx;
    tx.senderId = sender;
    tx.receiverId = receiver;
    tx.amount = amount;
    tx.sequence = sequence;
    tx.transactionId = HashHex(TransactionDataV03(tx));
    return tx;
}

static ValidationResultV03 ValidateTransactionsV03(
    const vector<User>& users,
    const vector<Transaction>& transactions,
    const unordered_set<string>& confirmedIds = {}) {
    ValidationResultV03 result;
    result.finalUsers = users;

    unordered_map<string, size_t> userIndex;
    userIndex.reserve(users.size());
    bool duplicateUserId = false;
    for (size_t i = 0; i < users.size(); ++i) {
        if (!userIndex.emplace(users[i].id, i).second) {
            duplicateUserId = true;
        }
    }

    unordered_set<string> seenIds = confirmedIds;
    seenIds.reserve(confirmedIds.size() + transactions.size());

    const auto reject = [&result](const Transaction& tx, const string& reason) {
        result.rejected.push_back({tx, reason});
    };

    for (const Transaction& tx : transactions) {
        if (duplicateUserId) {
            reject(tx, "Pasikartojantis vartotojo ID neleidžia vienareikšmiškai rasti paskyros");
            continue;
        }

        const auto senderIt = userIndex.find(tx.senderId);
        if (senderIt == userIndex.end()) {
            reject(tx, "Nežinomas siuntėjas");
            continue;
        }

        const auto receiverIt = userIndex.find(tx.receiverId);
        if (receiverIt == userIndex.end()) {
            reject(tx, "Nežinomas gavėjas");
            continue;
        }

        if (tx.senderId == tx.receiverId) {
            reject(tx, "Siuntėjas ir gavėjas turi skirtis");
            continue;
        }

        if (tx.amount <= 0) {
            reject(tx, "Suma turi būti teigiama");
            continue;
        }

        if (tx.transactionId != HashHex(TransactionDataV03(tx))) {
            reject(tx, "Transakcijos ID neatitinka jos turinio");
            continue;
        }

        if (!seenIds.insert(tx.transactionId).second) {
            reject(tx, "Ši transakcija jau apdorota");
            continue;
        }

        User& sender = result.finalUsers[senderIt->second];
        User& receiver = result.finalUsers[receiverIt->second];
        if (sender.balance < tx.amount) {
            reject(tx, "Siuntėjo balanso nepakanka pagal vykdymo tvarką");
            continue;
        }

        if (receiver.balance > numeric_limits<int64_t>::max() - tx.amount) {
            reject(tx, "Gavėjo balanso reikšmė viršytų leistiną ribą");
            continue;
        }

        sender.balance -= tx.amount;
        receiver.balance += tx.amount;
        result.accepted.push_back(tx);
    }

    return result;
}

static bool DecodeHexV03(const string& hex, string& bytes) {
    if (hex.empty() || hex.size() % 2 != 0) {
        return false;
    }

    string decoded;
    decoded.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
        const unsigned char high = static_cast<unsigned char>(hex[i]);
        const unsigned char low = static_cast<unsigned char>(hex[i + 1]);
        if (!isxdigit(high) || !isxdigit(low)) {
            return false;
        }

        const auto digit = [](unsigned char ch) -> unsigned int {
            if (ch >= '0' && ch <= '9') return ch - '0';
            if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
            return ch - 'A' + 10;
        };
        decoded.push_back(static_cast<char>((digit(high) << 4) | digit(low)));
    }

    bytes = move(decoded);
    return true;
}

static string EncodeHexV03(const string& bytes) {
    static const char digits[] = "0123456789abcdef";
    string hex;
    hex.reserve(bytes.size() * 2);
    for (unsigned char byte : bytes) {
        hex.push_back(digits[byte >> 4]);
        hex.push_back(digits[byte & 0x0f]);
    }
    return hex;
}

static string CalculateMerkleRootV03(const vector<Transaction>& transactions) {
    if (transactions.empty()) {
        return HashHex("");
    }

    vector<string> level;
    level.reserve(transactions.size());
    for (const Transaction& tx : transactions) {
        string leaf;
        if (tx.transactionId.size() != 64 ||
            !DecodeHexV03(tx.transactionId, leaf)) {
            return "";
        }
        level.push_back(move(leaf));
    }

    while (level.size() > 1) {
        size_t output = 0;
        const size_t inputSize = level.size();
        for (size_t input = 0; input < inputSize; input += 2) {
            string combined;
            combined.reserve(level[input].size() * 2);
            combined.append(level[input]);
            combined.append(input + 1 < inputSize ? level[input + 1] : level[input]);

            string parent;
            if (!DecodeHexV03(HashHex(combined), parent)) {
                throw runtime_error("HashHex funkcija grąžino netinkamą šešioliktainę reikšmę");
            }
            level[output++] = move(parent);
        }
        level.resize(output);
    }

    return EncodeHexV03(level.front());
}

static string CalculateBlockHashV03(const Block& block) {
    const string header = block.prevHash + "|" + block.timestamp + "|" +
        to_string(block.version) + "|" + block.txRoot + "|" +
        to_string(block.nonce) + "|" + to_string(block.difficulty);
    return HashHex(header);
}

static bool HasValidProofV03(const Block& block) {
    if (block.difficulty < 0 || block.difficulty > 64 ||
        block.blockHash.size() < static_cast<size_t>(block.difficulty)) {
        return false;
    }
    return all_of(
        block.blockHash.begin(),
        block.blockHash.begin() + block.difficulty,
        [](char digit) { return digit == '0'; });
}

static Block MineBlockV03(
    int height,
    const string& previousHash,
    const vector<Transaction>& transactions,
    int difficulty,
    const string& timestamp) {
    if (difficulty < 0 || difficulty > 64) {
        throw invalid_argument("Sudėtingumas turi būti nuo 0 iki 64");
    }

    Block block{};
    block.height = height;
    block.prevHash = previousHash;
    block.timestamp = timestamp;
    block.version = 1;
    block.txRoot = CalculateMerkleRootV03(transactions);
    if (block.txRoot.empty()) {
        throw invalid_argument("Transakcijoje yra netinkamas ID");
    }
    block.transactions = transactions;
    block.difficulty = difficulty;

    while (true) {
        block.blockHash = CalculateBlockHashV03(block);
        ++block.miningAttempts;
        if (HasValidProofV03(block)) {
            break;
        }
        if (block.nonce == numeric_limits<uint64_t>::max()) {
            throw overflow_error("Nonce reikšmių sritis išnaudota neradus darbo įrodymo");
        }
        ++block.nonce;
    }

    return block;
}

static vector<Block> CreateBlockchainV03(
    const vector<Transaction>& transactions,
    int difficulty) {
    if (difficulty < 0 || difficulty > 64) {
        throw invalid_argument("Sudėtingumas turi būti nuo 0 iki 64");
    }

    vector<Block> chain;
    Block genesis{};
    genesis.height = 0;
    genesis.prevHash = string(64, '0');
    genesis.timestamp = "2026-01-01 00:00:00";
    genesis.version = 1;
    genesis.txRoot = HashHex("");
    genesis.difficulty = 0;
    genesis.blockHash = CalculateBlockHashV03(genesis);
    chain.push_back(move(genesis));

    constexpr size_t transactionsPerBlock = 100;
    chain.reserve(1 + (transactions.size() + transactionsPerBlock - 1) /
        transactionsPerBlock);
    for (size_t begin = 0; begin < transactions.size();
         begin += transactionsPerBlock) {
        const size_t end = min(begin + transactionsPerBlock, transactions.size());
        vector<Transaction> blockTransactions(
            transactions.begin() + begin, transactions.begin() + end);
        Block block = MineBlockV03(
            static_cast<int>(chain.size()),
            chain.back().blockHash,
            blockTransactions,
            difficulty,
            GetCurrentTimestamp());
        chain.push_back(move(block));
    }

    return chain;
}

static bool VerifyBlockV03(const Block& block) {
    if (block.difficulty < 0 || block.difficulty > 64 ||
        CalculateMerkleRootV03(block.transactions) != block.txRoot ||
        CalculateBlockHashV03(block) != block.blockHash) {
        return false;
    }
    for (const Transaction& tx : block.transactions) {
        if (tx.transactionId != HashHex(TransactionDataV03(tx))) {
            return false;
        }
    }
    return HasValidProofV03(block);
}

static bool VerifyBlockchainV03(const vector<Block>& chain) {
    if (chain.empty() || chain.front().height != 0 ||
        !VerifyBlockV03(chain.front())) {
        return false;
    }

    for (size_t i = 1; i < chain.size(); ++i) {
        if (chain[i].height != chain[i - 1].height + 1 ||
            chain[i].prevHash != chain[i - 1].blockHash ||
            !VerifyBlockV03(chain[i])) {
            return false;
        }
    }
    return true;
}

static bool RunCorrectnessTestsV03() {
    size_t failures = 0;
    const auto check = [&failures](const string& name, bool passed) {
        cout << (passed ? "[GERAI] " : "[KLAIDA] ") << name << '\n';
        failures += passed ? 0 : 1;
    };

    const vector<User> users = {
        {"user_0001", "key1", 1000},
        {"user_0002", "key2", 500},
        {"user_0003", "key3", 100}
    };
    const Transaction valid = MakeTransactionV03(
        "user_0001", "user_0002", 100, 1);
    const ValidationResultV03 validResult =
        ValidateTransactionsV03(users, {valid});
    check("Galiojantis mokėjimas įtraukiamas vieną kartą; likučiai ir suma teisingi",
        validResult.accepted.size() == 1 &&
        validResult.finalUsers[0].balance == 900 &&
        validResult.finalUsers[1].balance == 600 &&
        validResult.finalUsers[0].balance +
            validResult.finalUsers[1].balance +
            validResult.finalUsers[2].balance == 1600);

    const Transaction unknownReceiver = MakeTransactionV03(
        "user_0001", "unknown", 10, 2);
    const Transaction unknownSender = MakeTransactionV03(
        "unknown", "user_0002", 10, 5);
    const Transaction zeroAmount = MakeTransactionV03(
        "user_0001", "user_0002", 0, 3);
    const Transaction negativeAmount = MakeTransactionV03(
        "user_0001", "user_0002", -1, 4);
    Transaction changedId = valid;
    changedId.transactionId = "changed";
    const auto unknownResult = ValidateTransactionsV03(users, {unknownReceiver});
    const auto unknownSenderResult = ValidateTransactionsV03(users, {unknownSender});
    const auto zeroResult = ValidateTransactionsV03(users, {zeroAmount});
    const auto negativeResult = ValidateTransactionsV03(users, {negativeAmount});
    const auto changedIdResult = ValidateTransactionsV03(users, {changedId});
    check("Nežinomas gavėjas atmetamas nurodant priežastį",
        unknownResult.rejected.size() == 1 &&
        unknownResult.rejected[0].reason == "Nežinomas gavėjas");
    check("Nežinomas siuntėjas atmetamas nurodant priežastį",
        unknownSenderResult.rejected.size() == 1 &&
        unknownSenderResult.rejected[0].reason == "Nežinomas siuntėjas");
    check("Nulinės ir neigiamos sumos atmetamos nurodant priežastį",
        zeroResult.rejected.size() == 1 && !zeroResult.rejected[0].reason.empty() &&
        negativeResult.rejected.size() == 1 &&
        !negativeResult.rejected[0].reason.empty());
    check("Pakeistas transakcijos ID atmetamas nurodant priežastį",
        changedIdResult.rejected.size() == 1 &&
        !changedIdResult.rejected[0].reason.empty());

    const vector<User> limitedUsers = {
        {"sender", "key1", 100}, {"receiver", "key2", 0}
    };
    const Transaction firstSpend =
        MakeTransactionV03("sender", "receiver", 80, 1);
    const Transaction secondSpend =
        MakeTransactionV03("sender", "receiver", 30, 2);
    const auto orderedResult =
        ValidateTransactionsV03(limitedUsers, {firstSpend, secondSpend});
    check("Per didelės išlaidos tikrinamos pagal vykdymo tvarką",
        orderedResult.accepted.size() == 1 &&
        orderedResult.rejected.size() == 1 &&
        orderedResult.finalUsers[0].balance == 20);

    const auto duplicateResult = ValidateTransactionsV03(users, {valid, valid});
    const unordered_set<string> alreadyConfirmed = {valid.transactionId};
    const auto confirmedResult =
        ValidateTransactionsV03(users, {valid}, alreadyConfirmed);
    check("Pasikartojantys ir anksčiau patvirtinti ID neįskaitomi du kartus",
        duplicateResult.accepted.size() == 1 &&
        duplicateResult.rejected.size() == 1 &&
        confirmedResult.accepted.empty() &&
        confirmedResult.rejected.size() == 1);
    const vector<User> overflowUsers = {
        {"sender", "key1", 100}, {"receiver", "key2", numeric_limits<int64_t>::max()}
    };
    const Transaction overflowPayment =
        MakeTransactionV03("sender", "receiver", 1, 1);
    const auto overflowResult =
        ValidateTransactionsV03(overflowUsers, {overflowPayment});
    check("Gavėjo balanso perpildymas atmetamas",
        overflowResult.accepted.empty() && overflowResult.rejected.size() == 1);

    vector<Transaction> validMerkleTransactions;
    for (uint64_t i = 1; i <= 5; ++i) {
        validMerkleTransactions.push_back(
            MakeTransactionV03("a", "b", static_cast<int64_t>(i), i));
    }
    bool merkleMatchesV02 = true;
    for (size_t count : {size_t(1), size_t(2), size_t(3), size_t(5)}) {
        vector<Transaction> sample(
            validMerkleTransactions.begin(),
            validMerkleTransactions.begin() + count);
        merkleMatchesV02 = merkleMatchesV02 &&
            CalculateMerkleRootV03(sample) == CalculateMerkleRoot(sample);
    }
    check("Merkle šaknys su 1, 2, 3 ir 5 lapais sutampa su v0.2",
        merkleMatchesV02);

    Transaction malformedLeaf = valid;
    malformedLeaf.transactionId = "not-hex";
    check("Netinkamas Merkle medžio lapas atmetamas",
        CalculateMerkleRootV03({malformedLeaf}).empty());

    Block fixture{};
    fixture.height = 1;
    fixture.prevHash = string(64, 'a');
    fixture.timestamp = "2026-01-01 00:00:00";
    fixture.version = 1;
    fixture.txRoot = CalculateMerkleRootV03({valid});
    fixture.transactions = {valid};
    fixture.difficulty = 0;
    fixture.blockHash = CalculateBlockHashV03(fixture);
    check("Nepakeistas blokas patikrinamas", VerifyBlockV03(fixture));

    Block changedContents = fixture;
    changedContents.transactions[0].amount++;
    check("Pakeistas bloko turinys atmetamas",
        !VerifyBlockV03(changedContents));
    Block changedPreviousHash = fixture;
    changedPreviousHash.prevHash[0] = 'b';
    check("Pakeista ankstesnio bloko maiša atmetama",
        !VerifyBlockV03(changedPreviousHash));

    Block invalidProof = fixture;
    invalidProof.difficulty = 1;
    while (true) {
        invalidProof.blockHash = CalculateBlockHashV03(invalidProof);
        if (invalidProof.blockHash[0] != '0') {
            break;
        }
        ++invalidProof.nonce;
    }
    check("Blokas su teisinga maiša, bet negaliojančiu darbo įrodymu atmetamas",
        !VerifyBlockV03(invalidProof));

    const Block candidateA = MineBlockV03(
        1, string(64, 'a'), {valid}, 1, "2026-01-01 00:00:00");
    const Block candidateB = MineBlockV03(
        1, string(64, 'b'), {validMerkleTransactions[0]}, 1,
        "2026-01-01 00:00:00");
    check("Atskiri kasimo kandidatai išlaiko savo šaknis ir būseną",
        candidateA.txRoot == CalculateMerkleRootV03({valid}) &&
        candidateB.txRoot == CalculateMerkleRootV03({validMerkleTransactions[0]}) &&
        VerifyBlockV03(candidateA) && VerifyBlockV03(candidateB));

    cout << "Nepavykusių teisingumo testų: " << failures << '\n';
    return failures == 0;
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    cout << "v0.3 – peržiūra atlikta naudojant DI; išsaugota v0.2 etaloninė elgsena\n";
    if (!RunCorrectnessTestsV03()) {
        return 1;
    }
    if (argc > 1 && string(argv[1]) == "--tests-only") {
        return 0;
    }

    constexpr int userCount = 1000;
    constexpr int transactionCount = 10000;
    constexpr uint32_t userSeed = 12345;
    constexpr uint32_t transactionSeed = 12346;
    constexpr int difficulty = 3;

    const vector<User> users = GenerateUsers(userCount, userSeed);
    const vector<Transaction> transactions =
        GenerateTransactions(users, transactionCount, transactionSeed);
    const ValidationResultV03 validation =
        ValidateTransactionsV03(users, transactions);
    const vector<Block> chain =
        CreateBlockchainV03(validation.accepted, difficulty);

    int64_t initialTotal = 0;
    int64_t finalTotal = 0;
    for (const User& user : users) initialTotal += user.balance;
    for (const User& user : validation.finalUsers) finalTotal += user.balance;

    cout << "\nVartotojų: " << users.size()
         << "\nSugeneruotų transakcijų: " << transactions.size()
         << "\nPriimtų transakcijų: " << validation.accepted.size()
         << "\nAtmestų transakcijų: " << validation.rejected.size()
         << "\nPradinė suma: " << initialTotal
         << "\nGalutinė suma: " << finalTotal
         << "\nBlokų, įskaitant pradinį bloką: " << chain.size()
         << "\nSudėtingumas: " << difficulty
         << "\nBlokų grandinė galiojanti: "
         << (VerifyBlockchainV03(chain) ? "TAIP" : "NE") << '\n';

    return initialTotal == finalTotal && VerifyBlockchainV03(chain) ? 0 : 1;
}