/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file syntactic.h
 * @brief Syntaktický analyzátor překladače jazyka IFJ20
 * @author Jakub Hlava (xhlava52)
 * @author Thi Bao Ngoc Vu (xvuthi00)
 * @author Lukáš Kraus (xkraus13)
 * @author Jan Pryč (xprycj00)
 * @date 10/2020
 */

#ifndef _SYNTACTIC_HEADER
#define _SYNTACTIC_HEADER

#include <stdio.h>
#include <stdlib.h>
#include "lexical.h"
#include "symtable.h"
#include "codegen.h"
#include "fwd.h"

#define SYN_F_START             0
#define SYN_F_FUNC              1
#define SYN_F_FUNCID            2
#define SYN_F_PARAMLEFTB        3
#define SYN_F_PARAMRIGHTB       4
#define SYN_F_PARAMID           5
#define SYN_F_PARAMTYPE         6
#define SYN_F_PARAMDIV          7
#define SYN_F_RETLEFTB          8
#define SYN_F_RETRIGHTB         9
#define SYN_F_RETTYPE           10
#define SYN_F_RETDIV            11
#define SYN_F_FINISH            12
#define SYN_F_EXIT              99

// readToken flagy
#define STAY 0
#define NEXT 1
#define EOL_IGNORE  2
#define NOPRINT 4

// flagy pro single_right
#define EXP_END_EOL 0
#define EXP_END_SEPARATOR 1
#define EXP_END_SEMICOLON 2
#define EXP_END_START_SECTION 3
#define EXP_END_EOL_OR_SEPARATOR 4 

#define FLAG_DECLARATION    8
#define FLAG_ASSIGNMENT     16

int DEBUG_parse_all_tokens();
void DEBUG_print_token(token *t);
char* DEBUG_tokchar(token *t);

typedef struct cacheItem {
    token* tok;
    struct cacheItem* ptrNext;
} cacheItem;

typedef struct tokenCache {
    cacheItem* first;
    cacheItem* head;
    cacheItem* bookmark;
} tCache;

typedef struct retValItem {//SEMANTIKA - PAMET NA UKLADANI RETURN VALUE FCI
    int type;
    struct retValItem* ptrNext;
} retValItem;

typedef struct retValList {
    retValItem* first;
    retValItem* head;
} RVList;

//***************************  
//      Tohle je moje
//***************************

void setBookmark(tCache* cache);

void initCache(tCache* cache);
int loadTokens(tCache* cache);                     // Funkce pro načtení všech možných tokenů z lexikální analýzy a uložení do paměti
cacheItem* getHead(tCache* cache);
void setHead(tCache* cache, cacheItem* newHead);
void resetHead(tCache* cache);
void destroyCache(tCache* cache);

void RVinitRV(RVList *L);                     //fc na ukládání return value typů pro semantickou analýzu
int RVaddParamType(RVList *L, int type);
void RVResetHead(RVList *L);
int RVReadHead(RVList *L);
void RVdestroyRVL(RVList *L);

int getProlog(tCache* cache);
int loadFuncDecl(tCache* cache, symTable *table);
int func_def(tCache *cache, symTable *T, gen* g, gen* b, gen *rv);
int parametry(tCache *cache, symTable *T, gen* g);
int vstup(tCache *cache, symTable *T, gen* g);
int vstupNext(tCache *cache, symTable *T, gen* g);
int typstart(tCache *cache);
int typ(tCache *cache);
int prikaz(tCache *cache, symTable *T, gen* g, gen* b, gen *rv);
int operation(tCache *cache, symTable *T, gen* g, gen* b, gen *rv);
int single_right(tCache *cache, int expStop, symTable *T, int* type, gen* g,gen* b, gen *rv);
int arg(tCache* cache, symTable *T, gen* g, gen* b, gen *rv);
int argNext(tCache* cache, symTable *T, gen* g, gen* b, gen *rv);
int right_value(tCache* cache, symTable *T, gen* g, gen* b, gen *rv);
int right_next(tCache* cache, symTable *T, gen* g, gen* b, gen *rv);
int IDNext(tCache* cache, symTable *T, gen* g, gen* b, gen *rv);
int f_set(tCache *cache, symTable *T, gen* g, gen* b, gen *rv);
int f_def(tCache *cache, symTable *T, gen* g, gen* b, gen *rv);
int ret_vals(tCache *cache, symTable *T, gen *rv, gen* g);
int ret_next(tCache *cache, symTable *T, gen *rv, gen* g);
int expterm(tCache *cache);
int term(tCache *cache, symTable *T);
int expression(tCache* cache, symTable *T, int* semType, gen* g, int* resultID);

int precTable(int top, int current);

int appendToken(tCache* cache, token* tok);
token* readToken(tCache* cache, int flags);                                  // Vrátí ptr na následující token z paměti
void throwToken(tCache* cache);

int vStackCtr(vStack* v);
int aStackCtr(aStack* a);

void precInit(precList *item);
int precAdd(precList *item, int type, int dataType, int itemType, void* insert);
void precDelete(precList *item);
void precChange(precList *item);
int precCurTer(precList *item);
void precReset(precList *item);
void precDestroy(precList *destroy);
void precDeletePrev(precList* item);

int precPrint(int op, precItem* a, precItem* b, int* returnID, int* retType, gen* g);
int precAnalysis(tCache* cache, cacheItem* headBack, precList* item, gen* g, int* returnID, int* retType);

#endif