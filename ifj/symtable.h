/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file symtable.h
 * @brief Tabulka symbolů pro překladač jazyka IFJ20
 * @author Jakub Hlava (xhlava52)
 * @author Thi Bao Ngoc Vu (xvuthi00)
 * @date 11/2020
 */

#ifndef _SYMTABLE_HEADER
#define _SYMTABLE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <stdbool.h>
#include "expstr.h"
#include "fwd.h"

/*
 *  FUNKCE PRO OBSLUHU TABULKY SYMBOLŮ
 */


/**
 * @brief Funkce pro posun do hlubšího scopu
 * Funkci je potřeba zavolat při každé detekci posunu do hlubšího scopu v rámci jedné funkce,
 * aby tabulka správně řadila proměnné a uvolňovala je po skončení scopu.
 * @param   T           pointer na inicializovanou tabulku symbolů
 * @return STATUS_OK při úspěchu nebo STATUS_INT_ERR při interní chybě (malloc)
 */
int startScope(symTable* T);    

/**
 * @brief Funkce pro posun do volané funkce
 * Tuto funkci je potřeba zavolat pokaždé, když se detekuje volání funkce v analyzovaném kódu.
 * Zajistí se tím oddělení a uložení existujících identifikátoru od těch, které vzniknou po volání funkce.
 * Parametry funkce je nutné registrovat až PO volání startCall();
 * @param   T           pointer na inicializovanou tabulku symbolů
 * @param   identifier  identifikátor volané funkce
 * 
 * @return STATUS_OK při úspěchu nebo STATUS_INT_ERR při interní chybě (malloc) nebo STATUS_PROG_ERR při pokusu zavolat nedefinovanou funkci
 */
int startCall(symTable* T, char* identifier);     


/**
 * @brief Funkce pro vyhledávání indentifikátoru v tabulce symbolů
 * Funkce prohledává aktuální scope, poté vyšší scopy a následně tabulku funkcí, výsledek vyhledávání zapíše do
 * struktury předané v parametru result.
 * @param   T           pointer na inicializovanou tabulku symbolů
 * @param   query       řetězec s hledaným identifikátorem
 * @param   result      pointer na strukturu sResult, kam bude vrácen výsledek vyhledávání
 * @see     sResult
 */
void search(symTable* T, char* query, sResult* result); 

/**
 * @brief Ověřuje, zda hledaný identifikátor v rámci viditelných záznamů v TS je funkcí
 * @param   T           pointer na inic. tabulku symbolů
 * @param   query       hledaný identifikátor
 */
bool isFunc(symTable *T, char* query);

/**
 * @brief Ověřuje, zda hledaný identifikátor v rámci viditelných záznamů v TS je proměnnou
 * @param   T           pointer na inic. tabulku symbolů
 * @param   query       hledaný identifikátor
 */
bool isVar(symTable *T, char* query, bool accept_);

/**
 * @brief Získá data o funkci s identifikátorem query
 * @param   T           pointer na inic. tabulku symbolů
 * @param   query       hledaný identifikátor
 * @return  strukturu s daty o funkci
 * @return  NULL, pokud identifikátor není funkce
 */
fData* getFuncData(symTable *T, char* query);

/**
 * @brief Získá data o proměnné s identifikátorem query
 * @param   T           pointer na inic. tabulku symbolů
 * @param   query       hledaný identifikátor
 * @return  strukturu s daty o proměnné
 * @return  NULL, pokud identifikátor není proměnná
 */
vData* getVarData(symTable *T, char* query);



/**
 * @brief Funkce pro vrácení aktuálně volané funkce
 * Pokud právě probíhá volání nějaké funkce, uloží do pointeru result data o ní
 * jinak nastaví pointer na NULL
 * @param   T           pointer na inicializovanou tabulku symbolů
 * @param   result      pointer na strukturu fData
 */
void getCall(symTable* T, fData** result); 


/**
 * @brief Funkce pro signalizaci návratu z volání funkce
 * Tuto funkci je nutné volat při každé detekci návratu z volání funkce v analyzovaném kódu.
 * Funkce zahodí všechny existující scopy, které byly v rámci volání vytvořeny vč. všech proměnných a jako
 * aktuální funkci pro vyhledávání označí tu, ze které bylo volání provedeno.
 * @param   T           pointer na inicializovanou tabulku scopů
 * @return  STATUS_OK při úspěchu, STATUS_INT_ERR při návratu z neexistujícího volání
 */
int endCall(symTable* T);       // Posun zpět v rámci volání

/**
 * @brief Funkce pro posun do vyššího scopu
 * Tuto funkci je nutné volat při každé detekci konce scopu v rámci analyzovaného kódu.
 * Funkce zahodí všechny proměnné existující v tomto scopu a jako aktivní označí předcházející scope.
 * @param   T           pointer na inicializovanou tabulku symbolů
 * @return  STATUS_OK při úspěchu, STATUS_INT_ERR při ukončení neexistujícího scopu
 */
int endScope(symTable* T);      // Posun zpět v rámci scopů


/**
 * @brief Funkce pro deklaraci proměnné v rámci aktuálního scopu
 * Zaregistruje k aktuálnímu scopu identifikátor proměnné a přiřadí mu datový typ
 * @param   T           pointer na inicializovanou tabulku symbolů
 * @param   key         název proměnné/identifikátoru - slouží zároveŇ jako vyhledávací klíč
 * @param   vType       datový typ proměnné zadaný jedním z příslušných maker
 * @param   gen         pointer na pomcnou strukturu generátoru
 * @return  STATUS_OK při úspěchu
 * @return  STATUS_INT_ERR při interní chybě (malloc)
 * @return  STATUS_PROG_ERR při pokusu o přidání existujícího identifikátoru v rámci scopu
 */
int addVar(symTable* T, char* key, int vType, gen* g);       // Deklarace proměnné

/**
 * @brief Funkce pro deklaraci globální funkce
 * Zaregistruje do globální tabulky funkcí identifikátor funkce spolu s informacemi datových typech a počtech parametrů
 * @param   T           pointer na inicializovanou tabulku symbolů
 * @param   key         název funkce/identifikátoru - slouží zároveň jako vyhledávací klíč
 * @param   inArgNum    počet vstupních argumentů přidávané funkce
 * @param   outArgNum   počet výstupních argumentů přidávané funkce
 * @param   inArgType   pole s typy vstupních argumentů
 * @param   outArgType  pole s typy výstupních argumentů
 * @return  STATUS_OK při úspěchu
 * @return  STATUS_INT_ERR při interní chybě (malloc)
 * @return  STATUS_PROG_ERR při pokusu o přidání existujícího identifikátoru v rámci scopu
 */
int addFunc(symTable* T, char* key, int inArgNum, int outArgNum, int* inArgType, int* outArgType);        //Deklarace funkce

/**
 * @brief Funkce pro inicializaci tabulky symbolů
 * Je nutné ji zavolat nad každou alokovanou, neinicializovanou nebo i deinicializovanou tabulkou před jejím použítím
 * Nesmí se volat nad inicializovanou tabulkou bez předchozí deinicializace, jinak nastane memory leak
 * @param   T       pointer na neinicializovanou tabulku symbolů
 */
void initSymTable(symTable *T);     // Iinicializace tabulky symbolů

/**
 * @brief Funkce pro korektní uvolnění a deinicializaci tabulky symbolů
 * Zahodí všechny uložené identifikátory, scopy, volání a funkce a vrátí tabulku do stavu, kdy je možné ji znovu inicializovat
 * Je nutné tuto funkci zavolat před ukončením používání tabulky, aby došlo ke korektnímu uvolnění paměti
 * @param   T       pointer na inicializovanou tabulku symbolů
 */
void destroySymTable(symTable *T);  // Deinicializace tabulky symbolů

// ===============================================================================
//  POMOCNÉ FUNKCE
// ===============================================================================

int scopeUp(symTable *T);
int callStackUp(symTable *T);
int compareKeys(char* search, char* node);
sItem* actualScope(symTable* T);
tNode* actualRoot(symTable* T);
tNode* createNode(char* key, sData data);
int insertNode(tNode* node , char* key, sData data);
int checkNodeExist(char* search, tNode* node);
int findSameVar(symTable* T, tNode* node, char* search);
int newCallStackItem (symTable *T);
int newScope(symTable *T);
int deleteScope(symTable* T);
void deleteVarTree(tNode* n);
void deleteFuncTree(tNode* n);

#endif