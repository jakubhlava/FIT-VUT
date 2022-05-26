/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file ifj20c.c
 * @brief Hlavní modul překladače pro jazyk IFJ20
 * @author Jakub Hlava (xhlava52)
 * @author Thi Bao Ngoc Vu (xvuthi00)
 * @author Lukáš Kraus (xkraus13)
 * @author Jan Pryč (xprycj00)
 * @date 10/2020
 */

#include "ifj20c.h"
#include "codegen.h"

int main() {
    symTable ST;
    tCache tokenCache;

    gen g;
    int returnVal = SUCCESS;
    initSymTable(&ST);
    initCache(&tokenCache);
    init_gen(&g);
    g.st = &ST;
    initVStack(g.v);
    initAStack(g.params);
    gen b;//stacky kontrolujici vstupni argumenty
    init_gen(&b);
    b.st = &ST;
    initVStack(b.v);
    initAStack(b.params);

    gen rv;
    init_gen(&rv);
    rv.st = &ST;
    initVStack(rv.v);
    initAStack(rv.params);
    
    
    // KROK 1 - plnění cache tokeny

    returnVal = fillCache(&tokenCache);
    resetHead(&tokenCache); // příprava na čtení

    // KROK 2 - předplnění tabulky symbolů funkcemi
    if (returnVal == SUCCESS) {
        prefill(&ST);
    }

    // KROK 3 - hledání definic funkcí prvním průchodem dvojprůchodové syntaktické analýzy
    // Při prvním průchodu se zároveŇ hledá i prolog
    // POZOR! Čtecí hlava paměti tokenů zůstává po prvním průchodu za prologem - je možné jej v druhém průchodu ignorovat
    if (returnVal == SUCCESS) {
        returnVal = firstPass(&ST, &tokenCache);
    }

    if (returnVal == SUCCESS) { // spuštění druhého průchodu překladu
        gen_header();
        returnVal = compile(&ST, &tokenCache, &g, &b, &rv);
    }
    
    destroyCache(&tokenCache);
    destroySymTable(&ST);
    destroyGen(&g);
    fprintf(stderr, "Výsledek překladu: %d\n",returnVal);
    return returnVal;
}

/**
 * Spuštění druhého průchodu překladu, generování kostry kódu
 */
int compile(symTable* ST, tCache* tokenCache, gen* g, gen* b, gen *rv) {
    int returnVal;
    returnVal = func_def(tokenCache, ST, g, b, rv);
    return returnVal;
}

/**
 * Načtení všech tokenů do paměti
 * Kontroluje lexikální chyby
 * Ovlivňuje výstupní hodnotu překladu
 */
int fillCache(tCache* tokenCache) {
    int status = loadTokens(tokenCache);
    switch (status) {
        case LEX_FINISHED:
            fprintf(stderr, "Načítání tokenů úspěšně dokončeno!\n");
            return SUCCESS;
            break;
        case LEX_ERR_INTERNAL:
            fprintf(stderr, "Interní chyba překladu\n");
            return ERR_INTERNAL;
            break;
        case LEX_ERR_ANALYSIS:
            fprintf(stderr, "Lexikální chyba v překladu\n");
            return ERR_LEXICAL;
            break;
        default:
            fprintf(stderr, "Neočekávaná interní chyba (předčasný konec načítání tokenů ze zdrojového kódu)\n");
            return ERR_INTERNAL;
            break;
    }
}


/**
 * Předplnění tabulky symbolů vestavěnými funkcemi
 */
void prefill(symTable* table) {
    OUTARGS(inputs, VAR_STRING, VAR_INT);
    BUILTIN_NOARG(inputs, 2);

    OUTARGS(inputi, VAR_INT, VAR_INT);
    BUILTIN_NOARG(inputi, 2);

    OUTARGS(inputf, VAR_FLOAT64, VAR_INT);
    BUILTIN_NOARG(inputf, 2);

    INARGS(int2float, VAR_INT);
    OUTARGS(int2float, VAR_FLOAT64);
    BUILTIN(int2float, 1, 1);

    INARGS(float2int, VAR_FLOAT64);
    OUTARGS(float2int, VAR_INT);
    BUILTIN(float2int, 1, 1);

    INARGS(len, VAR_STRING);
    OUTARGS(len, VAR_INT);
    BUILTIN(len, 1, 1);

    INARGS(substr, VAR_STRING, VAR_INT, VAR_INT);
    OUTARGS(substr, VAR_STRING, VAR_INT);
    BUILTIN(substr, 3, 2);

    INARGS(ord, VAR_STRING, VAR_INT);
    OUTARGS(ord, VAR_INT, VAR_INT);
    BUILTIN(ord, 2, 2);

    INARGS(chr, VAR_INT);
    OUTARGS(chr, VAR_STRING, VAR_INT);
    BUILTIN(chr, 1, 2);

    addFunc(table, "print", 0, 0, NULL, NULL); // primárně kvůli redefinicím, print je speciální funkce o n parametrech
}

/**
 * Provedení prvního průchodu SA, který detekuje pouze definice funkcí
 * Ověřuje redefinice, konflikty a chybějící main
 * Ovlivňuje podle těchto ověření návratovou hodnotu překladu
 */
int firstPass(symTable* ST, tCache* tokenCache) {
    int firstPassStatus = SYN_RET_SUCCESS;
    while (firstPassStatus == SYN_RET_SUCCESS) {
        firstPassStatus = loadFuncDecl(tokenCache, ST);
    }
    switch (firstPassStatus) {
        case SYN_RET_EOF:
            fprintf(stderr, "První průběh SA úspěšně dokončen\n");
            break;
        case SYN_RET_INTERNAL:
            fprintf(stderr, "CHYBA: interní chyba alokace paměti syntaktické analýzy\n");
            return ERR_INTERNAL;
            break;
        case SYN_RET_SYNTAX:
            fprintf(stderr, "CHYBA: syntaktická chyba v definici funkce\n");
            return ERR_SYNTAX;
            break;
        case SYN_RET_REDEF:
            fprintf(stderr, "CHYBA: redefinice funkce\n");
            return ERR_DEFINES;
            break;
        default:
            fprintf(stderr, "CHYBA: nespecifikovaná interní chyba syntaktické analýzy\n");
            return ERR_INTERNAL;
            break;
    }

    // ověření existence a správnosti funkce main
    sResult s;
    search(ST, "main", &s);
    if (s.resultType != TYPE_FUNC) {
        fprintf(stderr, "CHYBA: neexistuje main\n");
        return ERR_DEFINES;
    } else if (s.data.func->inArgC != 0 || s.data.func->outArgC != 0) {
        fprintf(stderr, "CHYBA: main má špatný počet parametrů\n");
        return ERR_FUNC;
    }

    resetHead(tokenCache);
    int status = getProlog(tokenCache);
    switch (status) {
        case SYN_RET_SYNTAX:
            fprintf(stderr, "CHYBA: chybějící prolog\n");
            return ERR_SYNTAX;
        break;
        case SYN_RET_SEMANTIC_OTHER:
            fprintf(stderr, "Chybný package v prologu\n");
            return ERR_SYNTAX;
        break;
        default: 
            fprintf(stderr, "prolog ok\n");
        break;
    }
    return SUCCESS;
}