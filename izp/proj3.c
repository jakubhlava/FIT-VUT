/*
    IZP - Projekt 3
    Hledani cesty bludistem
    
    Jakub Hlava
    login: xhlava52
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Strany trojuhelnikovych bunek
#define B_UPDOWN 0
#define B_LEFT 1
#define B_RIGHT 2

// Pravidlo prave a leve ruky
#define RULERIGHT 0
#define RULELEFT 1

// Orientace trojuhelnikovych bunek
#define UP true
#define DOWN false

// Struktura pro ulozeni mapy
typedef struct
{
    int rows;
    int cols;
    unsigned char *cells;
} Map;

/**
 * Tiskne napovedu k programu
 */
void printHelp();

/**
 * Zjistuje, zda retezec na druhe pozici v argumentech programu
 * odpovida hledanemu argumentu a zda ma spravny pocet parametru.
 * Vypisuje chybove hlaseni v pripade nespravneho poctu parametru.
 * 
 * @param   **argv      parametry prebrane z funkce main()
 * @param   argc        pocet parametru
 * @param   *argName    hledane jmeno argumentu
 * @param   argLen      pozadovany pocet parametru funkce
 * @param   noerr       vypnuti vypisu pripadneho chyboveho hlaseni
 * 
 * @return  - 0     pro neshodu
 *          - 1     pro uplnou shodu
 *          - -1    pro shodu s nepravnym poctem parametru 
 */
int checkArg(char **argv, int argc, char *argName, int argLen, bool noerr);

/**
 * Testuje, zda struktura souboru odpovida platne strukture bludiste
 * dle zadani.
 * 
 * @param   *fileName   nazev souboru s definici bludiste
 * @return  true/false  pro platny/neplatny soubor
 */
bool testFileStructure(char *fileName);

/**
 * Testuje, zda je mapa v souboru validni podle definice v zadani,
 * tj. navazuji na sebe zdi bunek, neobsahuje neplatne znaky, prip. 
 * cisla neodpovidajici defincii policka
 * 
 * @param   *fileName   nazev souboru s definici bludiste
 * @return  true/false  pro platny/neplatny soubor
 */
bool validateMap(char *fileName);

/**
 * Hleda cestu bludistem podle daneho pravidla prave nebo leve ruky.
 * Cestu vypisuje ve formatu x,y
 * 
 * @param   rule        RULERIGHT/RULELEFT pro pravidlo prave/leve ruky
 * @param   row         radek pocatecniho policka, musi byt na okraji a
 *                      mit volny vstup d obludiste
 * @param   col         sloupec pocatecniho policka
 * @param   *fileName   nazev souboru s definici bludiste
 */
void findPath(int rule, int row, int col, char *fileName);

/**
 * Konstruktor datove struktury Map, nacte velikost mapy ze souboru a
 * podle nactenych hodnot alokuje pamet a inicializuje prazdnou strukturu.
 * V pripade, ze se nepodari mapu inicializovat, zustava prazdna.
 * 
 * @param   *map        ukazatel na prazdnou datovou strukturu typu Map
 * @param   *fileName   nazev souboru s definici bludiste
 */
void constructMap(Map *map, char *fileName);

/**
 * Nacita ze souboru bludiste do inicializovane struktury mapy. V pripade, ze
 * se nacitani nezdari, mapa zustane prazdna.
 * 
 * @param   *map        ukazatel na inicializovanou datovou strukturu typu Map
 * @param   *fileName   nazev souboru s definici bludiste
 */
void loadMap(Map *map, char *fileName);

/**
 * Destruktor datove struktury Map, uvolni alokovanou pamet, parametry rows a cols
 * nastavi na nulu, ukazatel na pole bunek nastavi na NULL
 * 
 * @param   *map        ukazatel na datovou strukturu typu Map
 */
void destructMap(Map *map);

/**
 * Zjistuje, zda je hranice mapy bludiste na danych souradnicich pevna ci pruchozi.
 * 
 * @param   *map        ukazatel na nactenou mapu v datove strukture typu Map
 * @param   row         souradnice radku bunky
 * @param   col         souradnice sloupce bunky
 * @param   border      hranice bunky (B_LEFT,B_RIGHT,B_UPDOWN) pro levou/pravou/vodorovnou
 * 
 * @return  true/false  pokud je hranice pevna/pruchozi
 */
bool isborder(Map *map, int row, int col, int border);

/**
 * Prevadi 2D souradnice na souradnici v poli bunek
 * 
 * @param   *map        ukazatel na mapu, pro kterou bude prevod proveden
 * @param   row         souradnice radku bunky
 * @param   col         souradnice sloupce bunky
 * 
 * @return  index bunky v poli bunek dane mapy nebo -1, pokud bunka nelezi v mape
 */
int getCellPos(Map *map, int row, int col);

/**
 * Kontroluje, zda souradnice bunky lezi uvnitr mapy nebo za jejimi hranicemi
 * 
 * @param   *map        ukazatel na mapu, ve ktere budeme uverovat souradnice
 * @param   row         souradnice radku bunky
 * @param   col         souradnice sloupce bunky
 * 
 * @return  true/false  pokud bunka lezi/nelezi mimo hranice
 */
bool isOutOfBounds(Map *map, int row, int col);

/**
 * Zjistuje orientaci bunky, tedy zda vodorovna strana trojhuleniku lezi
 * nahore nebo dole.
 * 
 * @param   row         souradnice radku bunky
 * @param   col         souradnice sloupce bunky
 * 
 * @return  UP/DOWN     pokud strana lezi nahore/dole  
 */
bool checkOrientation(int row, int col);

/**
 * Kontroluje, zda startovni pozice je validni, tedy zda nelezi uvnitr mapy
 * a jestli ma startovni bunka vchod do bludiste.
 * 
 * @param   *map        ukazatel na mapu
 * @param   row         souradnice radku bunky
 * @param   col         souradnice sloupce bunky
 * 
 * @return  true/false  pro platnou/neplatnou souradnici
 */
bool checkStartPos(Map *map, int row, int col);

/**
 * Urcuje, ktera hranice je pri pocatku v danem bode vedena rukou podle pravidla.
 * 
 * @param   *map        ukazatel na mapu
 * @param   row         souradnice radku bunky
 * @param   col         souradnice sloupce bunky
 * 
 * @return  pocatecni hranice B_UPDOWN/B_LEFT/B_RIGHT
 */
int start_border(Map *map, int row, int col, int leftright);

int main(int argc, char **argv)
{
    int falseCounter = 0; // pocitadlo pro zjisteni, zda argument neodpovida pozadovanym nebo nema spravny pocet argumentu
    if (argc == 1)
    {
        fprintf(stderr, "CHYBA: Nespravny pocet argumentu\n");
        printf("Pouzijte: proj3 --help pro napovedu\n");
        return EXIT_FAILURE;
    }
    else
    {
        // Zobrazeni napovedy
        if (checkArg(argv, argc, "--help", 0, false) == 1)
        {
            printHelp();
            return EXIT_SUCCESS;
        }
        // Otestovani, zda je vstupni soubor validni format bludiste
        else if (checkArg(argv, argc, "--test", 1, false) == 1)
        {
            if (testFileStructure(argv[2]))
            {
                if (validateMap(argv[2])) {
                    printf("Valid\n");
                    return EXIT_SUCCESS;
                }
                else
                {
                    printf("Invalid\n");
                    return EXIT_FAILURE;
                }
            }
            else
            {
                printf("Invalid\n");
                return EXIT_FAILURE;
            }
        }
        // Hledani cesty pomoci pravidla prave ruky
        else if (checkArg(argv, argc, "--rpath", 3, false) == 1)
        {
            char *ptr;
            // Prevedeni a overeni argumentu
            int row = strtol(argv[2], &ptr, 10);
            if (ptr == argv[2])
            {
                fprintf(stderr, "CHYBA: Spatny format argumentu\n");
                printf("Pouzijte: proj3 --help pro napovedu\n");
                return EXIT_FAILURE;
            }

            int col = strtol(argv[3], &ptr, 10);
            if (ptr == argv[2])
            {
                fprintf(stderr, "CHYBA: Spatny format argumentu\n");
                printf("Pouzijte: proj3 --help pro napovedu\n");
                return EXIT_FAILURE;
            }
            findPath(RULERIGHT, row - 1, col - 1, argv[4]); // -1, protoze vstupy a vystupy predpokladaji souradnice zacinajici 1,1 misto 0,0
        }
        // Hledani cesty pomoci pravidla leve ruky
        else if (checkArg(argv, argc, "--lpath", 3, false) == 1)
        {
            char *ptr;
            int row = strtol(argv[2], &ptr, 10);
            if (ptr == argv[2])
            {
                fprintf(stderr, "CHYBA: Spatny format argumentu\n");
                printf("Pouzijte: proj3 --help pro napovedu\n");
                return EXIT_FAILURE;
            }
            int col = strtol(argv[3], &ptr, 10);
            if (ptr == argv[2])
            {
                fprintf(stderr, "CHYBA: Spatny format argumentu\n");
                printf("Pouzijte: proj3 --help pro napovedu\n");
                return EXIT_FAILURE;
            }
            findPath(RULELEFT, row - 1, col - 1, argv[4]); // -1, protoze vstupy a vystupy predpokladaji souradnice zacinajici 1,1 misto 0,0
        }
        // Zjisteni, zda je zadany spatny argument nebo spravny se spatnym poctem parametru
        falseCounter += checkArg(argv, argc, "--help", 0, true);
        falseCounter += checkArg(argv, argc, "--test", 1, true);
        falseCounter += checkArg(argv, argc, "--rpath", 3, true);
        falseCounter += checkArg(argv, argc, "--lpath", 3, true);
        // Pripadny vypis chyboveho hlaseni
        if (falseCounter == 0)
        {
            fprintf(stderr, "CHYBA: Neznamy argument\n");
            printf("Pouzijte: proj3 --help pro napovedu\n");
            return EXIT_FAILURE;
        }
    }
}

void printHelp()
{
    printf("Pouziti proj3\n");
    printf("--help\t\t\t\t\tZobrazi tuto napovedu\n");
    printf("--test <soubor>\t\t\t\tOtestuje, zda se v souboru nachazi korektni popis mapy\n");
    printf("--rpath <radek> <sloupec> <soubor>\tNajde cestu bludistem pomoci pravidla prave ruky a vypise kudy cesta vede\n");
    printf("--lpath <radek> <sloupec> <soubor>\tNajde cestu bludistem pomoci pravidla leve ruky a vypise kudy cesta vede\n");
    return;
}

void findPath(int rule, int row, int col, char *fileName)
{
    // Validace mapy pred hledanim
    if (!validateMap(fileName))
    {
        fprintf(stderr, "CHYBA: Mapa neni validni!\n");
        return;
    }
    // Nove nacteni mapy a kontrola pozice
    Map maze;
    constructMap(&maze, fileName);
    loadMap(&maze, fileName);
    if (!checkStartPos(&maze, row, col))
    {
        fprintf(stderr, "CHYBA: Nespravna startovni pozice!\n");
        destructMap(&maze);
        return;
    }
    // Hledani a vypis trasy
    int border; // hranice, ktere se drzi ruka
    switch (rule)
    {
    case RULERIGHT:
        border = start_border(&maze, row, col, RULERIGHT);
        while (!isOutOfBounds(&maze, row, col)) // dokud nevylezeme z bludiste
        {
            printf("%i,%i\n", row + 1, col + 1); // pricitame 1 kvuli formatu souradnic (zadani pozaduje pocatek v 1,1)
            if (border == B_UPDOWN)
            {
                if (checkOrientation(row, col) == DOWN)
                { // kdyz ma bunka dolni hranici
                    if (isborder(&maze, row, col, B_UPDOWN))
                    { // kontrola, zda je dolni hranice pevna
                        if (isborder(&maze, row, col, B_RIGHT))
                        {          // pokud je pevna, kontroluji pravou hranici
                            col--; // pokud je prava hrance pevna, vracim se zpet a ruku davam na horni hranici
                        }
                        else // pokud neni pevna, prochazim skrz a ruku davam na pravou hranici
                        {
                            col++;
                            border = B_RIGHT;
                        }
                    }
                    else // pokud dolni hranice neni pevna, prochazim skrz a ruku davam na levou hranici
                    {
                        row++;
                        border = B_LEFT;
                    }
                }
                else
                { // pokud ma bunka hranici nahore
                    if (isborder(&maze, row, col, B_UPDOWN))
                    { // pokud je pevna
                        if (isborder(&maze, row, col, B_LEFT))
                        { // pokud je pevna i leva, vracim se zpet otoceny
                            col++;
                        }
                        else
                        { // jinak prochazim levou hranici a ruku pokladam znovu na levou
                            col--;
                            border = B_LEFT;
                        }
                    } // pokud horni neni pevna, pak ji projdu, ruku pokladam doprava
                    else
                    {
                        row--;
                        border = B_RIGHT;
                    }
                }
            }
            else if (border == B_RIGHT) // pokud mam ruku na prave hranici
            {
                if (checkOrientation(row, col) == DOWN) // trojuhelnik ma vodorovnou stranu dole
                {
                    if (isborder(&maze, row, col, B_RIGHT)) // pokud je prava pevna
                    {
                        if (isborder(&maze, row, col, B_LEFT)) // pokud je pevna i leva, vracim se zpet otoceny
                        {
                            row++;
                            border = B_LEFT;
                        }
                        else // jinak prochazim a ruku davam nahoru
                        {
                            col--;
                            border = B_UPDOWN;
                        }
                    }
                    else // pokud prava neni pevna, prochazim, ruka zustava na prave, jen v dalsi bunce
                    {
                        col++;
                    }
                }
                else // pokud ma trojuhelnik vodorovnou stranu nahore
                {
                    if (isborder(&maze, row, col, B_RIGHT)) // pokud je prava pevna
                    {
                        if (isborder(&maze, row, col, B_UPDOWN)) // pokud je i horni pevna, vracim se zpet otoceny
                        {
                            col--;
                            border = B_LEFT;
                        }
                        else // jinak prochazim, ruka zustava vpravo
                        {
                            row--;
                        }
                    }
                    else // pokud prava neni pevna, prochazim, ruku davam dolu
                    {
                        col++;
                        border = B_UPDOWN;
                    }
                }
            }
            else if (border == B_LEFT) // ruka je vlevo
            {
                if (checkOrientation(row, col) == DOWN) // trojuhelnik ma vodorovnou stranu dole
                {
                    if (isborder(&maze, row, col, B_LEFT)) // pokud je pevna leva
                    {
                        if (isborder(&maze, row, col, B_UPDOWN)) // pokud je pevna i dolni, tak se vracim, ruku davam doprava
                        {
                            col++;
                            border = B_RIGHT;
                        }
                        else // pokud neni dolni pevna, prochazim, ruku nechavam na leve
                        {
                            row++;
                        }
                    }
                    else // pokud leva neni pevna, prochazim, ruku davam nahoru
                    {
                        col--;
                        border = B_UPDOWN;
                    }
                }
                else // trojhuelnik ma vodorovnou stranu nahore
                {
                    if (isborder(&maze, row, col, B_LEFT)) // pokud je leva pevna
                    {
                        if (isborder(&maze, row, col, B_RIGHT)) // pokud je pevna i prava, pak jdu zpet dolu otoceny
                        {
                            row--;
                            border = B_RIGHT;
                        }
                        else // jinak prochazim, ruku davam dolu
                        {
                            col++;
                            border = B_UPDOWN;
                        }
                    }
                    else // pokud leva nneipevna, prochazim, nechavam ruku vlevo
                    {
                        col--;
                    }
                }
            }
        }
        break;
    /*
        Pro popis algoritmu hledani levou rukou viz. prava ruka
        Postup je obdobny, pouze jsou prevraceny strany.
     */
    case RULELEFT:
        border = start_border(&maze, row, col, RULELEFT);
        while (!isOutOfBounds(&maze, row, col))
        {
            printf("%i,%i\n", row + 1, col + 1);
            if (border == B_UPDOWN)
            {
                if (checkOrientation(row, col) == DOWN)
                {
                    if (isborder(&maze, row, col, B_UPDOWN))
                    {
                        if (isborder(&maze, row, col, B_LEFT))
                        {
                            col++;
                        }
                        else
                        {
                            col--;
                            border = B_LEFT;
                        }
                    }
                    else
                    {
                        row++;
                        border = B_RIGHT;
                    }
                }
                else
                {
                    if (isborder(&maze, row, col, B_UPDOWN))
                    {
                        if (isborder(&maze, row, col, B_RIGHT))
                        {
                            col--;
                        }
                        else
                        {
                            col++;
                            border = B_RIGHT;
                        }
                    }
                    else
                    {
                        row--;
                        border = B_LEFT;
                    }
                }
            }
            else if (border == B_LEFT)
            {
                if (checkOrientation(row, col) == DOWN)
                {
                    if (isborder(&maze, row, col, B_LEFT))
                    {
                        if (isborder(&maze, row, col, B_RIGHT))
                        {
                            row++;
                            border = B_RIGHT;
                        }
                        else
                        {
                            col++;
                            border = B_UPDOWN;
                        }
                    }
                    else
                    {
                        col--;
                    }
                }
                else
                {
                    if (isborder(&maze, row, col, B_LEFT))
                    {
                        if (isborder(&maze, row, col, B_UPDOWN))
                        {
                            col++;
                            border = B_RIGHT;
                        }
                        else
                        {
                            row--;
                        }
                    }
                    else
                    {
                        col--;
                        border = B_UPDOWN;
                    }
                }
            }
            else if (border == B_RIGHT)
            {
                if (checkOrientation(row, col) == DOWN)
                {
                    if (isborder(&maze, row, col, B_RIGHT))
                    {
                        if (isborder(&maze, row, col, B_UPDOWN))
                        {
                            col--;
                            border = B_LEFT;
                        }
                        else
                        {
                            row++;
                        }
                    }
                    else
                    {
                        col++;
                        border = B_UPDOWN;
                    }
                }
                else
                {
                    if (isborder(&maze, row, col, B_RIGHT))
                    {
                        if (isborder(&maze, row, col, B_LEFT))
                        {
                            row--;
                            border = B_LEFT;
                        }
                        else
                        {
                            col--;
                            border = B_UPDOWN;
                        }
                    }
                    else
                    {
                        col++;
                    }
                }
            }
        }
        break;
    }
    destructMap(&maze); // uvolneni mapy z pameti
    return;
}

int checkArg(char **argv, int argc, char *argName, int argLen, bool noerr)
{
    if (strcmp(argv[1], argName) == 0)
    {
        if (argc == argLen + 2) // argLen je pocet parametru bez nazvu souboru a hlavniho argumentu (napr. --rpath)
            return 1;
        else
        {
            if (!noerr)
            {
                fprintf(stderr, "CHYBA: Nespravny pocet argumentu\n");
                printf("Pouzij proj3 --help pro napovedu\n");
            }
            return -1;
        }
    }
    else
    {
        return 0;
    }
}

bool checkStartPos(Map *map, int row, int col)
{
    if (row == 0 && !isborder(map, row, col, B_UPDOWN) && checkOrientation(row, col) == UP)
    {
        // trojuhelnik v horni rade, orientovany vodorovnou stranou nahoru s horni stranou pruchozi
        return true;
    }
    else if (col == 0 && !isborder(map, row, col, B_LEFT))
    {
        // trojuhelnik v levem sloupci s pruchozi levou stranou
        return true;
    }
    else if (row == map->rows - 1 && !isborder(map, row, col, B_UPDOWN) && checkOrientation(row, col) == DOWN)
    {
        // trojuhelnik v posledni rade, orientovany vodorovnou stranou dolu s dolni stranou pruchozi
        return true;
    }
    else if (col == map->cols - 1 && !isborder(map, row, col, B_RIGHT))
    {
        // trojuhelnik v poslednim sloupci s pruchozi pravou stranou
        return true;
    }
    else
    {
        // zadny jiny trojuhelnik neni validni vstup
        return false;
    }
}

bool testFileStructure(char *fileName)
{
    FILE *file = fopen(fileName, "r");
    // Kontrola otevreni souboru
    if (file != NULL)
    {
        int rows;
        int cols;
        int count = 0;
        // Nacteni rozmeru mapy
        count += fscanf(file, "%i %i", &rows, &cols);
        // Kontrola validniho nacteni
        if (count != 2)
        {
            fclose(file);
            return false;
        }
        // Pruchod souborem a cteni jednotlivych cisel, kontrola nacteni a kontrola rozsahu cisel (validni cisla udavajici nastaveni stran policka = 0-7)
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                int num;
                int count = fscanf(file, "%i", &num);
                if (count != 1)
                {
                    fclose(file);
                    return false;
                }

                if (num < 0 || num > 7)
                {
                    fclose(file);
                    return false;
                }
            }
        }
        // Kontrola, zda ma soubor spravne rozmery
        int last;
        if (fscanf(file, "%i", &last) != EOF)
        {
            fclose(file);
            return false;
        }
        else // Pokud jsme se dostali pres vsechny kontroly az sem, soubor je validni
        {
            fclose(file);
            return true;
        }
    }
    else
    {
        fprintf(stderr, "CHYBA: Nepodarilo se otevrit soubor!\n");
        return false;
    }
}

bool validateMap(char *fileName)
{
    // Nacteni mapy ze souboru
    Map map;
    constructMap(&map, fileName);
    if (map.cells == NULL)
    {
        fprintf(stderr, "CHYBA: Nepodarilo se inicializovat mapu!\n");
        return false;
    }
    loadMap(&map, fileName);
    // Kontrola, zda na sebe sedi zdi sousedu (horni na dolni, dolni na horni, leva na pravou, prava na levou)
    for (int i = 0; i < map.rows; i++)
    {
        for (int j = 0; j < map.cols; j++)
        {
            if (isborder(&map, i, j, B_UPDOWN)) // pokud je zed pevna
            {
                if (checkOrientation(i, j) == UP)
                {
                    if (!isOutOfBounds(&map, i - 1, j)) // a pokud nelezi policko na okraji bludiste
                    {
                        if (!isborder(&map, i - 1, j, B_UPDOWN)) // musi byt pevna i sousedni zed
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    if (!isOutOfBounds(&map, i + 1, j))
                    {
                        if (!isborder(&map, i + 1, j, B_UPDOWN))
                        {
                            return false;
                        }
                    }
                }
            }
            if (isborder(&map, i, j, B_LEFT))
            {
                if (!isOutOfBounds(&map, i, j - 1))
                {
                    if (!isborder(&map, i, j - 1, B_RIGHT))
                    {
                        return false;
                    }
                }
            }
            if (isborder(&map, i, j, B_RIGHT))
            {
                if (!isOutOfBounds(&map, i, j + 1))
                {
                    if (!isborder(&map, i, j + 1, B_LEFT))
                    {
                        return false;
                    }
                }
            }
        }
    }
    destructMap(&map); // uvolneni mapy z pameti
    return true;
}

bool isOutOfBounds(Map *map, int row, int col)
{
    // overeni ,zda getCellPos vraci validni hodnotu pri zadanych souradnicich
    if (getCellPos(map, row, col) < 0 || getCellPos(map, row, col) > map->rows * map->cols)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool checkOrientation(int row, int col)
{
    // sude trojuhelniky na sudych radcich maji vodorovnou hranici nahore
    if (row % 2 == 0 && col % 2 == 0)
    {
        return UP;
    }
    // liche trojuhelniky na lichych radcich maji vodorovnou hranici nahore
    else if (row % 2 == 1 && col % 2 == 1)
    {
        return UP;
    }
    // vsechny ostatni trojuhelniky maji vodorovnou hranici dole
    else
    {
        return DOWN;
    }
}

bool isborder(Map *map, int row, int col, int border)
{
    int n = map->cells[getCellPos(map, row, col)] - '0'; // prevod na cislo
    // prevod na binarni cislo delenim
    int bit0 = n % 2;
    int bit1 = (n / 2) % 2;
    int bit2 = (n / 4) % 2;
    // overeni podle pravide lze zadani
    if (border == B_UPDOWN && bit2 == 1)
    {
        return true;
    }
    else if (border == B_LEFT && bit0 == 1)
    {
        return true;
    }
    else if (border == B_RIGHT && bit1 == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int getCellPos(Map *map, int row, int col)
{
    if (col < map->cols && col >= 0 && row < map->rows && row >= 0) // kontrola, ze jsou souradnice uvnitr pole
    {
        return map->cols * row + col; // prevod na index v poli bunek
    }
    else
    {
        return -1; // pri chybe vraci -1
    }
}

void constructMap(Map *map, char *fileName)
{
    int rows, cols;                  // promenne pro nacteni velikosti mapy
    if (testFileStructure(fileName)) // test struktury a nacteni velikosti
    {
        FILE *file = fopen(fileName, "r");
        if (file != NULL)
        {
            fscanf(file, "%i %i", &rows, &cols);
            fclose(file);
        } else {
            fprintf(stderr, "CHYBA: Nepodarilo se otevrit soubor!\n");
            map->cells = NULL;
            map->rows = 0;
            map->cols = 0;
            return;
        }
    }
    else // pokud neni struktura validni, pak program vraci prazdnou mapu
    {
        fprintf(stderr, "CHYBA: Spatny format souboru!\n");
        map->cells = NULL;
        map->rows = 0;
        map->cols = 0;
        return;
    }
    // alokace pameti a prirazeni parametru do struktury
    unsigned char *cells = malloc(rows * cols * sizeof(unsigned char));
    map->rows = rows;
    map->cols = cols;
    map->cells = cells;
    return;
}

void loadMap(Map *map, char *fileName)
{
    if (testFileStructure(fileName)) // test struktury souboru
    {
        FILE *file = fopen(fileName, "r");
        int rows, cols;
        if (file != NULL) {
            fscanf(file, "%i %i", &rows, &cols);        // nacteni velikosti
            if (rows != map->rows || cols != map->cols) // overeni struktury
            {
                fprintf(stderr, "CHYBA: Soubor neodpovida prazdne mape!\n");
                fclose(file);
                return;
            }
            for (int i = 0; i < rows; i++) // nacteni postupne vsech bunek
            {
                for (int j = 0; j < cols; j++)
                {
                    int val;
                    fscanf(file, "%i", &val);
                    map->cells[getCellPos(map, i, j)] = val + '0'; // prevod na char a ulozeni do struktury
                }
            }
            fclose(file);
        }
        else
        {
            fprintf(stderr,"CHYBA: Nepodarilo se otevrit soubor!\n");
        }
        
    }
    else
    {
        fprintf(stderr, "CHYBA: Spatny format souboru\n");
    }
    return;
}

void destructMap(Map *map)
{
    map->rows = 0;
    map->cols = 0;
    free(map->cells);
    map->cells = NULL;
    return;
}

int start_border(Map *map, int row, int col, int leftright)
{
    if (leftright == RULERIGHT)
    {
        if (col == 0) // levy sloupec
        {
            if (row == 0) // osetreni leveho horniho rohu
            {
                if (isborder(map, row, col, B_LEFT)) // osetreni smeru vstupu (zleva/shora)
                {
                    return B_LEFT;
                }
                else if (!isborder(map, row, col, B_UPDOWN)) // osetreni prazdneho rohu
                {
                    return B_RIGHT;
                }
            }
            else if (row == map->rows - 1) // osetreni leveho dolniho rohu
            {
                if (isborder(map, row, col, B_LEFT)) // smer vstupu
                {
                    return B_RIGHT;
                }
                else if (!isborder(map, row, col, B_UPDOWN) && checkOrientation(row, col) == DOWN) // prazdny roh
                {
                    return B_RIGHT;
                }
            }
            // mimo rohy
            if (checkOrientation(row, col) == UP)
                return B_RIGHT;
            else
                return B_UPDOWN;
        }
        else if (col == map->cols - 1) // pravy sloupec
        {
            if (row == 0) // pravy horni roh
            {
                if (isborder(map, row, col, B_RIGHT)) // smer vstupu
                {
                    return B_LEFT;
                }
                else if (!isborder(map, row, col, B_UPDOWN) && checkOrientation(row, col) == UP) // prazdny roh
                {
                    return B_LEFT;
                }
            }
            else if (row == map->rows - 1) // pravy dolni roh
            {
                if (isborder(map, row, col, B_RIGHT)) // smer vstupu
                {
                    return B_RIGHT;
                }
                else if (!isborder(map, row, col, B_UPDOWN) && checkOrientation(row, col) == DOWN) // prazdny roh
                {
                    return B_LEFT;
                }
            }
            // mimo rohy
            if (checkOrientation(row, col) == UP)
                return B_UPDOWN;
            else
                return B_LEFT;
        }
        else if (row == 0) // horni radek
        {
            return B_LEFT;
        }
        else if (row == map->rows - 1) // spodni radek
        {
            return B_RIGHT;
        }
    }
    else if (leftright == RULELEFT)
    {
        if (col == 0) // levy sloupec
        {
            if (row == 0) // levy horni roh
            {
                if (isborder(map, row, col, B_LEFT)) // smer vstupu
                {
                    return B_RIGHT;
                }
                else if (!isborder(map, row, col, B_UPDOWN)) // prazdny roh
                {
                    return B_RIGHT;
                }
            }
            else if (row == map->rows - 1) // levy dolni roh
            {
                if (isborder(map, row, col, B_LEFT)) // smer vstupu
                {
                    return B_LEFT;
                }
                else if (!isborder(map, row, col, B_UPDOWN) && checkOrientation(row, col) == DOWN) // prazdny roh
                {
                    return B_RIGHT;
                }
            }
            // mimo rohy
            if (checkOrientation(row, col) == UP)
                return B_UPDOWN;
            else
                return B_RIGHT;
        }
        else if (col == map->cols - 1) // pravy sloupec
        {
            if (row == 0) // pravy horni roh
            {
                if (isborder(map, row, col, B_RIGHT)) // smer vstupu
                {
                    return B_RIGHT;
                }
                else if (!isborder(map, row, col, B_UPDOWN) && checkOrientation(row, col) == UP) // prazdny roh
                {
                    return B_LEFT;
                }
            }
            else if (row == map->rows - 1) // pravy dolni roh
            {
                if (isborder(map, row, col, B_RIGHT)) // smer vstupu
                {
                    return B_LEFT;
                }
                else if (!isborder(map, row, col, B_UPDOWN) && checkOrientation(row, col) == DOWN) // prazdny roh
                {
                    return B_LEFT;
                }
            }
            // mimo rohy
            if (checkOrientation(row, col) == UP)
                return B_LEFT;
            else
                return B_UPDOWN;
        }
        else if (row == 0) // horni radek
        {
            return B_RIGHT;
        }
        else if (row == map->rows - 1) // spodni radek
        {
            return B_LEFT;
        }
    }
    return -1;
}