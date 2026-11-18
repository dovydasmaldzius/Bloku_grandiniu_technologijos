# Bloku_grandiniu_technologijos

Šiam darbui naudotos maišos funkcijos repozitorija: Github dovydasmaldzius Bloku_Grandiniu_Technologijos v0.1.2 (release), failo pavadinimas: v01.cpp (versija be dirbtinio intelekto).

Naudoti dirbtinio intelekto įrankiai: ChatGPT; Google GEMINI; COPILOT (šis įrankis naudotas v0.3 užduoties papildomo kodo generavimui)

Programos paleidimo instrukcija: 

v0.1: terminale įrašomos eilutės: g++ -std=c++17 main.cpp hash.cpp blockchain.cpp -o v0.1.exe ir .\v0.1.exe;

v0.2: terminale įrašomos eilutės: g++ -std=c++17 main2.cpp hash.cpp blockchain.cpp merkle.cpp -o v0.2.exe ir .\v0.2.exe;

v0.3: terminale įrašomos eilutės: g++ -std=c++17 -O2 -Wall -Wextra -pedantic v0.3.cpp blockchain.cpp hash.cpp merkle.cpp -o v0.3.exe ir .\v0.3.exe;

lygiagretus_kasimas (papildoma užduotis): terminale įrašomos eilutės: g++ -std=c++11 -O2 -pthread lygiagretus_kasimas.cpp hash.cpp -o lygiagretus_kasimas.exe ir .\lygiagretus_kasimas.exe;

v0.1 užduotis
Užduoties tikslas: sukurti supaprastintą blokų grandinę, generuojančią vartotojus/transakcijas, formuojančią blokus, ieškančią PoW, tikrinančią duomenis ir atnaujinančią grandinės būseną. Šiai užduočiai sukurti failai: hash.h ir hash.cpp (ankstesnės užduoties hash funkcija, išskirstyta į dalis); blockchain.h ir blockchain.cpp; main.cpp.

Pastaba: naudotas kasimo sudėtingumas - 3.

Darbo įrodymai iš kodų:

Galima atkurti vartotojus, transakcijas, pradinę būseną ir testų rezultatus:

<img width="455" height="196" alt="image" src="https://github.com/user-attachments/assets/0f813fd7-cb62-4d73-b287-bc429d0737b6" />
<img width="665" height="88" alt="image" src="https://github.com/user-attachments/assets/3eabfa49-d736-4b5f-8552-d39e12101869" />
<img width="561" height="37" alt="image" src="https://github.com/user-attachments/assets/ba7db289-4df9-4960-813d-22c52a48a852" />

(Naudojant tą pačią pradinę reikšmę galima atkurti tuos pačius rezultatus)

Balansai neneigiami, lėšų suma išsaugoma, ta pati transakcija neįtraukiama pakartotinai:

<img width="590" height="90" alt="image" src="https://github.com/user-attachments/assets/ca8d05b0-07c0-4a96-9c9b-6afc52e9fe58" />
<img width="572" height="57" alt="image" src="https://github.com/user-attachments/assets/4ce5fe99-888c-4c60-b2cb-5f32e387de4a" />
<img width="807" height="93" alt="image" src="https://github.com/user-attachments/assets/90badebf-c3f6-47d1-bbe6-bc0df4a1f3e6" />

PoW ir transakcijų duomenys patikrinami prieš įtraukiant bloką:

<img width="608" height="215" alt="image" src="https://github.com/user-attachments/assets/f5ff1437-e8d3-48c9-921f-c4e6d5cb4d04" />
<img width="866" height="365" alt="image" src="https://github.com/user-attachments/assets/23c3ec51-a5f3-4cc0-9c7b-53e61c6d8319" />

(Parodoma, kad iš pirmo transakcijos turi būti patvirtintos, ir tik vėliau priimtos į bloką)

Genesis ("nulinis") blokas, jo aprašymas ir išvestis:

<img width="858" height="405" alt="image" src="https://github.com/user-attachments/assets/94b6746a-b653-4e76-aa85-360742f0a33b" />
<img width="821" height="215" alt="image" src="https://github.com/user-attachments/assets/b09addd9-66b7-44d5-93ea-92bbcd898be4" />


Pavyzdys: kelių blokų išvestys (imti pirmasis ir antrasis blokai):

<img width="832" height="443" alt="image" src="https://github.com/user-attachments/assets/e995b3f6-39ac-49d1-9416-ad8b4834695e" />

Kodėl iš viso buvo sugeneruoti 48 skirtingi blokai (nuo nulinio iki 47-o)? Nes buvo sugeneruota 10000 transakcijų, bet į blokų grandines priimtos tik 4625 transakcijos, 46 blokai turi po 100 transakcijų, o paskutinysis blokas turi 25.

Kodėl į blokų grandines buco priimtos tik 4625 transakcijos iš 10000? Nes transakcijose buvo tikrinama, ar egzistuoja siuntėjas/gavėjas, ar tai nėra tas pats asmuo, ar pervedama teigiama suma, ar transakcijos ID yra teisingas ir nesikartoja, tad neatitikus šių kriterijų buvo atmestos likusios transakcijos (paprasčiau tariant - kiekvienos transakcijos suma buvo parenkama atsitiktinai, tačiau ir vartotojų turimi pinigai taip pat buvo parenkami atsitiktinai, tad daugeliui sugeneruotų vartotojų paprasčiausiai neužtekdavo lėšų pervesti jas kitam vartotojui, taip būdavo atmetama transakcija)

Galutinė suvestinė:

<img width="357" height="418" alt="image" src="https://github.com/user-attachments/assets/23f2a3f9-f41d-4091-b143-dcbc5aa07b17" />

Galutinė pinigų suma nepasikeitė, nes nėra papildomai skaičiuojama komisinių ar kasimo atlygiai.

v0.2 užduotis:
Užduoties tikslas: papildyti pirmąją darbo dalį (v0.1) merkle medžiu ir validavimo testais. Šiai užduočiai buvo sukurti papildomi failai merkle.h, merkle.cpp ir main2.cpp.

Šioje užduotyje atlikti transakcijų validavimo testai:

<img width="742" height="840" alt="image" src="https://github.com/user-attachments/assets/c96f23ef-b845-4fa6-9bf7-a83788cfc864" />
<img width="427" height="222" alt="image" src="https://github.com/user-attachments/assets/1595d337-034e-4be4-b3f2-e4d995c309ff" />

Validavimo testai tikrina:
1) Ar galioja teisinga transakcija
2) ir 3) - ar transakcija su nežinomu siuntėju/gavėju bus atmesta
4) Ar kai transakcijos suma bus 0 ji bus atmesta
5) Ar transakcija su neteisingu transakcijos ID bus atmesta
6) Ar transakcija su nepakankamu siuntėjo balansu bus atmesta
7) ar transakcija su pasikartojančiu transakcijos ID bus atmesta
8) Ar transakcija sukelianti sveikojo skaičiaus perpildymą bus atmesta

<img width="752" height="220" alt="image" src="https://github.com/user-attachments/assets/cf8bec7c-a578-45e8-b7c7-12c6e3343493" />

Kodo vietos, kuriose yra transakcijų turinio ir tvarkos pakeitimo testai:

<img width="1180" height="205" alt="image" src="https://github.com/user-attachments/assets/4e07c73f-9daa-4059-bdb2-b5ea9c0c03e3" />
<img width="1060" height="172" alt="image" src="https://github.com/user-attachments/assets/e76f5c90-bf7b-47c4-b70e-8c4e2b9fd7f4" />

Šioje užduotyje atlikti Merkle medžio testai su 1/2/3/5 transakcijomis:

<img width="760" height="330" alt="image" src="https://github.com/user-attachments/assets/cbecef4e-109e-4cb9-ad62-e6914fc10aca" />

Pavyzdys: kelių blokų išvestys:

<img width="823" height="425" alt="image" src="https://github.com/user-attachments/assets/6942a22c-3f1f-4d26-a5be-4549f6d0ed9d" />

Galutinė suvestinė:

<img width="492" height="255" alt="image" src="https://github.com/user-attachments/assets/7cd208cb-3e8b-4798-8e80-d18b84b9b3d9" />

v0.3 užduotis: 

Šią užduotį (papildomą failą v0.3.cpp) pilnai sugeneravo DI įrankis "COPILOT", keliose vietose užduotį tobulino įrankis "ChatGPT".

Šioje užduotyje buvo tobulinami: Merkle medžio skaičiavimo efektyvumas; Validavimo klaidos

<img width="745" height="373" alt="image" src="https://github.com/user-attachments/assets/f20dbf17-3483-4844-a55c-594a83d13434" />
<img width="510" height="221" alt="image" src="https://github.com/user-attachments/assets/e6d4f636-5b05-4a5d-9902-f9f0c4a5944d" />

Papildoma užduotis: Lygiagretus kasimas.

Šiai užduočiai papildomai buvo sukurtas kodas lygiagretus_kasimas.cpp.

Užduoties tikslas: kandidatų kasimą (v0.2 užduotyje) pakeisti iš nuoseklaus į lygiagretų.

<img width="865" height="133" alt="image" src="https://github.com/user-attachments/assets/85c87f05-1bfb-4048-b61a-a03b86e95e8c" />

Kaip matome, beveik 4 kartus padidėjo užduoties efektyvumas.
