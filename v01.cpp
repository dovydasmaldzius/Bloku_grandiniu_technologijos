#include <iostream>
#include <string>
#include <iomanip>
#include <cstdint>
#include <chrono>
#include <fstream>
#include <iterator>
#include <windows.h>

using namespace std;



void HashFunkcija(const string& zodis, uint32_t hash[8]) { //& - kad nereikėtų papildomai kopijuoti teksto; 
//uint32_t - 32 bitų sveikasis skaičius, nes hash funkcija grąžina 8 skaičius po 32 bitus

const uint32_t daugikliai[8] = {31, 37, 41, 43, 47, 53, 59, 61};

for (int i = 0; i < zodis.length(); i++) { //tol, kol galima skaityti simbolius
    uint32_t simbolis = static_cast<unsigned char>(zodis[i]); //simbolis paverčiamas skaitine reikšme (ascii kodu)

for (int j = 0; j < 8; j++) { //kiekvienas hash turi 32 bitus, todėl reikia 8 kartus atlikti skaičiavimus
    hash[j] = hash[j] * daugikliai[j] + simbolis + static_cast<uint32_t>(i + 1) * (j + 1); //formulė
//pavyzdžiui, jei simbolis yra 'a', tai jo ascii kodas yra 97, o hash[0] bus 0 * 31 + 97 + 1 * 1 = 98
//hash[1] bus 0 * 37 + 97 + 1 * 2 = 99 ir t.t.
//toliau visi simboliai bus skaičiuojami pagal tą pačią formulę ir kaskart atsinaujins hash reikšmė
}
}
}



int main() {

SetConsoleOutputCP(CP_UTF8);
SetConsoleCP(CP_UTF8);

string zodis;
string pasirinkimas;

cout << "Kaip norite įvesti žodį arba tekstą?\n";
cout << "1 - įvesti ranka\n";
cout << "2 - nuskaityti iš failo\n";
cout << "Pasirinkimas: ";
getline(cin, pasirinkimas);

if (pasirinkimas == "2") {
    ifstream failas("abc.txt");
if (!failas) {
    cerr << "Nepavyko atidaryti failo" << endl;
return 1;
}

zodis.assign(istreambuf_iterator<char>(failas), istreambuf_iterator<char>()); //visas failo turinys paverčiamas į string tipo kintamąjį zodis
//istreambuf_iterator<char> - iteratorius, kuris skaito simbolius iš failo
//istreambuf_iterator<char>() - iteratorius, kuris žymi failo pabaigą
uint32_t hash[8] = {}; //sukuriamas 8 elementų masyvas, kolkas užpildytas nuliais

auto start = chrono::steady_clock::now();

HashFunkcija(zodis, hash);

auto end = chrono::steady_clock::now();

chrono::duration<double> laikas = end - start;

cout << "hash: ";
cout << hex << setfill('0'); //tuščias vietas užpildome nuliais
for (int i = 0; i < 8; i++) {
    cout << setw(8) << hash[i];
}
cout << endl << endl;
cout << "Hashavimo laikas: " << fixed << setprecision(6) << laikas.count() << " sekundžių" << endl;
cout << "Programa baigta" << endl;
return 0;
}

if (pasirinkimas != "1") {
    cerr << "Prašome paritinkti tik 1 arba 2!" << endl;
return 1;
}

while (true) {
cout << "įveskite žodį arba tekstą (b - baigti darbą): ";
    getline(cin, zodis);
if (zodis == "b") {
break;
}

uint32_t hash[8] = {};

auto start = chrono::steady_clock::now();

HashFunkcija(zodis, hash);

auto end = chrono::steady_clock::now();

chrono::duration<double> laikas = end - start;

cout << "žodis/tekstas: " << zodis << endl;
cout << "hash: ";
cout << hex << setfill('0');

for (int i = 0; i < 8; i++) {
cout << setw(8) << hash[i];
}
cout << endl;
cout << "Hashavimo laikas: " << fixed << setprecision(10) << laikas.count() << " s" << endl << endl;
}

return 0;
}