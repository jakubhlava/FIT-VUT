drop table Ockovani;
drop table PreventivniProhlidka;
drop table AkutniPripad;
drop table Vykon;
drop table ReceptNaLek;
drop table ZadankaNaVysetreni;
drop table PlanovanyVykon;


drop table Pozvanka;
drop table Navsteva;
drop table Faktura;
drop table VlastniPacient;
drop table Pacient;
drop table Pojistovna;

create table Pojistovna
(
    cisloPojistovny int PRIMARY KEY,
    nazevPojistovny varchar(128)
);

create table Pacient
(
    rodneCislo    varchar(10) PRIMARY KEY,
    jmeno         varchar(64),
    prijmeni      varchar(64),
    datumNarozeni date,
    telefon       varchar(13),
    email         varchar(64),
    ulice         varchar(64),
    mesto         varchar(64),
    psc           number(5),
    cisloPojistovny int references Pojistovna,

    CONSTRAINT formatrc
    CHECK (REGEXP_LIKE(rodneCislo, '^[0-9]{9,10}$')),
    CONSTRAINT delitelnost
    CHECK (NOT (LENGTH(rodneCislo) = 10 AND MOD(TO_NUMBER(rodneCislo), 11) != 0)),
    CONSTRAINT koncovka
    CHECK (NOT (LENGTH(rodneCislo) = 9 AND SUBSTR(rodneCislo, 7, 3) = '000')),
    CONSTRAINT ecprcpkolize
    CHECK (NOT (TO_NUMBER(SUBSTR(rodneCislo, 5, 2)) > 40 AND
                 ((TO_NUMBER(SUBSTR(rodneCislo, 3, 2)) BETWEEN 20 and 50) OR
                  TO_NUMBER(SUBSTR(rodneCislo, 3, 2)) > 70))),
    CONSTRAINT mesicrc
    CHECK ((TO_NUMBER(SUBSTR(rodneCislo, 3, 2)) BETWEEN 1 AND 12) OR
             (TO_NUMBER(SUBSTR(rodneCislo, 3, 2)) BETWEEN 21 AND 32) OR
             (TO_NUMBER(SUBSTR(rodneCislo, 3, 2)) BETWEEN 51 AND 62) OR
             (TO_NUMBER(SUBSTR(rodneCislo, 3, 2)) BETWEEN 71 AND 82)),
    CONSTRAINT denrc
    CHECK (TO_NUMBER(SUBSTR(rodneCislo, 5, 2)) BETWEEN 1 AND 31 OR
           TO_NUMBER(SUBSTR(rodneCislo, 5, 2)) BETWEEN 41 AND 71),
    CONSTRAINT ecpkoncovka
    CHECK (NOT (TO_NUMBER(SUBSTR(rodneCislo, 5, 2)) > 40 AND TO_NUMBER(SUBSTR(rodneCislo, 7, 1)) < 6))

);

create table VlastniPacient
(
    rodneCislo varchar(10) PRIMARY KEY references Pacient
);

create table AkutniPripad
(
    rodneCislo     varchar(10) PRIMARY KEY references Pacient,
    praktickyLekar varchar(128)
);

create table Navsteva
(
    termin         date PRIMARY KEY,
    duvod          varchar(128),
    lekarskaZprava clob,
    rodneCislo     varchar(10) references Pacient
);


create table Faktura
(
    cisloFaktury    int GENERATED AS IDENTITY PRIMARY KEY,
    cisloPojistovny int references Pojistovna
);

create table Vykon
(
    idVykonu     int GENERATED AS IDENTITY PRIMARY KEY,
    nazev        varchar(64),
    cena         float,
    cisloFaktury int references Faktura,
    termin       date references Navsteva
);

create table ReceptNaLek
(
    idReceptu    int GENERATED AS IDENTITY PRIMARY KEY,
    nazev        varchar(64),
    mnozstvi     int,
    celkovaCena  float,
    cisloFaktury int references Faktura,
    termin       date references Navsteva
);

create table ZadankaNaVysetreni
(
    id               int GENERATED AS IDENTITY PRIMARY KEY,
    lekar            varchar(128),
    nazevPracoviste  varchar(128),
    zpravaZVysetreni clob,
    termin           date references Navsteva
);

create table Pozvanka
(
    termin     date PRIMARY KEY references Navsteva,
    rodneCislo varchar(10) references VlastniPacient
);

create table PlanovanyVykon
(
    idVykonu int GENERATED AS IDENTITY PRIMARY KEY,
    popis    varchar(128),
    termin   date references Pozvanka
);

create table Ockovani
(
    idOckovani int GENERATED AS IDENTITY PRIMARY KEY,
    typ        varchar(64),
    expirace   date,
    rodneCislo references VlastniPacient
);

create table PreventivniProhlidka
(
    idProhlidky int GENERATED AS IDENTITY PRIMARY KEY,
    expirace    date,
    rodneCislo references VlastniPacient
);

INSERT INTO Pojistovna (cisloPojistovny, nazevPojistovny) VALUES (111, 'Všeobecná zdravotní pojišťovna');
INSERT INTO Pojistovna (cisloPojistovny, nazevPojistovny) VALUES (205, 'Česká průmyslová zdravotní pojišťovna');
INSERT INTO Pojistovna (cisloPojistovny, nazevPojistovny) VALUES (211, 'Zdravotní pojišťovna ministerstva vnitra České republiky');

INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('7512042692', 'Bohdan', 'Zdravý', TO_DATE('1975-12-04', 'YYYY-MM-DD'), '+420777888999',
        'bohdan.z@seznam.cz', 'Pod Kosířem 24', 'Prostějov', 79601, 111);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('5910113451', 'Igor', 'Nemocný', TO_DATE('1959-10-11', 'YYYY-MM-DD'), '+420722123789',
        'igor59@seznam.cz', 'Tovární 5', 'Prostějov', 79811, 205);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('6606068722', 'Radovan', 'Čert', TO_DATE('1966-06-06', 'YYYY-MM-DD'), '+420606666666',
        'pekelnik@centrum.cz', 'Dolní 75', 'Prostějov', 79601, 211);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('8101071572', 'Mirek', 'Neurčitý', TO_DATE('1981-01-07', 'YYYY-MM-DD'), '+420774456789',
        'neurcitym@volny.cz', 'Určice 72', 'Určice', 79804, 205);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('9454059846', 'Kristýna', 'Černá', TO_DATE('1994-04-05', 'YYYY-MM-DD'), '+420777789456',
        'cerna.k@seznam.cz', 'Olomoucká 56', 'Kralice na Hané', 79812, 111);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('9761077854', 'Veronika', 'Bílá', TO_DATE('1997-11-07', 'YYYY-MM-DD'), '+420721212121',
        'vbila@post.cz', 'Trávníky 464', 'Kralice na Hané', 79812, 111);

INSERT INTO VlastniPacient (rodneCislo) VALUES ('7512042692');
INSERT INTO VlastniPacient (rodneCislo) VALUES ('6606068722');
INSERT INTO VlastniPacient (rodneCislo) VALUES ('8101071572');
INSERT INTO VlastniPacient (rodneCislo) VALUES ('9454059846');
INSERT INTO VlastniPacient (rodneCislo) VALUES ('9761077854');

INSERT INTO AkutniPripad (rodneCislo, praktickyLekar) VALUES ('5910113451', 'MUDr. Aleš Léčitel');

INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-01 08:00', 'YYYY-MM-DD HH24:MI'), 'Očkování COVID', 'Podána vakcína AstraZeneca', '6606068722');
INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'), 'Preventivní prohlídka', 'Předepsáno vyšetření zubařem', '9761077854');
INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'), 'Běžná prohlídka', 'Bolesti kolenního kloubu, předepsány léky na tlumení bolesti a vyšetření ortopedem', '8101071572')
INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-02 09:00', 'YYYY-MM-DD HH24:MI'), 'Akutní případ', 'Silná bolest v krku, předepsány antibiotika', '5910113451');

INSERT INTO Pozvanka (termin, rodneCislo) VALUES (TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'), '9761077854');

INSERT INTO PlanovanyVykon (popis, termin) VALUES ('Preventivní prohlídka - 23 let', TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Ockovani (typ, expirace, rodneCislo) VALUES ('COVID-AstraZeneca-davka1', TO_DATE('2021-03-22', 'YYYY-MM-DD HH24:MI'),'6606068722');

INSERT INTO ZadankaNaVysetreni (lekar, nazevPracoviste, zpravaZVysetreni, termin) VALUES ('MUDr. Zdeněk Trhač', 'Zubní ordinace', NULL, TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ZadankaNaVysetreni (lekar, nazevPracoviste, zpravaZVysetreni, termin) VALUES ('MUDr. Petr Doktor', 'Ortopedie', NULL, TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Faktura (cisloPojistovny) VALUES (211);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Prohlídka před očkováním', 220.0, 1, TO_DATE('2021-03-01 08:00', 'YYYY-MM-DD HH24:MI'));
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Provedení očkování', 70.0, 1, TO_DATE('2021-03-01 08:00', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ReceptNaLek (nazev, mnozstvi, celkovaCena, cisloFaktury, termin) VALUES ('AstraZeneca Vakcína', 1, 200.0, 1, TO_DATE('2021-03-01 08:00', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Faktura (cisloPojistovny) VALUES (111);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Preventivni prohlidka', 500.0, 2, TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'));
INSERT INTO PreventivniProhlidka (expirace, rodneCislo) VALUES (TO_DATE('2023-03-02 08:30', 'YYYY-MM-DD HH24:MI'),'9761077854');

INSERT INTO Faktura (cisloPojistovny) VALUES (205);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Běžná prohlídka na žádost pacienta', 350.0, 3, TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ReceptNaLek (nazev, mnozstvi, celkovaCena, cisloFaktury, termin) VALUES ('Ibalgin', 1, 120.9, 3, TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Faktura (cisloPojistovny) VALUES (205);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Akutní prohlídka', 400.0, 4, TO_DATE('2021-03-02 09:00', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ReceptNaLek (nazev, mnozstvi, celkovaCena, cisloFaktury, termin) VALUES ('ProtiBakterikumVKrkum', 2, 350.7, 4, TO_DATE('2021-03-02 09:00', 'YYYY-MM-DD HH24:MI'));

-- Po specializovanem vysetreni

UPDATE ZadankaNaVysetreni SET zpravaZVysetreni = 'Nález zubního kazu, odvrtán, zaplombován.' WHERE termin = TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI');
UPDATE ZadankaNaVysetreni SET zpravaZVysetreni = 'Bolest do návštěvy částečně odezněla, pacientovi vyžádán rentgen kolene.' WHERE termin = TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI');


-- Všechny návštěvy lidí z Prostějova (spojení dvou tabulek 1/2)
SELECT DISTINCT P.jmeno, P.prijmeni, P.telefon, N.termin, N.duvod
FROM Pacient P, Navsteva N
WHERE N.rodneCislo = P.rodneCislo AND P.mesto = 'Prostějov';

-- Všechna expirovaná očkování (spojení dvou tabulek 2/2)
SELECT DISTINCT P.jmeno, P.prijmeni, P.telefon, O.typ
FROM Pacient P, Ockovani O
WHERE P.rodneCislo=O.rodneCislo AND O.expirace < SYSDATE;

-- Počty pacientů z jednotlivých obcí (GROUP BY 1/2)
SELECT COUNT(rodneCislo) pocet_pacientu, mesto
FROM Pacient
GROUP BY mesto
ORDER BY COUNT(rodneCislo) ASC;

-- Ceny výkonů jednotlivých faktur (GROUP BY 2/2)
SELECT F.cisloFaktury, SUM(V.cena) cena_vykonu FROM
Faktura F, Vykon V
WHERE F.cisloFaktury = V.cisloFaktury
GROUP BY F.cisloFaktury
ORDER BY F.cisloFaktury ASC;

-- Celkové ceny jednotlivých faktur (spojení 3 tabulek)
SELECT sv.cisloFaktury, COALESCE(v_ceny, 0)+COALESCE(rnl_ceny, 0) celkovaCena FROM
(SELECT SUM(V.cena) v_ceny, F.cisloFaktury
    FROM Faktura F, Vykon V
    WHERE V.cisloFaktury = F.cisloFaktury
    GROUP BY F.cisloFaktury) sv
LEFT JOIN
(SELECT SUM(RNL.celkovaCena) rnl_ceny, F.cisloFaktury
    FROM Faktura F, ReceptNaLek RNL
    WHERE F.cisloFaktury = RNL.cisloFaktury
    GROUP BY F.cisloFaktury) srnl ON sv.cisloFaktury = srnl.cisloFaktury
ORDER BY sv.cisloFaktury;

-- Pacienti s alespoň jednou návštěvou (EXISTS)
SELECT DISTINCT P.jmeno, P.prijmeni, P.rodneCislo
FROM Pacient P
WHERE EXISTS
    (SELECT N.termin FROM Navsteva N WHERE N.rodneCislo=P.rodneCislo);

-- Kontakt na všechny akutní případy (IN)
SELECT DISTINCT P.jmeno, P.prijmeni, P.telefon, P.email, P.rodneCislo
FROM Pacient P
WHERE p.rodneCislo IN
      (SELECT rodneCislo FROM AkutniPripad);