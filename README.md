# Bloku_grandiniu_technologijos

Programos failas: v01.exe

Programos paleidimo instrukcija:

Programos veikimo principas: 

Programos pseudokodas: 

Programos tobulinimui naudoti DI įrankiai: ChatGPT; Google GEMINI (nemokamos versijos).

P.S. eksperimentai 1-3 buvo atlikti be DI pagalbos rašytame kode, o eksperimentams 4-7 buvo naudojama DI pagalba kodo rašymui.

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

P.S. - 4-7 eksperimentams įgyvendinti buvo naudotos anksčiau įvardytos DI priemonės (įrankiai naudoti tobulinti kodui, kad galima būtų atlikti šiuos eksperimentus).

4 eksperimentas (kodas rašytas su DI pagalba) (efektyvumas):

<img width="477" height="786" alt="image" src="https://github.com/user-attachments/assets/f7cc6fd7-b546-4dcb-8f4e-1b067c7e95ed" />
<img width="261" height="745" alt="image" src="https://github.com/user-attachments/assets/5aca20d5-1bf8-4b1f-97ea-5db87f64dcf7" />
<img width="283" height="285" alt="image" src="https://github.com/user-attachments/assets/916914e1-46f7-4624-b4df-827c87bdcbe3" />

Grafikas (pastaba - grafiką sukūrė Google Gemini dirbtinio intelekto įrankis):
<img width="1053" height="621" alt="image" src="https://github.com/user-attachments/assets/cf6dcd10-a0da-47ef-b401-66972819bf6b" />

Kaip matome iš gautos išvesties, kuo daugiau baitų apdorojo ("hash'avo") programa, tuo ilgiau ji užtruko, taip pat, matome, kad kreivė pastoviai kyla į viršų, tad keistų anomalijų ar nesutapimų šiame programos eksperimente nebuvo rasta.

5 eksperimentas (kodas rašytas su DI pagalba) (kolizijos):

<img width="852" height="727" alt="image" src="https://github.com/user-attachments/assets/f4f3b128-ff5b-406b-87e4-7a14c52f106e" />

Kaip matome, nebuvo rasta nei vienos kolizijos, nes to šansas yra itin mažas: Kai yra "hash'uojama" skirtinga įvestis šansas to, kad kažkuri viena pora turės tokį patį "hash'ą", pagal formulę, yra 2^-256, o skirtingų porų, kai eilučių yra 200000 (100000 porų = 100000 * 2 eilučių), pagal formulę, yra 200000 * (200000-1)/2 = 19 999 900 000, o tai yra labai mažas skaičius lyginant su 2^256.

6 eksperimentas (lavinos efektas):

7 eksperimentas (spėjimas, vieša druska ir slaptas atsitiktinumas):

8 eksperimentas (išvados):

Šis eksperimentas yra daugiausia aprašytas readme failo pradžioje, tai yra, ten įkėliau programos veikimo principą, pseudokodą ir paleidimo instrukciją.

Papildoma užduotis (kodo lyginimas su MD5, SHA-1, SHA-256:
