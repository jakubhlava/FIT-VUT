drop trigger overitUdaje;
drop trigger pridatDite;
drop trigger overitTelefonDite;
drop sequence pocitadlo;
drop table Deti;

drop procedure kontakty;
drop procedure nastavRodice;

drop table Ockovani;
drop table PreventivniProhlidka;
drop table AkutniPripad;
drop table ZadankaNaVysetreni;
drop table PlanovanyVykon;


drop table Pozvanka;
drop table ReceptNaLek;
drop table Vykon;
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

create table Deti
(
    idDitete                int PRIMARY KEY,
    rodneCislo              varchar(10) references Pacient,
    jmenoZastupce           varchar(64),
    prijmeniZastupce        varchar(64),
    telefonZastupce         varchar(13)
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
INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'), 'B????n?? prohl??dka', 'Bolesti kolenn??ho kloubu, p??edeps??ny l??ky na tlumen?? bolesti a vy??et??en?? ortopedem', '8101071572');
INSERT INTO Navsteva (termin, duvod, lekarskaZprava, rodneCislo) VALUES (TO_DATE('2021-03-02 09:00', 'YYYY-MM-DD HH24:MI'), 'Akutn?? p????pad', 'Siln?? bolest v krku, p??edeps??ny antibiotika', '5910113451');

INSERT INTO Pozvanka (termin, rodneCislo) VALUES (TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'), '9761077854');

INSERT INTO PlanovanyVykon (popis, termin) VALUES ('Preventivn?? prohl??dka - 23 let', TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Ockovani (typ, expirace, rodneCislo) VALUES ('COVID-AstraZeneca-davka1', TO_DATE('2021-03-22', 'YYYY-MM-DD HH24:MI'),'6606068722');

INSERT INTO ZadankaNaVysetreni (lekar, nazevPracoviste, zpravaZVysetreni, termin) VALUES ('MUDr. Zden??k Trha??', 'Zubn?? ordinace', NULL, TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI'));
INSERT INTO ZadankaNaVysetreni (lekar, nazevPracoviste, zpravaZVysetreni, termin) VALUES ('MUDr. Petr Doktor', 'Ortopedie', NULL, TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI'));

INSERT INTO Faktura (cisloPojistovny) VALUES (211);
INSERT INTO Vykon (nazev, cena, cisloFaktury, termin) VALUES ('Prohl??dka p??ed o??kov??n??m', 220.0, 1, TO_DATE('2021-03-01 08:00', 'YYYY-MM-DD HH24:MI'));
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
COMMIT;
-- Po specializovanem vysetreni

UPDATE ZadankaNaVysetreni SET zpravaZVysetreni = 'N??lez zubn??ho kazu, odvrt??n, zaplombov??n.' WHERE termin = TO_DATE('2021-03-02 08:30', 'YYYY-MM-DD HH24:MI');
UPDATE ZadankaNaVysetreni SET zpravaZVysetreni = 'Bolest do n??v??t??vy ????ste??n?? odezn??la, pacientovi vy????d??n rentgen kolene.' WHERE termin = TO_DATE('2021-03-02 08:45', 'YYYY-MM-DD HH24:MI');
COMMIT;

-- V??echny n??v??t??vy lid?? z Prost??jova (spojen?? dvou tabulek 1/2)
SELECT DISTINCT P.jmeno, P.prijmeni, P.telefon, N.termin, N.duvod
FROM Pacient P, Navsteva N
WHERE N.rodneCislo = P.rodneCislo AND P.mesto = 'Prost??jov';

-- V??echna expirovan?? o??kov??n?? (spojen?? dvou tabulek 2/2)
SELECT DISTINCT P.jmeno, P.prijmeni, P.telefon, O.typ
FROM Pacient P, Ockovani O
WHERE P.rodneCislo=O.rodneCislo AND O.expirace < SYSDATE;

-- Po??ty pacient?? z jednotliv??ch obc?? (GROUP BY 1/2)
SELECT COUNT(rodneCislo) pocet_pacientu, mesto
FROM Pacient
GROUP BY mesto
ORDER BY COUNT(rodneCislo) ASC;

-- Ceny v??kon?? jednotliv??ch faktur (GROUP BY 2/2)
SELECT F.cisloFaktury, SUM(V.cena) cena_vykonu FROM
Faktura F, Vykon V
WHERE F.cisloFaktury = V.cisloFaktury
GROUP BY F.cisloFaktury
ORDER BY F.cisloFaktury ASC;

-- Celkov?? ceny jednotliv??ch faktur (spojen?? 3 tabulek)
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

-- Pacienti s alespo?? jednou n??v??t??vou (EXISTS)
SELECT DISTINCT P.jmeno, P.prijmeni, P.rodneCislo
FROM Pacient P
WHERE EXISTS
    (SELECT N.termin FROM Navsteva N WHERE N.rodneCislo=P.rodneCislo);

-- Kontakt na v??echny akutn?? p????pady (IN)
SELECT DISTINCT P.jmeno, P.prijmeni, P.telefon, P.email, P.rodneCislo
FROM Pacient P
WHERE p.rodneCislo IN
      (SELECT rodneCislo FROM AkutniPripad);


INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('1202123065', 'Martin', 'Mlad??', TO_DATE('2012-02-12', 'YYYY-MM-DD'), '+420777345009',
        'milanmlady@seznam.cz', 'Vlhk?? 1', 'Mok??any', 62023, 111);

-- Sekvence, ze kter?? se budou generovat prim??rn?? kl????e
-- nejmen???? vygenerovan?? hodnota je 1000 a postupn?? roste o 1 nahoru

CREATE SEQUENCE pocitadlo
  start with 1000
  increment by 1;

-- Trigger, kter?? po p??id??n?? nov??ho pacienta zkontroluje v??k, a jestli je dan?? pacient mlad???? ne?? 18 let, tak ho p??id?? do tabulky Deti (1/2)
-- Vypo????t?? se v??k pacienta podle dne??n??ho data a pokud je v??sledek men???? ne?? 18, p??id??me pacienta do tabulky Deti
-- Jako prim??rn?? kl???? v tabulce Deti se pou??ije hodnota vygenerovan?? ze sekvence

CREATE OR REPLACE TRIGGER pridatDite
  AFTER INSERT ON Pacient
  FOR EACH ROW
BEGIN
    IF (TRUNC((SYSDATE - :NEW.datumNarozeni) / 365)) < 18 THEN
            INSERT INTO Deti(IDDITETE, rodneCislo, JMENOZASTUPCE, PRIJMENIZASTUPCE, telefonZastupce) VALUES (pocitadlo.nextval, :NEW.rodneCislo, NULL, NULL, :NEW.telefon);
    END IF;
END;

-- Ov????en?? form??tu telefonu a data narozen?? jako p????klad vyu??it?? triggeru pro zaj????t??n?? integrity dat
CREATE OR REPLACE TRIGGER overitUdaje
    BEFORE INSERT ON Pacient
    FOR EACH ROW
DECLARE
    chybny_telefon EXCEPTION;
    PRAGMA exception_init ( chybny_telefon, -20000 );
    chybne_datum_narozeni EXCEPTION;
    PRAGMA exception_init ( chybne_datum_narozeni, -20001 );
BEGIN
    IF NOT REGEXP_LIKE(:NEW.telefon, '^\+[0-9]{1,3}[0-9]{9}$') THEN
        raise_application_error(-20000, 'Neplatn?? telefonn?? ????slo (form??t +(predvolba)xxxxxxxxx)');
    end if;
    IF (SYSDATE < :NEW.datumNarozeni) THEN
        raise_application_error(-20001, 'Datum narozen?? mus?? b??t v minulosti!');
    end if;
END;

-- Pomocn?? trigger pro demonstraci exception handling v procedu??e n????e
CREATE OR REPLACE TRIGGER overitTelefonDite
    BEFORE INSERT OR UPDATE ON Deti
    FOR EACH ROW
declare
    chybny_telefon EXCEPTION;
    PRAGMA exception_init ( chybny_telefon, -20000 );
begin
    IF NOT REGEXP_LIKE(:NEW.telefonZastupce, '^\+[0-9]{1,3}[0-9]{9}$') THEN
        raise_application_error(-20000, 'Neplatn?? telefonn?? ????slo (form??t +(predvolba)xxxxxxxxx)');
    end if;
end;

-- P????kady triggeru pridatDite
INSERT INTO Deti (idDitete, rodneCislo, jmenoZastupce, prijmeniZastupce, telefonZastupce) VALUES ( '1','1202123065', 'Milan', 'MLad??', null/*'+420776543445'*/);

INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('1858080422', 'Jana', 'Mal??', TO_DATE('2018-08-08', 'YYYY-MM-DD'), '+420777666333',
        'jmala@seznam.cz', 'Olnova 12', 'Holiny', 78000, 111);

INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('9205011464', 'Martin', 'Mal??', TO_DATE('1998-08-18', 'YYYY-MM-DD'), '+420776953113',
        'martam@seznam.cz', 'HLavn?? 12', 'Hustope??e', 78001, 111);
SELECT DISTINCT P.rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny
FROM Pacient P;

SELECT DISTINCT D.idDitete, rodneCislo, jmenoZastupce, prijmeniZastupce, telefonZastupce
FROM Deti D;
COMMIT;

-- P????klady pro trigger overitUdaje
-- Datum narozen?? v budoucnosti
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('0458156039', 'Jana', 'Mal??', TO_DATE('2022-08-08', 'YYYY-MM-DD'), '+420777666333',
        'jmala@seznam.cz', 'Olnova 12', 'Holiny', 78000, 111);

-- telefonn?? ????slo bez p??edvolby
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('1152186838', 'Jana', 'Mal??', TO_DATE('2018-08-08', 'YYYY-MM-DD'), '777666333',
        'jmala@seznam.cz', 'Olnova 12', 'Holiny', 78000, 111);

-- nesmysl m??sto telefonn??ho ????sla
INSERT INTO Pacient (rodneCislo, jmeno, prijmeni, datumNarozeni, telefon, email, ulice, mesto, psc, cisloPojistovny) VALUES ('0551075338', 'Jana', 'Mal??', TO_DATE('2018-08-08', 'YYYY-MM-DD'), '+abcdefghijkl',
        'jmala@seznam.cz', 'Olnova 12', 'Holiny', 78000, 111);


-- Procedura pro v??pis osob narozen??ch v ur??it??m roce, nap??. pro ????ely objedn??n?? na preventivn?? prohl??dku
CREATE OR REPLACE PROCEDURE kontakty(rok in INT)
    IS
    CURSOR cur_pacienti IS
        SELECT jmeno, prijmeni, datumNarozeni, telefon, email, rodneCislo
        FROM Pacient
        WHERE datumNarozeni BETWEEN TO_DATE(TO_CHAR(rok), 'YYYY') AND TO_DATE(TO_CHAR(rok+1), 'YYYY');

    jmeno Pacient.jmeno%type;
    prijmeni Pacient.prijmeni%type;
    telefon Pacient.telefon%type;
    email Pacient.email%type;
    datumNarozeni Pacient.datumNarozeni%type;
    rodneCislo Pacient.rodneCislo%type;
BEGIN
    OPEN cur_pacienti;

    LOOP
        FETCH cur_pacienti INTO jmeno, prijmeni, datumNarozeni, telefon, email, rodneCislo;
        EXIT WHEN cur_pacienti%NOTFOUND;

        dbms_output.PUT_LINE('Pacient: ' || jmeno || ' ' || prijmeni || ' (' || rodneCislo || ') nar. ' || datumNarozeni || ': ' || telefon || ', ' || email);
    end loop;
        dbms_output.PUT_LINE('Celkem pacient?? narozen??ch v roce '|| rok || ': ' || cur_pacienti%ROWCOUNT);

    CLOSE cur_pacienti;
    return;
end;

-- Procedura pro p??i??azen?? ??daj?? o z??konn??m z??stupci k d??t??ti
CREATE OR REPLACE PROCEDURE
nastavRodice(rcDite in varchar, jmenorodice in varchar, prijmenirodice in varchar, telefonrodice in varchar) IS
    chybny_telefon EXCEPTION;
    PRAGMA exception_init ( chybny_telefon, -20000 );
    BEGIN
        UPDATE Deti SET jmenoZastupce=jmenorodice, prijmeniZastupce=prijmenirodice, telefonZastupce=telefonrodice WHERE rodneCislo=rcDite;
    EXCEPTION
        WHEN chybny_telefon THEN
            dbms_output.PUT_LINE('Zadan?? telefon m?? nespr??vn?? form??t, zkuste to pros??m znovu');
    end;

-- uk??zka kontakty
begin
kontakty(2018);
end;

-- uk??zka nastavRodice
begin
    nastavRodice(1858080422, 'Petr', 'Mal??', '+420777998541');
end;

-- handle vyj??mky
begin
    nastavRodice(1858080422, 'Petr', 'Mal??', '777998541');
end;

DROP MATERIALIZED VIEW ceny_pacientu;

CREATE MATERIALIZED VIEW ceny_pacientu
    NOLOGGING
    CACHE
    BUILD IMMEDIATE
    AS SELECT P.jmeno, P.prijmeni, P.rodneCislo, cv.celkemv vykonyCelkem, cl.celkeml lekyCelkem, COALESCE(cv.celkemv,0) + COALESCE(cl.celkeml,0) celkem
        FROM
         XHLAVA52.PACIENT P JOIN (SELECT P.rodneCislo rc, SUM(V.cena) celkemv FROM XHLAVA52.PACIENT P, XHLAVA52.VYKON V WHERE termin IN (SELECT termin from XHLAVA52.NAVSTEVA WHERE rodneCislo=P.rodneCislo) GROUP BY P.rodneCislo) cv on P.rodneCislo=cv.rc LEFT JOIN
         (SELECT P.rodneCislo rc, SUM(RNL.celkovaCena) celkeml FROM XHLAVA52.PACIENT P, XHLAVA52.RECEPTNALEK RNL WHERE termin IN (SELECT termin from XHLAVA52.NAVSTEVA WHERE rodneCislo=P.rodneCislo) GROUP BY P.rodneCislo) cl ON P.rodneCislo=cl.rc;

SELECT * FROM ceny_pacientu;

EXPLAIN PLAN FOR
    SELECT DISTINCT P.jmeno, P.prijmeni, P.rodneCislo, COUNT(N.termin)
FROM Pacient P INNER JOIN Navsteva N
    on P.rodneCislo = N.rodneCislo
GROUP BY P.jmeno, P.prijmeni, P.rodneCislo;
--SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());

CREATE INDEX rc ON Navsteva (rodneCislo);
CREATE INDEX jm ON Pacient (rodneCislo, jmeno, prijmeni);

EXPLAIN PLAN FOR
    SELECT DISTINCT P.jmeno, P.prijmeni, P.rodneCislo, COUNT(N.termin)
FROM Pacient P INNER JOIN Navsteva N
    on P.rodneCislo = N.rodneCislo
GROUP BY P.jmeno, P.prijmeni, P.rodneCislo;
--SELECT PLAN_TABLE_OUTPUT FROM TABLE(DBMS_XPLAN.DISPLAY());
