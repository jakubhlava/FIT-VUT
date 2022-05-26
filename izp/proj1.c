/* IZP Projekt 1: Prace s textem
*  Autor: Jakub Hlava
*  Login: xhlava52
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Velikosti telefonniho seznamu
#define LINE_LEN 101
#define MAX_CONTACTS 100

// Velikosti pole vzoru pro hledani
#define S_WIDTH 6
#define S_COUNT 9

// Globalni promenne pro telefonni seznam
char names[MAX_CONTACTS][LINE_LEN];
char numbers[MAX_CONTACTS][LINE_LEN];

// Pole se vzory pro hledani
char searcher[S_COUNT][S_WIDTH] = {"2abc", "3def", "4ghi", "5jkl", "6mno", "7pqrs", "8tuv", "9wxyz", "0+"};

// Prototypy funkci
int loadcontacts();
bool checkcontact(char criteria[], char *name, char *num);

int main(int argc, char *argv[])
{
    /*
     * Funkce main, v argv akceptuje pouze 0-1 spoustecich argumentu 
     */
    loadcontacts(); // Nacteni kontaktu do globalnich promennych
    // Vypis vsech kontaktu, pokud nemame argument
    if (argc == 1)
    {
        for (int i = 0; i < MAX_CONTACTS; i++)
        {
            if (*names[i] == '\0')
            {
                break;
            }
            printf("%s, %s\n", names[i], numbers[i]);
        }
    }
    // Pruchod vsech ulozenych kontaktu a porovnani s hledanym vyrazem
    else if (argc == 2)
    {
        bool found = false;
        for (int i = 0; i < MAX_CONTACTS; i++)
        {
            if (*names[i] == '\0')
            {
                break;
            }
            else if (checkcontact(argv[1], names[i], numbers[i]) == true)
            {
                printf("%s, %s\n", names[i], numbers[i]);
                found = true;
            }
        }
        if (found == false)
        {
            printf("Not found\n");
        }
    }
    else
    {
        fprintf(stderr, "Spatny pocet argumentu! (Spravne 0-1)");
    }
    return 0;
}

int loadcontacts()
{
    /*
    * Funkce loadcontacts
    * Nacita kontakty z stdin a uklada je do globalnich promennych names a numbers
    */
    int i = 0;
    int arrsw = 0; // Prepinac jmeno/cislo
    while (i < MAX_CONTACTS)
    {
        int eofstop = 0;             // Sig. zastaveni pri EOF
        int run = 1;                 
        int counter = 0;             // Pocitadlo znaku
        char buf[LINE_LEN] = {}; // Mezipamet pro nacitana data
        while (run)
        {
            char c = getchar();
            if (c == '\n' || c == '\r') // Aby fungoval i CRLF konec radku pod Linuxem
            {
                break;
            }
            else if (c == EOF)
            {
                eofstop = 1;
                break;
            }
            else
            {
                // Precteni zbytku radku, ktery ma > 100 znaku
                if (counter >= LINE_LEN-1)
                {
                    while (c != '\n')
                    {
                        c = getchar();
                    }
                    buf[counter] = '\0';
                    break;
                }
                buf[counter] = c;
                counter++;
            }
        }
        if (buf[0] != '\n' && buf[0] != '\r' && buf[0] != '\0') // Zahozeni neplatnych zbytku radku
        {
            if (arrsw == 0)
            {
                strcpy(names[i], buf); // Zapis do pole
                arrsw = 1;
            }
            else
            {
                strcpy(numbers[i], buf);
                arrsw = 0;
                i++;
            }
        }
        if (eofstop == 1) // Ukonceni nacitani pri EOF
        {
            break;
        }
    }
    return 1;
}

bool checkcontact(char criteria[], char *name, char *num)
{
    /*
    * Funkce checkcontact
    * Prebira parametry:
    * char criteria[] - ciselna kriteria z parametru programu
    * char *name - jmeno k porovnani
    * char *num - cislo k porovnani
    * Vraci: true pri nalezeni shody, false pri neuspechu hledani
    */
    int len = strlen(criteria); 
    int cpos = 0;               // pozice v retezci s kriterii hledani
    char w;                     // pracovni znak
    int pos = 0;                // pozice v textu
    // Hledani ve jmene
    do
    {
        w = name[pos];
        // prevod na mala pismena
        if (w > 'A' && w < 'Z')
        {
            w = w - ('A' - 'a');
        }

        int critNum = criteria[cpos] - '0';
        char *critList;
        // osetreni velikosti pole vyhledavacich vzoru
        if (critNum - 2 >= 0 && critNum - 2 <= S_COUNT)
        {
            critList = searcher[critNum - 2];
        }
        else if (critNum == 0)
        {
            critList = searcher[S_COUNT - 1];
        }
        else
        {
            critList = NULL;
            break;
        }
        // porovnavani pismen
        for (int x = 0; x < S_WIDTH; x++)
        {
            if (critList[x] == '\0')
            {
                break;
            }
            else
            {
                if (w == critList[x])
                {
                    cpos++;
                    if (cpos == len)
                    {
                        return true;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        pos++;
    } while (w != '\0');
    // vynulovani pozice a hledani v telefonnim cisle
    pos = 0;
    cpos = 0;
    do
    {
        w = num[pos];
        if (w == criteria[cpos])
        {
            cpos++;
            if (cpos == len)
            {
                return true;
            }
        }
        pos++;
    } while (w != '\0');

    return false;
}
