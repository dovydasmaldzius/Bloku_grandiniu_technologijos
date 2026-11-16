# Bloku_grandiniu_technologijos

Šiam darbui naudotos maišos funkcijos repozitorija: Github dovydasmaldzius Bloku_Grandiniu_Technologijos v0.1.2 (release), failo pavadinimas: v01.cpp (versija be dirbtinio intelekto).

Naudoti dirbtinio intelekto įrankiai: ChatGPT; Google GEMINI; COPILOT (šis įrankis naudotas v0.3 užduoties papildomo kodo generavimui)

Programos paleidimo instrukcija: 

v0.1: terminale įrašomos eilutės: g++ -std=c++17 main.cpp hash.cpp blockchain.cpp -o v0.1.exe ir .\v0.1.exe;

v0.2: terminale įrašomos eilutės: g++ -std=c++17 main2.cpp hash.cpp blockchain.cpp merkle.cpp -o v0.2.exe ir .\v0.2.exe;

v0.3: terminale įrašomos eilutės: g++ -std=c++17 -O2 -Wall -Wextra -pedantic v0.3.cpp blockchain.cpp hash.cpp merkle.cpp -o v0.3.exe ir .\v0.3.exe;

lygiagretus_kasimas (papildoma užduotis): terminale įrašomos eilutės: g++ -std=c++11 -O2 -pthread lygiagretus_kasimas.cpp hash.cpp -o lygiagretus_kasimas.exe ir .\lygiagretus_kasimas.exe;

Modelio aprašymas: 



v0.1 užduotis
Užduoties tikslas: 

Pastaba: naudotas kasimo sudėtingumas - 3.

Darbo įrodymai iš kodų:

Galima atkurti vartotojus, transakcijas, pradinę būseną ir testų rezultatus:

<img width="455" height="196" alt="image" src="https://github.com/user-attachments/assets/0f813fd7-cb62-4d73-b287-bc429d0737b6" />
<img width="665" height="88" alt="image" src="https://github.com/user-attachments/assets/3eabfa49-d736-4b5f-8552-d39e12101869" />
<img width="396" height="115" alt="image" src="https://github.com/user-attachments/assets/53f42257-e540-463c-9b4e-0457f3f021d7" />

(Naudojant tą pačią pradinę reikšmę galima atkurti tuos pačius rezultatus)

Balansai neneigiami, lėšų suma išsaugoma, ta pati transakcija neįtraukiama pakartotinai:

PoW ir transakcijų duomenys patikrinami prieš įtraukiant bloką: 

Pavyzdys: kelių blokų išvestys (imti pirmasis ir antrasis blokai):

<img width="832" height="443" alt="image" src="https://github.com/user-attachments/assets/e995b3f6-39ac-49d1-9416-ad8b4834695e" />

Kiekvieną bloką sudaro: 

Kodėl iš viso buvo sugeneruoti 48 skirtingi blokai (nuo nulinio iki 47-o)? Nes buvo sugeneruota 10000 transakcijų, bet į blokų grandines priimtos tik 4625 transakcijos, 46 blokai turi po 100 transakcijų, o paskutinysis blokas turi 25.

Kodėl į blokų grandines buco priimtos tik 4625 transakcijos iš 10000? Nes transakcijose buvo tikrinama, ar egzistuoja siuntėjas/gavėjas, ar tai nėra tas pats asmuo, ar pervedama teigiama suma, ar transakcijos ID yra teisingas ir nesikartoja, tad neatitikus šių kriterijų buvo atmestos likusios transakcijos (paprasčiau tariant - kiekvienos transakcijos suma buvo parenkama atsitiktinai, tačiau ir vartotojų turimi pinigai taip pat buvo parenkami atsitiktinai, tad daugeliui sugeneruotų vartotojų paprasčiausiai neužtekdavo lėšų pervesti jas kitam vartotojui, taip būdavo atmetama transakcija)

Galutinė suvestinė:

<img width="357" height="418" alt="image" src="https://github.com/user-attachments/assets/23f2a3f9-f41d-4091-b143-dcbc5aa07b17" />

Galutinė pinigų suma nepasikeitė, nes nėra papildomai skaičiuojama komisinių ar kasimo atlygiai.




v0.2 užduotis:
Užduoties tikslas: 

