# Bloku_grandiniu_technologijos

Šiam darbui naudotos maišos funkcijos repozitorija: Github dovydasmaldzius Bloku_Grandiniu_Technologijos v0.1.2 (release), failo pavadinimas: v01.cpp (versija be dirbtinio intelekto).

Naudoti dirbtinio intelekto įrankiai: ChatGPT; Google GEMINI; COPILOT (šis įrankis naudotas v0.3 užduoties papildomo kodo generavimui)

Programos paleidimo instrukcija: 

v0.1: terminale įrašomos eilutės: g++ -std=c++17 main.cpp hash.cpp blockchain.cpp -o v0.1.exe ir .\v0.1.exe

v0.2: terminale įrašomos eilutės: 

v0.3: terminale įrašomos eilutės: 

lygiagretus_kasimas (papildoma užduotis): terminale įrašomos eilutės: 

Modelio aprašymas: 



v0.1 užduotis
Užduoties tikslas: 

Pastaba: naudotas kasimo sudėtingumas - 3.

Darbo įrodymai iš kodų:

Galima atkurti vartotojus, transakcijas, pradinę būseną ir testų rezultatus:

Balansai neneigiami, lėšų suma išsaugoma, ta pati transakcija neįtraukiama pakartotinai:

PoW ir transakcijų duomenys patikrinami prieš įtraukiant bloką: 

Pavyzdys: kelių blokų išvestys (imti pirmasis ir antrasis blokai):

<img width="832" height="443" alt="image" src="https://github.com/user-attachments/assets/e995b3f6-39ac-49d1-9416-ad8b4834695e" />

Kiekvieną bloką sudaro: 

Kodėl iš viso buvo sugeneruoti 48 skirtingi blokai (nuo nulinio iki 47-o)? Nes buvo sugeneruota 10000 transakcijų, bet į blokų grandines priimtos tik 4625 transakcijos, 46 blokai turi po 100 transakcijų, o paskutinysis blokas turi 25.

Kodėl į blokų grandines buco priimtos tik 4625 transakcijos iš 10000? Nes 

Galutinė suvestinė:

<img width="357" height="418" alt="image" src="https://github.com/user-attachments/assets/23f2a3f9-f41d-4091-b143-dcbc5aa07b17" />

Galutinė pinigų suma nepasikeitė, nes nėra papildomai skaičiuojama komisinių ar kasimo atlygiai.




v0.2 užduotis:
Užduoties tikslas: 

