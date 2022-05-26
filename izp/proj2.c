#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

// Konstanta pro zaverny saturacni proud diody dle zadani
#define I0 1e-12
// Konstanta pro tepelny proud dle zadani
#define UT 25.8563e-3

//#define DEBUG

//#define ALTEPS

/* Funkce diode
 * Pocita pracovni napeti na diode pomoci metody puleni intervalu a Shockleyovy rovnice
 * Prijima argumenty:
 * u0 = vstupni napeti obvodu ve voltech
 * r = odpor predradneho rezistoru v ohmech
 * eps = maximalni hodnota odchylky vysledku
 * 
 * Navratova hodnota:
 * Pracovni bod diody ve voltech
*/
double diode(double u0, double r, double eps);

/* Funkce shockley
 * Pomocna funkce, ktera provadi vypocet proudu za pomoci vyjadreni z Shockleyovy rovnice a pouziti Kirchhoffova zakona
 * Prijima argumenty:
 * u0 = vstupni napeti obvodu ve voltech
 * u = pracovni napeti diody
 * r = odpor predradneho rezistoru v ohmech
 * 
 * Navratova hodnota:
 * Odchylka eps od presneho napeti u
 */
double shockley(double u0, double u, double r);

/* Funkce signum
 * Implementace znamenkove funkce
 * Prijima argumenty:
 * x = cislo, ke ktere chceme ziskat znamenko
 * 
 * Navratova hodnota:
 * 1.0 pro x > 0
 * -1.0 pro x < 0
 */
double signum(double x);

int main(int argc, char const *argv[])
{
    /* Pri spusteni program prebira 3 argumenty - U0, R, EPS
     * Pouziti pro spusteni programu: proj2 <U0> <R> <EPS>
     * U0 = vstupni napeti obvodu ve voltech
     * R = odpor predradneho rezistoru v ohmech
     * EPS = maximalni hodnota odchylky vysledku
     * 
     * Vystup programu je ve formatu:
     * Up=X V
     * Ip=Y A
     * Kde X je pracovni napeti diody a Y je pracovni proud diody
    */
    if (argc != 4)
    {
        fprintf(stderr,"CHYBA: Nespravny pocet argumentu!\n");
        fprintf(stderr,"Pouziti programu: proj2 <U0> <R> <EPS>\n");
        return 1;
    }

    // Pracovni promenne, vstup z argumentu prevedeny na korektni format, kontrola spravnosti zadanych dat
    char *end;
    bool err = false;
    double u0 = strtod(argv[1], &end);
    if (end == argv[1] || u0 < 0)
    {
        fprintf(stderr,"CHYBA: Chybny argument <U0>\n");
        err = true;
    }
    double r = strtod(argv[2], &end);
    if (end == argv[2] || r < 0)
    {
        fprintf(stderr,"CHYBA: Chybny argument <R>\n");
        err = true;
    }
    double eps = strtod(argv[3], &end);
    if (end == argv[3] || eps < 0)
    {
        fprintf(stderr,"CHYBA: Chybny argument <EPS>\n");
        err = true;
    }
    // Ukonceni pri nespravnych argumentech
    if (err)
        return 1;
    
    // Osetreni absolutni presnosti double
    if (eps < 1e-15)
        eps = 1e-15;
   
    

    // Vypocet a vypis pracovniho napeti a dopocet proudu pomoci vzorcu pro pracovni bod diody
    double up = diode(u0, r, eps);
    double ip = I0 * (exp(up / UT) - 1);
    printf("Up=%g V\n", up);
    printf("Ip=%g A\n", ip);
    return 0;
}

double diode(double u0, double r, double eps)
{
    double low = 0;                       // Dolni mez intervalu
    double high = u0;                     // Horni mez intervalu
    double up = (low + high) / 2;         // Priblizna hodnota pracovniho napeti, ve chvili deklarace je priblizna hodnota napeti rovna stredu intervalu moznych napeti
    double calculated = shockley(u0, up, r); // Aktualni vysledek vyjadreneho vyrazu
    // Dokud vypoctena odchylka nebude mensi nez odchylka, kterou pozaduje uzivatel, zpresnujeme vypocet pulenim intevalu
    #ifndef ALTEPS
    while (fabs(high-low) >= eps)
    #else
    while (fabs(calculated) >= eps)
    #endif
    {
        // Postupne se zmensuje interval moznych napeti nasledujicim principem
        if (signum(shockley(u0, low, r)) == signum(calculated)) // Pokud ma funkce v dolni mezi stejne znamenko jako ve stredu, pak se za dolni mez dosadi puvodni stred
        {
            low = up;
        }
        else if (signum(shockley(u0, high, r)) == signum(calculated)) // Pokud ma funkce v horni mezi stejne znamenko jako ve stredu, pak se za horni mez dosadi puvodni stred
        {
            high = up;
        } else {
            return up;
        }
        if (fabs(high-low) >= eps)
        {  
        up = (low + high) / 2; // Urceni stredu zmenseneho intervalu = priblizne hodnoty pracovniho napeti 
        calculated = shockley(u0, up, r); // Novy vysledek v novem stredu
        }

        #ifdef DEBUG
        printf("<low: %.15f, mid: %.15f, high: %.15f>, intervalSize: %.15f, eval: %.15f, eps: %.15f\n", low, up, high, fabs(high-low), calculated, eps);
        #endif
    }
    return up;
}

double shockley(double u0, double u, double r)
{
    return I0 * (exp(u / UT) - 1) - (u0 - u) / r;
}

double signum(double x)
{
    return copysign(1.0, x);
}
