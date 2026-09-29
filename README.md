# Bloku_grandiniu_technologijos

Programos failas: v01.exe

Programos paleidimo instrukcija:

Programos veikimo principas: 

Programos pseudokodas: 

Programos tobulinimui naudoti DI įrankiai: ChatGPT; Google GEMINI (nemokamos versijos).


1 eksperimentas (įvestys): 

Pateikiu excel lentelę su tikrintais failais (kas juos sudarė ir kokie gavosi hash). "Random" visus failus pakeičiau taip: pirmame pakeičiau pirmąjį simbolį, antrame pakeičiau paskutinįjį simbolį, trečiame pridėjau tarpą viduryje teksto.

Pirmi du "random" >1000 baitų failai turi tik ascii simbolius, trečiasis turi ir kitokių (kurie užima daugiau nei 1 baitą atminties).
P.S. visus tris failus atsitiktinai generavo DI įrankis ChatGPT.

<img width="788" height="355" alt="image" src="https://github.com/user-attachments/assets/86f9b226-e460-405b-a7e3-2b131d2488f2" />
<img width="670" height="283" alt="image" src="https://github.com/user-attachments/assets/66ffe6d3-50a7-4223-aff9-48a7d12d509e" />
<img width="780" height="347" alt="image" src="https://github.com/user-attachments/assets/d7e49e53-5a8d-4893-8d0d-9f85bf2ab054" />
<img width="657" height="310" alt="image" src="https://github.com/user-attachments/assets/5b7635ab-f670-4826-aa22-97b89138f03c" />
<img width="677" height="302" alt="image" src="https://github.com/user-attachments/assets/1f36d32f-5b8e-4da4-b02d-e2589622f9cd" />
<img width="673" height="272" alt="image" src="https://github.com/user-attachments/assets/92db6403-8ff6-4c87-80f9-f39e983eeb46" />
<img width="662" height="250" alt="image" src="https://github.com/user-attachments/assets/310489cb-7524-4ad9-a252-3dba94781970" />
<img width="688" height="235" alt="image" src="https://github.com/user-attachments/assets/8906615d-94bc-4f03-859d-b9ad7e7b3306" />
<img width="637" height="202" alt="image" src="https://github.com/user-attachments/assets/49cc1f7a-01b2-43e1-a95b-363c981debad" />
<img width="1402" height="282" alt="image" src="https://github.com/user-attachments/assets/361c5e63-6c12-4523-a1d2-00aee304a05d" />

Pastebėtina, jog pasikeitus vos vienam simboliui arba atsirandant papildomam tarpui visiškai pasikeičia gautas hash.



2 eksperimentas (išvestis):

Iš paveikslėlių pirmajame teste matosi, kad kiekvienas gautas hash turi lygiai 64 hex simbolius.

Programa taip pat duoda vienodą rezultatą (hash'ą), jei taip pat parašytas vienodų simbolių rinkinys yra rašomas ranka arba skaitomas iš failo.

Įrodymas: (tarkime, tekstas yra abc).

<img width="326" height="106" alt="image" src="https://github.com/user-attachments/assets/bb57e658-65b1-491e-8f87-a53408321fc1" />
<img width="786" height="487" alt="image" src="https://github.com/user-attachments/assets/26975daa-be32-4e56-b851-d6b8118dabd8" />

Matome, kad išvestis gavosi lygiai tokia pati.



3 eksperimentas (determinizmas):

<img width="712" height="362" alt="image" src="https://github.com/user-attachments/assets/503602ef-1f70-4d9c-8f0c-24c77f29f62c" />

Matome, kad du kartus įvedus tą patį žodį, išvestis gaunasi tokia pati. o čia įrodymas, kad paleidus programą iš naujo ir įvedus tą patį žodį vistiek gaunama ta pati išvestis:

<img width="788" height="338" alt="image" src="https://github.com/user-attachments/assets/f23dfe07-073e-4928-a334-bbacb4b249d8" />

Programa taip pat išveda tą patį hash'ą, jei įvestyje tą patį tekstą įvedame ne iš eilės:

<img width="665" height="420" alt="image" src="https://github.com/user-attachments/assets/4efd8ab8-2b61-428b-b030-cb058bd29a7e" />

Kaip matome, naujaszodis įvesties gautas hash yra visiškai tas pats, nors tarp įvedimų įrašėme ir visiškai kitą reikšmę.

4 eksperimentas (efektyvumas):

4 eksperimentas (kodas, patobulintas DI) (efektyvumas):

5 eksperimentas (kolizijos):

5 eksperimentas (kodas, patobulintas DI) (kolizijos):

6 eksperimentas (lavinos efektas):

6 eksperimentas (kodas, patobulintas DI) (lavinos efektas):

7 eksperimentas (spėjimas, vieša druska ir slaptas atsitiktinumas):

7 eksperimentas (kodas, patobulintas DI) (spėjimas, vieša druska ir slaptas atsitiktinumas):

8 eksperimentas (išvados):

8 eksperimentas (išvados):

Papildoma užduotis (kodo lyginimas su MD5, SHA-1, SHA-256:
