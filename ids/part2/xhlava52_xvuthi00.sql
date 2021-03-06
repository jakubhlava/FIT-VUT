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

INSERT INTO Pojistovna (cisloPojistovny, nazevPojistovny) VALUES (111, 'V??eobecn?? zdravotn?? poji????ovna');
INSERT INTO Pojistovna (cisloPojistovny, nazevPojistovny) VALUES (205, '??esk?? pr??myslov?? zdravotn?? poji????ovna');
INSERT INTO Pojistovna (cisloPojistovny, nazevPojistovny) VALUES (211, 'Zdravotn?? poji????ovna ministerstva vnitra ??esk?? republiky');

INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('7512042692', 'Bohdan', 'Zdrav??', TO_DATE('1975-12-04', 'YYYY-MM-DD'), '+420777888999',
        'bohdan.z@seznam.cz', 'Pod Kos????em 24', 'Prost??jov', 79601, 111);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('5910113451', 'Igor', 'Nemocn??', TO_DATE('1959-10-11', 'YYYY-MM-DD'), '+420722123789',
        'igor59@seznam.cz', 'Tov??rn?? 5', 'Prost??jov', 79811, 205);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('6606068722', 'Radovan', '??ert', TO_DATE('1966-06-06', 'YYYY-MM-DD'), '+420606666666',
        'pekelnik@centrum.cz', 'Doln?? 75', 'Prost??jov', 79601, 211);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('8101071572', 'Mirek', 'Neur??it??', TO_DATE('1981-01-07', 'YYYY-MM-DD'), '+420774456789',
        'neurcitym@volny.cz', 'Ur??ice 72', 'Ur??ice', 79804, 205);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('9454059846', 'Krist??na', '??ern??', TO_DATE('1994-04-05', 'YYYY-MM-DD'), '+420777789456',
        'cerna.k@seznam.cz', 'Olomouck?? 56', 'Kralice na Han??', 79812, 111);
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('9761077854', 'Veronika', 'B??l??', TO_DATE('1997-11-07', 'YYYY-MM-DD'), '+420721212121',
        'vbila@post.cz', 'Tr??vn??ky 464', 'Kralice na Han??', 79812, 111);

INSERT INTO VlastniPacient (rodneCislo) VALUES ('7512042692');
INSERT INTO VlastniPacient (rodneCislo) VALUES ('6606068722');
INSERT INTO VlastniPacient (rodneCislo) VALUES ('8101071572');
INSERT INTO VlastniPacient (rodneCislo) VALUES ('9454059846');
INSERT INTO VlastniPacient (rodneCislo) VALUES ('9761077854');

INSERT INTO AkutniPripad (rodneCislo, praktickyLekar) VALUES ('5910113451', 'MUDr. Ale?? L????itel');

INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-01 08:00', 'YYYY-MM-DD HH24:MI'), 'O??kov??n?? COVID', 'Pod??na vakc??na AstraZeneca', '6606068722');
INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'), 'Preventivn?? prohl??dka', 'P??edeps??no vy??et??en?? zuba??em', '9761077854');
INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'), 'B????n?? prohl??dka', 'Bolesti kolenn??ho kloubu, p??edeps??ny l??ky na tlumen?? bolesti a vy??et??en?? ortopedem', '8101071572')
INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-02 09:00', 'YYYY-MM-DD HH24:MI'), 'Akutn?? p????pad', 'Siln?? bolest v krku, p??edeps??ny antibiotika', '5910113451');

INSERT INTO Pozvanka (termin, rodneCislo) VALUES (TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'), '9761077854');

INSERT INTO PlanovanyVykon (popis, termin) VALUES ('Preventivn?? prohl??dka - 23 let', TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Ockovani (typ, expirace, rodneCislo) VALUES ('COVID-AstraZeneca-davka1', TO_DATE('2021-03-22', 'YYYY-MM-DD HH24:MI'),'6606068722');

INSERT INTO ZadankaNaVysetreni (lekar, nazevPracoviste, zpravaZVysetreni, termin) VALUES ('MUDr. Zden??k Trha??', 'Zubn?? ordinace', NULL, TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ZadankaNaVysetreni (lekar, nazevPracoviste, zpravaZVysetreni, termin) VALUES ('MUDr. Petr Doktor', 'Ortopedie', NULL, TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Faktura (cisloPojistovny) VALUES (211);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Proveden?? o??kov??n??', 70.0, 1, TO_DATE('2021-03-01 08:00', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ReceptNaLek (nazev, mnozstvi, celkovaCena, cisloFaktury, termin) VALUES ('AstraZeneca Vakc??na', 1, 200.0, 1, TO_DATE('2021-03-01 08:00', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Faktura (cisloPojistovny) VALUES (111);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Preventivni prohlidka', 500.0, 2, TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'));
INSERT INTO PreventivniProhlidka (expirace, rodneCislo) VALUES (TO_DATE('2023-03-02 08:30', 'YYYY-MM-DD HH24:MI'),'9761077854');

INSERT INTO Faktura (cisloPojistovny) VALUES (205);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('B????n?? prohl??dka na ????dost pacienta', 350.0, 3, TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ReceptNaLek (nazev, mnozstvi, celkovaCena, cisloFaktury, termin) VALUES ('Ibalgin', 1, 120.9, 3, TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Faktura (cisloPojistovny) VALUES (205);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Akutn?? prohl??dka', 400.0, 4, TO_DATE('2021-03-02 09:00', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ReceptNaLek (nazev, mnozstvi, celkovaCena, cisloFaktury, termin) VALUES ('ProtiBakterikumVKrkum', 2, 350.7, 4, TO_DATE('2021-03-02 09:00', 'YYYY-MM-DD HH24:MI'));

-- Po specializovanem vysetreni

UPDATE ZadankaNaVysetreni SET zpravaZVysetreni = 'N??lez zubn??ho kazu, odvrt??n, zaplombov??n.' WHERE termin = TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI');
UPDATE ZadankaNaVysetreni SET zpravaZVysetreni = 'Bolest do n??v??t??vy ????ste??n?? odezn??la, pacientovi vy????d??n rentgen kolene.' WHERE termin = TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI');