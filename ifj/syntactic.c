/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file syntactic.c
 * @brief Syntaktický analyzátor překladače jazyka IFJ20
 * @author Jakub Hlava (xhlava52)
 * @author Thi Bao Ngoc Vu (xvuthi00)
 * @author Lukáš Kraus (xkraus13)
 * @author Jan Pryč (xprycj00)
 * @date 10/2020
 */

#include "syntactic.h"
#include "codegen.h"



/**
 * @brief Inicializace paměti na tokeny
 * Při volání této funkce na inicializovanou strukturu dochází k memory leaku
 * @param   cache           pointer na neinicializovanou strukturu tCache
 */
void initCache(tCache* cache) {
    cache->first = NULL;
    cache->head = NULL;
    cache->bookmark = NULL;
}

/**
 * @brief Funkce pro načtení všech tokenů, které lze ze zdrojového kódu vytvořit.
 * Funkce načte tokeny pomocí funkce getToken a vytvoří z nich vázaný seznam pro další zpracování
 * @param   cache               pointer na inicializovanopu strukturu tCache
 * @return  LEX_FINISHED        při korektním načtení všech tokenů
 * @return  LEX_ERR_INTERNAL    při interní chybě (malloc)
 * @return  LEX_ERR_ANALYSIS    při lexikální chybě
 */
int loadTokens(tCache* cache) {
    int status = LEX_SUCCESS;
    int line = 0;
    char buffer = 0;
    while (status == LEX_SUCCESS) {
        token* t = malloc(sizeof(token));
        if( t == NULL){
            return SYN_RET_INTERNAL;
        }
        status = getToken(t, &buffer);
        t->line = line;
        if (t->tokenType == TYPE_EOL) {
            line++;
        }
        appendToken(cache, t);
    }
    if (status == LEX_ERR_ANALYSIS) {
        fprintf(stderr, "Nastala lexikální chyba! (%u)\n",status);
    } else if (status == LEX_ERR_INTERNAL) {
        fprintf(stderr, "Nastala interní chyba! (%u)\n",status); 
    }
    return status;
}

/**
 * @brief Pomocná funkce pro přidání nového tokenu na konec paměti
 * @param   cache       pointer na paměť tokenů
 * @param   tok         pointer na ukládaný token
 */
int appendToken(tCache* cache, token* tok) {
    cacheItem* ci = malloc(sizeof(cacheItem));
    if( ci == NULL){
            return SYN_RET_INTERNAL;
        }
    ci->tok = tok;
    ci->ptrNext = NULL;
    if (cache->first == NULL) {  
        cache->first = ci;
    } else {
        cache->head->ptrNext = ci;
    }
    cache->head = ci;
    return SYN_RET_SUCCESS;
}

/**
 * @brief Funkce pro posun čtecí hlavy na začátek
 * @param   cache       pointer na paměť tokenů
 */
void resetHead(tCache* cache) {
    cache->head = cache->first;
}

/**
 * Nastaví "záložku" v paměti tokenů
 */
void setBookmark(tCache* cache) {
    cache->bookmark = cache->head;
}

/**
 * Vrátí se v paměti na "záložku", pokud existuje a případně ji smaže. Pokud neexistuje, vrací NO_BOOKMARK
 */
int jumpBookmark(tCache* cache, bool clear) {
    if (cache->bookmark == NULL) {
        return NO_BOOKMARK;
    }
    cache->head = cache->bookmark;
    if (clear) {
        cache->bookmark = NULL;
    }
    return SYN_RET_SUCCESS;
}

/**
 * Uloží do čtecí hlavy pointer na item v parametru newHead
 * Potenciálně nebezpečné!
 */
void setHead(tCache* cache, cacheItem* newHead) {
    fprintf(stderr, "returned token cache head to token type %d\n",newHead->tok->tokenType);
    if (newHead != NULL && cache != NULL) {
        cache->head = newHead;
    }
}

/**
 * Vrací pointer na aktuální item, na který ukazuje čtecí hlava
 */
cacheItem* getHead(tCache* cache) {
    if (cache != NULL) {
        return cache->head;
    } else {
        return NULL;
    }
}

/**
 * @brief Funkce pro přečtení jednoho tokenu z paměti
 * @param   cache       pointer na paměť tokenů
 * @param   move        NEXT nebo STAY pro posun resp. neposouvání hlavy na další token
 * @return pointer na přečtený token nebo NULL, pokud je čtecí hlava za koncem paměti
 */
token* readToken(tCache* cache, int flags) {
    token* ret = NULL;
    if (cache->head != NULL) {
        if (flags & EOL_IGNORE) {
            while (cache->head->tok->tokenType == TYPE_EOL && cache->head != NULL) {
                fprintf(stderr, "EOL thrown away\n");
                cache->head = cache->head->ptrNext;
            }
        }
        ret = cache->head->tok;
        if (!(flags & NOPRINT)) 
            fprintf(stderr, "ln: %3d | mv: %d | returned token: %-20s (RET: %d)", ret->line, flags & NEXT, DEBUG_tokchar(ret), ret->tokenType);
        if (flags & NEXT) {
            cache->head = cache->head->ptrNext;
        } 
        if (cache->head != NULL) {
            if (!(flags & NOPRINT))
                fprintf(stderr, " | HEAD: %d\n", cache->head->tok->tokenType);
        } else {
            if (!(flags & NOPRINT))
                fprintf(stderr, "\n");
        }
    }
    return ret;
}

/**
 * posune hlavu dopředu
 */
void throwToken(tCache* cache) {
    if (cache->head != NULL) {
        fprintf(stderr, "ln: %3d | mv: 1 | thrown token: %-22s (RET: %d)\n", cache->head->tok->line, DEBUG_tokchar(cache->head->tok), cache->head->tok->tokenType);
        cache->head = cache->head->ptrNext;
    }
}

/**
 * @brief Pomocná funkce pro vymazání všech tokenů z paměti tokenů a korektní uvolnění alokací
 * @param   cache       pointer na paměť tokenů
 */
void destroyCache(tCache* cache) {
    cacheItem* worker = cache->first;
    while(worker != NULL) {
        if (worker->tok->tokenType == TYPE_STRING || worker->tok->tokenType == TYPE_IDENTIFIER) {
            free(worker->tok->attr.strAttr);
        }
        free(worker->tok);
        cache->first = cache->first->ptrNext;
        free(worker);
        worker = cache->first;
    }
}

/**
 * @brief Inicializace paměti na return values funkcí
 * Při volání této funkce na inicializovanou strukturu dochází k memory leaku
 * @param   L           pointer na neinicializovanou strukturu RVList
 */
void RVinit(RVList *L){
    L->first = NULL;
    L->head = NULL;
}

/**
 * @brief Pomocná funkce pro přidání nové return value na konec paměti
 * @param   L      pointer na paměť return value
 * @param   type   datový typ ukládané return value
 * @return  SYN_RET_INTERNAL    nezdařený malloc
 * @return  SYN_RET_SUCCESS     úspěch
 */
int RVaddParamType(RVList *L, int type){
    retValItem* newItem = malloc(sizeof(retValItem));
    if(newItem == NULL){
        return SEM_ERR_INTERNAL;
    }
    newItem->type = type;
    newItem->ptrNext = NULL;
    if (L->first == NULL) {  
        L->first = newItem; 
    } else {
        L->head->ptrNext = newItem;
    }
    L->head = newItem;
    return SEM_RET_SUCCESS;
}

/**
 * @brief Funkce pro posun čtecí hlavy na začátek
 * @param   L       pointer na paměť return value
 */
void RVResetHead(RVList *L) {
    L->head = L->first;
}

/**
 * @brief Funkce vrací hodnotu z hlavy a posunuje hlavu na další položku seznamu return values
 * @param   L       pointer na paměť return value
 * @return          typ return value
 */
int RVReadHead(RVList *L){
    retValItem* ret = L->head;
    L->head = L->head->ptrNext;
    return ret->type;
}

/**
 * @brief Pomocná funkce pro vymazání všech uložených return value z paměti
 * @param   L       pointer na paměť typů return value
 */
void RVdestroyRVL(RVList *L) {
    retValItem* worker = L->first;
    while(worker != NULL) {
        worker = worker->ptrNext;
        free(L->first);
        L->first = worker;
    }
    L->head = NULL;
}

void precInit(precList *item){
    item->first = NULL;
    item->head = NULL;
    item->saveHead = NULL;
    item->terPosition = NULL;
    item->bracCntr = 0;
    item->resType = 0;
}

int precAdd(precList *item, int type, int dataType, int itemType, void* insert){
    precItem* newItem = malloc(sizeof(precItem));
    tCache* cache;
    token* tok;
    if (itemType == ARG_TOKEN) {
        cache = (tCache*)insert;
        tok = readToken(cache, STAY);
    }
    if(!newItem){
        return SEM_ERR_INTERNAL;
    }
    newItem->ptrNext = NULL;
    newItem->ptrPrev = NULL;
    newItem->item = NULL;
    newItem->itemType = itemType; // ARG_TOKEN nebo ARG_ID
    newItem->type = type;
    newItem->dataType = dataType;
    if (itemType == ARG_TOKEN) {
        if(type >= TYPE_IDENTIFIER && type <= TYPE_STRING){
            newItem->item = (void*)tok;
        }
    } else if (itemType == ARG_ID) {
        newItem->item = (void*)insert;
    }
    if(type == TYPE_PREC_START_BRACKET){
        if(item->terPosition->ptrNext == NULL){
            item->head->ptrNext = newItem;
            newItem->ptrPrev = item->head;
            item->head = newItem;
        } else {
            item->terPosition->ptrNext = newItem;
            newItem->ptrPrev = item->terPosition;

            item->head->ptrPrev = newItem;
            newItem->ptrNext = item->head;
        }
    } else {
        if(!(item->first)){
            item->first = newItem;
        } else {
            item->head->ptrNext = newItem;
            newItem->ptrPrev = item->head;
        }
        item->head = newItem;
    }
    
    return SEM_RET_SUCCESS;
}

void precDelete(precList *item){
    precItem* delete = item->head->ptrPrev;
    free(item->head);
    delete->ptrNext = NULL;
    item->head = delete;
}

void precDeletePrev(precList* item) {
    precItem* prev = item->head->ptrPrev->ptrPrev;
    free(item->head->ptrPrev);
    item->head->ptrPrev = prev;
    prev->ptrNext = item->head;
}

void precChange(precList* item) {
    precItem* prev = item->head->ptrPrev->ptrPrev;
    free(item->head->ptrPrev);
    item->head->ptrPrev = prev;
    prev->ptrNext = item->head;
    item->head->type = TYPE_PREC_TER;
}

int precCurTer(precList *item){ 
    precItem* back = item->head;
    if((back->type >= TYPE_OP_ADD && back->type <= TYPE_OP_NOT_EQ) || back->type == TYPE_PREC_DOLLAR || (back->type >= TYPE_IDENTIFIER && back->type <= TYPE_STRING) || back->type == TYPE_START_BRACKET || back->type == TYPE_END_BRACKET){
        item->terPosition = item->head;
        return item->head->type;
    } else {
        item->head = back->ptrPrev;
        return precCurTer(item);
    }
}

void precReset(precList *item) {
    item->head = item->first;
}

void precDestroy(precList *destroy) {
    precItem* item = destroy->first;
    while(item) {
        item = item->ptrNext;
        free(destroy->first);
        destroy->first = item;
    }
    destroy->head = NULL;
}

/**
 * @brief Funkce pro detekci prologu
 * Funkce hledá prolog a nechá čtecí hlavu paměti tokenů na posledním tokenu prologu
 * @param   cache       pointer na paměť tokenů
 * @return  SYN_RET_SYNTAX pokud prolog neodpovídá specifikaci nebo neexistuje
 * @return  SYN_RET_SUCCESS při nalezení prologu na správném místě
 */
int getProlog(tCache* cache) {
    token* tok = readToken(cache, NEXT);
    while (tok != NULL && tok->tokenType == TYPE_EOL) { // vyhledávání "package" mezi povolenými EOLy
        tok = readToken(cache, NEXT);
    }
    if (tok != NULL) {
        if (tok->attr.kwAttr == KW_PACKAGE) {
            tok = readToken(cache, NEXT);
            while (tok != NULL && tok->tokenType == TYPE_EOL) { // vyhledávání "main" mezi povolenými EOLy
                tok = readToken(cache, NEXT);
            }
            if (tok != NULL) {
                if (tok->tokenType != TYPE_IDENTIFIER) {
                    return SYN_RET_SYNTAX;
                }
                if (strcmp("main", tok->attr.strAttr) != 0) {
                    return SYN_RET_SEMANTIC_OTHER;
                } else {
                    return SYN_RET_SUCCESS;
                }
            } else {
                return SYN_RET_SYNTAX;
            }
        }
    } else {
        return SYN_RET_SYNTAX;
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Načte jednu deklaraci funkce do tabulky symbolů
 * U načítané funkce kontrolu syntaxi hlavičky a ověřuje redefinici
 * @param   cache   Paměť tokenů
 * @param   table   Tabulka symbolů
 * @return  SYN_RET_SUCCESS         úspěšné načtení funkce
 * @return  SYN_RET_EOF             vyčerpání všech tokenů z paměti
 * @return  SYN_RET_INTERNAL        vnitřní chyba (malloc)
 * @return  SYN_RET_SYNTAX          syntaktická chyba
 * @return  SYN_RET_REDEF           sémantická chyba redefinice
 */
int loadFuncDecl(tCache* cache, symTable* table) {
    token* tok = readToken(cache, NEXT | NOPRINT);
    token* prev = tok;
    int state = SYN_F_START;
    int nextState = SYN_F_START;
    int status;
    char* identifier;
    typeArr inArgs;
    typeArr outArgs;
    initTA(&inArgs);
    initTA(&outArgs);
    int retVal = SYN_RET_SUCCESS;
    sResult s;
    while (state != SYN_F_EXIT && tok != NULL) {
        switch(state) {
            case SYN_F_START:
                if (tok->tokenType == TYPE_KEYWORD && tok->attr.kwAttr == KW_FUNC) {
                    nextState = SYN_F_FUNC;
                } else if (tok->tokenType == TYPE_IDENTIFIER && strcmp(tok->attr.strAttr, "main") == 0) {
                    if (prev->tokenType != TYPE_KEYWORD && prev->attr.kwAttr != KW_PACKAGE) {
                        nextState = SYN_F_EXIT;
                        retVal = SYN_RET_SYNTAX;
                    }
                } else {
                    nextState = SYN_F_START;
                }
            break;
            case SYN_F_FUNC:
                if (tok->tokenType == TYPE_IDENTIFIER) {
                    identifier = tok->attr.strAttr;
                    nextState = SYN_F_FUNCID;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_FUNCID:
                if (tok->tokenType == TYPE_START_BRACKET) {
                    nextState = SYN_F_PARAMLEFTB;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_PARAMLEFTB:
                if (tok->tokenType == TYPE_END_BRACKET) {
                    nextState = SYN_F_PARAMRIGHTB;
                } else if (tok->tokenType == TYPE_IDENTIFIER) {
                    nextState = SYN_F_PARAMID;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_PARAMID: 
                if (tok->tokenType == TYPE_KEYWORD) {
                    switch (tok->attr.kwAttr) {
                        case KW_INT:
                            status = appendTA(&inArgs, VAR_INT);
                            break;
                        case KW_FLOAT:
                            status = appendTA(&inArgs, VAR_FLOAT64);
                            break;
                        case KW_STR:
                            status = appendTA(&inArgs, VAR_STRING);
                            break;
                        default:
                            nextState = SYN_F_EXIT;
                            retVal = SYN_RET_SYNTAX;
                    }
                    if (retVal != SYN_RET_SYNTAX) {
                        nextState = SYN_F_PARAMTYPE;
                    }
                    if (status == EXPSTR_ERR_REALLOC) {
                        nextState = SYN_F_EXIT;
                        retVal = SYN_RET_INTERNAL;
                    }
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_PARAMTYPE: 
                if (tok->tokenType == TYPE_SEPARATOR) {
                    nextState = SYN_F_PARAMDIV;
                } else if (tok->tokenType == TYPE_END_BRACKET) {
                    nextState = SYN_F_PARAMRIGHTB;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_PARAMDIV:
                if (tok->tokenType == TYPE_IDENTIFIER) {
                    nextState = SYN_F_PARAMID;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_PARAMRIGHTB:
                if (tok->tokenType == TYPE_START_SECTION) {
                    nextState = SYN_F_FINISH; 
                } else if (tok->tokenType == TYPE_START_BRACKET){
                    nextState = SYN_F_RETLEFTB;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_RETLEFTB:
                if (tok->tokenType == TYPE_KEYWORD) {
                    switch (tok->attr.kwAttr) {
                        case KW_INT:
                            status = appendTA(&outArgs, VAR_INT);
                            break;
                        case KW_FLOAT:
                            status = appendTA(&outArgs, VAR_FLOAT64);
                            break;
                        case KW_STR:
                            status = appendTA(&outArgs, VAR_STRING);
                            break;
                        default:
                            nextState = SYN_F_EXIT;
                            retVal = SYN_RET_SYNTAX;
                    }
                    if (retVal != SYN_RET_SYNTAX) {
                        nextState = SYN_F_RETTYPE;
                    }
                    if (status == EXPSTR_ERR_REALLOC) {
                        nextState = SYN_F_EXIT;
                        retVal = SYN_RET_INTERNAL;
                    }
                } else if (tok->tokenType == TYPE_END_BRACKET) {
                    nextState = SYN_F_RETRIGHTB;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_RETTYPE:
                if (tok->tokenType == TYPE_SEPARATOR) {
                    nextState = SYN_F_RETDIV;
                } else if (tok->tokenType == TYPE_END_BRACKET) {
                    nextState = SYN_F_RETRIGHTB;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_RETDIV:
                if (tok->tokenType == TYPE_KEYWORD) {
                    switch (tok->attr.kwAttr) {
                        case KW_INT:
                            status = appendTA(&outArgs, VAR_INT);
                            break;
                        case KW_FLOAT:
                            status = appendTA(&outArgs, VAR_FLOAT64);
                            break;
                        case KW_STR:
                            status = appendTA(&outArgs, VAR_STRING);
                            break;
                        default:
                            nextState = SYN_F_EXIT;
                            retVal = SYN_RET_SYNTAX;
                    }
                    if (retVal != SYN_RET_SYNTAX) {
                        nextState = SYN_F_RETTYPE;
                    }
                    if (status == EXPSTR_ERR_REALLOC) {
                        nextState = SYN_F_EXIT;
                        retVal = SYN_RET_INTERNAL;
                    }
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_RETRIGHTB:
                if (tok->tokenType == TYPE_START_SECTION) {
                    nextState = SYN_F_FINISH;
                } else {
                    nextState = SYN_F_EXIT;
                    retVal = SYN_RET_SYNTAX;
                }
            break;
            case SYN_F_FINISH:
                search(table, identifier, &s); 
                if (s.resultType != TYPE_NONE) {
                    destroyTA(&inArgs);
                    destroyTA(&outArgs);
                    retVal = SYN_RET_REDEF;
                } else {
                    addFunc(table, identifier, inArgs.len, outArgs.len, inArgs.array, outArgs.array);
                }
                nextState = SYN_F_EXIT;
            break;
        } // END SWITCH
        state = nextState;
        prev = tok;
        tok = readToken(cache, NEXT | NOPRINT);   
    } // END WHILE
    if (tok == NULL) {
        return SYN_RET_EOF;
    } else {
        return retVal;
    }
}

/**
 * @brief Funkce pro definování funkce
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param T pointer na strukturu tabulky symbolů
 * @param L pointer na funkci pro určení typu proměnné 
 */
int func_def(tCache *cache, symTable *T, gen* g, gen* b, gen *rv){

    token* tok = readToken(cache, NEXT | EOL_IGNORE);
    char* fName;
    if(tok == NULL || tok->tokenType == TYPE_EOF)      // ověření, zda už je konec
    {
        return SYN_RET_SUCCESS;
    }
    if(tok != NULL && tok->attr.kwAttr == KW_FUNC)  // načtení "func"
    {   
        setBookmark(cache);
        tok = readToken(cache, NEXT);
        if(tok->tokenType == TYPE_IDENTIFIER) // načtení identifikátoru
        {   //----------------------semantika - start----------------------
            fName = tok->attr.strAttr;
            int call = startCall(T, fName); // vytvorime polozku do callstacku v symtable
            if(call == STATUS_INT_ERR) // pokud ne nepovede malloc
            {
                return STATUS_INT_ERR;
            }
            if (call == STATUS_PROG_ERR) //pokud volame nedefinovanou fci
            {
                return STATUS_PROG_ERR;
            }            
            
            sResult s;
            search(T, fName, &s);
            //kontrola, zda byla fce deklarovana
            if(s.resultType != TYPE_FUNC){
                return SEM_ERR_DEFINES;
            }
            gen_func_header(fName, s.data.func);
                int kontrola = startScope(T);
                if(kontrola == STATUS_PROG_ERR){
                    return STATUS_PROG_ERR;
                } else if(kontrola == STATUS_INT_ERR){
                    return STATUS_INT_ERR;
                }
            //----------------------semantika - end----------------------
            kontrola = parametry(cache, T, g);    // rozbor neterminálu parametry 
            if(kontrola == SYN_RET_SUCCESS)
            {
                tok = readToken(cache, NEXT);
                if(tok->tokenType == TYPE_EOL) // vyhledání EOL
                { 
                    kontrola = prikaz(cache, T, g, b, rv);            
                    if(kontrola == SYN_RET_SUCCESS)
                    {
                        //----------------------semantika - start----------------------
                            kontrola = endScope(T);
                            if(kontrola == STATUS_INT_ERR){
                                return STATUS_INT_ERR;
                            }
                            int end = endCall(T);
                            if (strcmp(fName, "main") == 0) {
                                gen_return(true);
                            } else {
                                gen_return(false);
                            }
                            
                            if(end == STATUS_INT_ERR){
                                return STATUS_INT_ERR;
                            }
                        //----------------------semantika - end----------------------
                        return func_def(cache, T, g, b, rv);               
                    }
                    if (kontrola == SEM_ERR_SEMANTIC){
                        return SEM_ERR_SEMANTIC;
                    }if (kontrola == SEM_ERR_DEFINES){
                        return SEM_ERR_DEFINES;
                    }
                }
            }
            return kontrola;
        }
    }
    tok = readToken(cache, STAY);
    if (tok != NULL) {
        fprintf(stderr, "ERROR: error on line (prolog+%d)\n",tok->line);
        fprintf(stderr, "Unexpected %s\n",DEBUG_tokchar(tok));
    }
    return SYN_RET_SYNTAX;   
}

/**
 * @brief Funkce pro vstupní parametry funkce
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param T pointer na strukturu tabulky symbolů
 * @param L pointer na funkci pro určení typu proměnné 
 */
int parametry(tCache *cache, symTable *T, gen* g){
    token* tok = readToken(cache, NEXT);
    if(tok->tokenType == TYPE_START_BRACKET){ // ověření povinné závorky na začátku
        int kontrola = vstup(cache, T, g);       // rozbor neterminálu vstup a ověření ukončující závorky                                  
        if(kontrola == SYN_RET_SUCCESS){
            return typstart(cache);                 
        } else {
            return kontrola;
        }
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro vstupní parametr funkce
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param T pointer na strukturu tabulky symbolů
 */
int vstup(tCache *cache, symTable *T, gen* g){
    setBookmark(cache);//ID
    token* tok = readToken(cache, NEXT);
    if(tok->tokenType == TYPE_IDENTIFIER){ // identifikátor
        tok = readToken(cache, NEXT); // precte se pripadny datovy typ a posuneme hlavu na dalsi
        if(tok->attr.kwAttr == KW_INT || tok->attr.kwAttr == KW_FLOAT || tok->attr.kwAttr == KW_STR){
            //----------------------semantika - start----------------------
                int addVarCheck;
                if(tok->attr.kwAttr == KW_INT){  
                    addVarCheck = addVar(T, cache->bookmark->tok->attr.strAttr, TYPE_INT, g);//do scopu vlozime promennou
                }
                if(tok->attr.kwAttr == KW_FLOAT){  
                    addVarCheck = addVar(T, cache->bookmark->tok->attr.strAttr, TYPE_FLOAT64, g);//do scopu vlozime promennou
                }
                if(tok->attr.kwAttr == KW_STR){  
                    addVarCheck = addVar(T, cache->bookmark->tok->attr.strAttr, TYPE_STRING, g);//do scopu vlozime promennou
                }
                if(addVarCheck == STATUS_INT_ERR){
                    return addVarCheck;
                }
                if(addVarCheck == STATUS_PROG_ERR){
                    return addVarCheck;
                }
                //----------------------semantika - end----------------------
            tok = readToken(cache, STAY);
            if(tok->tokenType == TYPE_END_BRACKET){
                throwToken(cache);//jen posune hlavu
                return SYN_RET_SUCCESS;
            } else {
                return vstupNext(cache, T, g);
            }
        }    
    } else if (tok->tokenType == TYPE_END_BRACKET) { // přepis na eps
        return SYN_RET_SUCCESS;
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro vstupní parametry funkce
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param T pointer na strukturu tabulky symbolů
 */
int vstupNext(tCache *cache, symTable *T, gen* g){
    token* tok = readToken(cache, STAY);
    while(tok->tokenType != TYPE_END_BRACKET){ // rozklad na eps
        tok = readToken(cache, NEXT);
        if(tok->tokenType == TYPE_SEPARATOR){ // rozklad na , ID type <vstupNext>
            setBookmark(cache);
            tok = readToken(cache, NEXT);
            if(tok->tokenType == TYPE_IDENTIFIER){
                tok = readToken(cache, NEXT);
                if(tok->attr.kwAttr == KW_INT || tok->attr.kwAttr == KW_FLOAT || tok->attr.kwAttr == KW_STR){
                //----------------------semantika - start----------------------
                    int addVarCheck;
                    if(tok->attr.kwAttr == KW_INT){  
                    addVarCheck = addVar(T, cache->bookmark->tok->attr.strAttr, VAR_INT, g);//do scopu vlozime promennou
                    }
                    if(tok->attr.kwAttr == KW_FLOAT){  
                    addVarCheck = addVar(T, cache->bookmark->tok->attr.strAttr, VAR_FLOAT64, g);//do scopu vlozime promennou
                    }
                    if(tok->attr.kwAttr == KW_STR){  
                    addVarCheck = addVar(T, cache->bookmark->tok->attr.strAttr, VAR_STRING, g);//do scopu vlozime promennou
                    }
                    if(addVarCheck == STATUS_INT_ERR){
                        return STATUS_INT_ERR;
                    }
                    if(addVarCheck == STATUS_PROG_ERR){
                        return STATUS_PROG_ERR;
                    }
                //----------------------semantika - end----------------------
                    tok = readToken(cache, STAY);
                    continue; 
                }                
            }
        }
        return SYN_RET_SYNTAX;
    }
    throwToken(cache); // zahození END_BRACKET z bufferu
    return SYN_RET_SUCCESS;
}

/**
 * @brief funkce pro výstupní definování proměnných v hlaviččce funkce se ()
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param L pointer na funkci pro určení typu proměnné 
 */
int typstart(tCache *cache){
    token* tok = readToken(cache, NEXT);
    if(tok->tokenType == TYPE_START_BRACKET){ // přepis na ( type <typ> )
        tok = readToken(cache, NEXT);
        if (tok->tokenType == TYPE_KEYWORD) {
            if(tok->attr.kwAttr == KW_INT || tok->attr.kwAttr == KW_FLOAT || tok->attr.kwAttr == KW_STR){
                return typ(cache);                                                          
            }
        } else if (tok->tokenType == TYPE_END_BRACKET) { // prázdné závorky
            tok = readToken(cache, NEXT);
            if (tok->tokenType == TYPE_START_SECTION){ 
                return SYN_RET_SUCCESS;
            } 
        }      
    } else if (tok->tokenType == TYPE_START_SECTION){ // epsilon
        return SYN_RET_SUCCESS;
    } 
    return SYN_RET_SYNTAX;
}
/**
 * @brief Funkce pro výstupní určení typu proměnné v hlavičce funkce 
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param L pointer na funkci pro určení typu proměnné 
 */
int typ(tCache *cache){
    token* tok = readToken(cache, STAY);
    bool empty = true;
    while(tok->tokenType != TYPE_END_BRACKET){
        empty = false;
        tok = readToken(cache, NEXT);
        if(tok->tokenType == TYPE_SEPARATOR){
            tok = readToken(cache, NEXT);
            if (tok->tokenType == TYPE_KEYWORD) {
                if(tok->attr.kwAttr == KW_INT || tok->attr.kwAttr == KW_FLOAT || tok->attr.kwAttr == KW_STR){ 
                    continue;
                }  
            }             
        } else if (tok->tokenType == TYPE_END_BRACKET) {
            break;
        }
        return SYN_RET_SYNTAX;
    }
    if (tok->tokenType == TYPE_END_BRACKET) {
        if (empty) {
            throwToken(cache);
        }
        tok = readToken(cache, NEXT);
        if(tok->tokenType == TYPE_START_SECTION){
            return SYN_RET_SUCCESS;
        }
    }
    return SYN_RET_SYNTAX;
}
/**
 * @brief Funkce pro volání příkazů
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param T ukazatel na strukturu tabulky symbolů
 */
int prikaz(tCache *cache, symTable *T, gen* g, gen* b, gen *rv){
    /////////////////////////////////////////////////////////////////////
    // pozor křehké
    if (cache->head && cache->head->tok->tokenType == TYPE_IDENTIFIER) setBookmark(cache);
    token* tok = readToken(cache, STAY | EOL_IGNORE);
    if (cache->head && cache->head->tok->tokenType == TYPE_IDENTIFIER) setBookmark(cache);
    throwToken(cache);
    /////////////////////
    int kontrola;
    if(tok->tokenType == TYPE_END_SECTION){     //eps - 17. pravidlo
        return SYN_RET_SUCCESS;
    } else if(tok->tokenType == TYPE_IDENTIFIER){          //ID - 13.,20. pravidlo
        return operation(cache, T, g, b, rv);
    } else if(tok->attr.kwAttr == KW_IF){           //if - 16. pravidlo
        int type;
        kontrola = expression(cache, T, &type, g, NULL);
        if (kontrola != SYN_RET_SUCCESS) {
            return kontrola;
        }
        if (type != VAR_BOOL) {
            return SEM_ERR_TYPE;
        }
        tok = readToken(cache, NEXT);
        if (tok->tokenType == TYPE_START_SECTION) {
            tok = readToken(cache, NEXT);
            if(tok->tokenType == TYPE_EOL)
            {
                //----------------------semantika - start----------------------
                    int kontrola = startScope(T);
                    if(kontrola == STATUS_PROG_ERR){
                        return STATUS_PROG_ERR;
                    } else if(kontrola == STATUS_INT_ERR){
                        return STATUS_INT_ERR;
                    }
                //----------------------semantika - end----------------------
                kontrola = prikaz(cache, T, g, b, rv);
                if(kontrola == SYN_RET_SUCCESS)
                {
                        //----------------------semantika - start----------------------
                            kontrola = endScope(T);
                            if(kontrola == STATUS_INT_ERR){
                                return STATUS_INT_ERR;
                            }
                        //----------------------semantika - end----------------------
                        tok = readToken(cache ,NEXT);
                        if(tok->attr.kwAttr == KW_ELSE)
                        {
                            tok = readToken(cache, NEXT);
                            if(tok->tokenType == TYPE_START_SECTION)
                            {
                                tok = readToken(cache, NEXT);
                                if (tok->tokenType == TYPE_EOL)
                                {
                                    //----------------------semantika - start----------------------
                                        int kontrola = startScope(T);
                                        if(kontrola == STATUS_PROG_ERR){
                                            return STATUS_PROG_ERR;
                                        } else if(kontrola == STATUS_INT_ERR){
                                            return STATUS_INT_ERR;
                                        }
                                    //----------------------semantika - end----------------------
                                    kontrola = prikaz(cache, T, g, b, rv);
                                    if(kontrola == SYN_RET_SUCCESS)
                                    {
                                        //----------------------semantika - start----------------------
                                            kontrola = endScope(T);
                                            if(kontrola == STATUS_INT_ERR){
                                                return STATUS_INT_ERR;
                                            }
                                        //----------------------semantika - end----------------------
                                        tok = readToken(cache, NEXT);
                                        if(tok->tokenType == TYPE_EOL)
                                        {
                                            return prikaz(cache, T, g, b, rv);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        return kontrola;
    } else if(tok->attr.kwAttr == KW_FOR){          //for - 18. pravidlo
        //----------------------semantika - start----------------------
            int kontrola = startScope(T);
            if(kontrola == STATUS_PROG_ERR){
                return STATUS_PROG_ERR;
            } else if(kontrola == STATUS_INT_ERR){
                return STATUS_INT_ERR;
            }
        //----------------------semantika - end----------------------
        kontrola = f_def(cache, T, g, b, rv);       //po prvni strednik
        //----------------------semantika - start----------------------
            kontrola = startScope(T);
            if(kontrola == STATUS_PROG_ERR){
                return STATUS_PROG_ERR;
            } else if(kontrola == STATUS_INT_ERR){
                return STATUS_INT_ERR;
            }
        //----------------------semantika - end----------------------
        if(kontrola != SYN_RET_SYNTAX){
            int fortype;
            kontrola = expression(cache, T, &fortype, g, NULL);//expression
            if (kontrola != SYN_RET_SUCCESS) {
                return kontrola;
            }
            if (fortype != VAR_BOOL) {
                return SEM_ERR_TYPE;
            }
            tok = readToken(cache, NEXT);
            if(tok->tokenType == TYPE_SEMICOLON){
                kontrola = f_set(cache, T, g, b, rv); //posledni ve foru
                if(kontrola == SYN_RET_SUCCESS){
                    tok = readToken(cache, NEXT);
                    if(tok->tokenType == TYPE_EOL){
                        kontrola = prikaz(cache, T, g, b, rv);
                        if(kontrola == SYN_RET_SUCCESS){
                            //----------------------semantika - start----------------------
                                kontrola = endScope(T);
                                if(kontrola == STATUS_INT_ERR){
                                    return STATUS_INT_ERR;
                                }
                                kontrola = endScope(T);
                                if(kontrola == STATUS_INT_ERR){
                                    return STATUS_INT_ERR;
                                }
                            //----------------------semantika - end----------------------
                            tok = readToken(cache, NEXT);
                            if (tok->tokenType == TYPE_EOL) {
                                return prikaz(cache, T, g, b, rv);
                            }
                        }
                    }
                }
                return kontrola;
            }
        }
        return SYN_RET_SYNTAX;
    } else if(tok->attr.kwAttr == KW_RET){ 
        destroyAStack(rv->params);         //return - 19. pravidlo
        kontrola = ret_vals(cache, T, rv, g);

        if(kontrola == SYN_RET_SUCCESS){
            return prikaz(cache, T, g, b, rv);
        }
        return kontrola;
    } else if(tok->attr.kwAttr == TYPE_EOL){
       return prikaz(cache, T, g, b, rv);
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro rozhodnutí jestli do identifikátoru přiřazujeme, nebo deklarujeme a přiřazujeme.
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param T ukazatel na strukturu tabulky symbolů
 */
int operation(tCache *cache, symTable *T, gen* g, gen* b, gen *rv){
    token* tok = readToken(cache, STAY);
    int kontrola;//syntakticka kontrola
    int semancheck;
    if(tok->tokenType == TYPE_OP_DEF){    //:=  - 14. pravidlo
        throwToken(cache); // posun hlavy
        //----------------------semantika - start--------------------
        int dataType = 0;
            destroyVStack(b->v); //smazani pomocneho stacku na pocitani poctu promennych napravo a nalevo
        //----------------------semantika - end----------------------
        kontrola = single_right(cache, EXP_END_EOL | FLAG_DECLARATION, T, &dataType, g, b, rv);
        
        //----------------------semantika - start----------------------
            destroyVStack(b->v); //smazani pomocneho stacku na pocitani poctu promennych napravo a nalevo
        int status = addVar(T, cache->bookmark->tok->attr.strAttr, dataType, g);
        if (status == STATUS_PROG_ERR) {
            return SYN_RET_REDEF;
        }
        sResult r;
        search(T, cache->bookmark->tok->attr.strAttr, &r);
        gen_declaration(r.data.var,g->rightSideTemp);
        //----------------------semantika - end----------------------
        if(kontrola == SYN_RET_SUCCESS){
            return prikaz(cache, T, g, b, rv);
        }
        return kontrola;
    } else if(tok->tokenType == TYPE_START_BRACKET){   //call 20. pravidlo
        //----------------------semantika - start----------------------
            semancheck = isFunc(T, cache->bookmark->tok->attr.strAttr); //kontrola, zda byla fce deklarovana
            if(semancheck == false){
                return SEM_ERR_DEFINES;
            }
        //----------------------semantika - end----------------------
        //-----------------------------------------------------------
        //              BUILTIN FUNKCE
        //----------------------------------------------------------
        if (strcmp(cache->bookmark->tok->attr.strAttr, "print") == 0) {
            cacheItem* back = getHead(cache); // uložení čtecí hlavy
            pQueue ps;
            int err = SYN_RET_SUCCESS;
            initPQueue(&ps);
            throwToken(cache);
            int tokenc = 0;
            while(tok->tokenType != TYPE_END_BRACKET) {
                tok = readToken(cache, NEXT);
                if (tok->tokenType == TYPE_SEPARATOR) {
                    continue;
                } else if (tok->tokenType == TYPE_INT || tok->tokenType == TYPE_FLOAT64 || tok->tokenType == TYPE_STRING || tok->tokenType == TYPE_IDENTIFIER) {
                    printAppend(&ps, tok);
                    tokenc++;
                } else if (tok->tokenType == TYPE_END_BRACKET) {
                    break;
                } else {
                    err = SYN_RET_SYNTAX;
                    break;
                }
            }
            if (tokenc) {
                gen_print(&ps, T);
            }
            destroyPQueue(&ps);
            setHead(cache, back); // návrat do původního stavu
            if (err != SYN_RET_SUCCESS) {
                return err;
            }
        }
        throwToken(cache);
        kontrola = arg(cache, T, g, b, rv);
        if(kontrola == SYN_RET_SUCCESS){
            return prikaz(cache, T, g, b, rv);
        }
        return kontrola;
    } else if(tok->tokenType == TYPE_SEPARATOR){    // = - 15. pravidlo
        
        
        //----------------------semantika - start----------------------
        sResult r;
        if (strcmp(cache->bookmark->tok->attr.strAttr, "_") == 0) {
            vData empty;
            empty.index = 0;
            empty.name = "_";
            empty.type = VAR_UNIVERSAL;
            appendVStack(g->v, &empty);
            appendVStack(b->v, &empty);
        } else {
            search(T, cache->bookmark->tok->attr.strAttr, &r);
            if (r.resultType != TYPE_VAR) {
                destroyVStack(g->v);
                return SEM_ERR_DEFINES;
            }
            appendVStack(g->v, r.data.var);
            appendVStack(b->v, r.data.var);
        }
        //----------------------semantika - end----------------------
        kontrola = IDNext(cache, T, g, b, rv);   	        
        if(kontrola == SYN_RET_SUCCESS){
            kontrola = right_value(cache, T, g, b, rv);
            if(kontrola == SYN_RET_SUCCESS){      
                return prikaz(cache, T, g, b, rv);
            }
        }  
        return kontrola;                       
    } else if (tok->tokenType == TYPE_OP_SET) { // 15. pravidlo IDNext == eps
        throwToken(cache); // zahození =
        //----------------------semantika - start----------------------
            sResult r;
            if (strcmp(cache->bookmark->tok->attr.strAttr, "_") == 0) {
                vData empty;
                empty.index = 0;
                empty.name = "_";
                empty.type = VAR_UNIVERSAL;
                appendVStack(g->v, &empty);
                appendVStack(b->v, &empty);
            } else {
                search(T, cache->bookmark->tok->attr.strAttr, &r);
                if (r.resultType != TYPE_VAR) {
                    destroyVStack(g->v);
                    return SEM_ERR_DEFINES;
                }
                appendVStack(g->v, r.data.var);
                appendVStack(b->v, r.data.var);
            }
        //----------------------semantika - end----------------------
        kontrola = right_value(cache,T, g, b, rv);
        if(kontrola == SYN_RET_SUCCESS){  
            return prikaz(cache, T, g, b, rv);
        }
        return kontrola;
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro volání funkce, výrazu, nebo termu.
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int single_right(tCache *cache, int flags, symTable *T, int* semType, gen* g, gen* b, gen *rv){        
    token* tok = readToken(cache, STAY);
    int kontrola;
    int type;
    int result;
    bool sus = false;
    cacheItem* prev = getHead(cache);
    char * tmp;//do nej se ulozi ID fce
    
    kontrola = expression(cache, T, &type, g, &result);       //kontrola zda přiřazovaná hodnota není expression
    
    if (kontrola != SYN_RET_SUCCESS) {
        if (kontrola == SEM_ERR_DEFINES) {
            sus = true;
        } else {
            return kontrola;
        }
    }
    if (semType != NULL) {
        *semType = type;
    }
    if (flags & FLAG_DECLARATION) { 
        if (prev->ptrNext == getHead(cache)) { // single token pravá strana
            g->rightSideTemp = gen_temp_var(g);
            gen_temp_set_from_tok(g, g->rightSideTemp, prev->tok);
        } else {
            g->rightSideTemp = result;
        }
        
    } else if (flags & FLAG_ASSIGNMENT) {
        
        if (prev->ptrNext == getHead(cache)) { // single token pravá strana
            if(prev->tok->tokenType == TYPE_IDENTIFIER){
                tmp = prev->tok->attr.strAttr;//do nej se ulozi ID fce
                cache->bookmark = prev;
            }
            if (!(prev->tok->tokenType == TYPE_IDENTIFIER && isFunc(T, prev->tok->attr.strAttr))) {
                vData* var = popVStack(g->v);
                if (var != NULL) {
                    if (var->type != VAR_UNIVERSAL) {
                        gen_simple_set(var, prev->tok, T);
                    }
                } else {
                    return SEM_ERR_FUNC;
                }
            } 
        } else {
            vData* var = popVStack(g->v);
            if (var != NULL) {
                if (var->type != VAR_UNIVERSAL) {
                    gen_simple_set(var, prev->tok, T);
                }
            } else {
                return SEM_ERR_FUNC;
            }
            gen_set_from_temp(var, result);
        }
    }
    tok = readToken(cache, STAY);
    if(tok->tokenType == TYPE_START_BRACKET){//když se bude přiřazovat funkce (proto se kontroluje start bracket a poté se volá arg)
        if (flags & FLAG_DECLARATION) {
            return SYN_RET_SYNTAX;
        } else if (flags & FLAG_ASSIGNMENT) { 
            // BUILTIN FUNKCE bez parametru
            
            if (strcmp(prev->tok->attr.strAttr, "inputi") == 0) {
                vData* var1 = popVStack(g->v);
                vData* var2 = popVStack(g->v);
                if (var1 == NULL || var2 == NULL) {
                    return SEM_ERR_FUNC;
                }
                gen_input(TYPE_INT, var2, var1, g);
            } else if (strcmp(prev->tok->attr.strAttr, "inputs") == 0) {
                vData* var1 = popVStack(g->v);
                vData* var2 = popVStack(g->v);
                if (var1 == NULL || var2 == NULL) {
                    return SEM_ERR_FUNC;
                }
                gen_input(TYPE_STRING, var2, var1, g);
            } else if (strcmp(prev->tok->attr.strAttr, "inputf") == 0) {
                vData* var1 = popVStack(g->v);
                vData* var2 = popVStack(g->v);
                if (var1 == NULL || var2 == NULL) {
                    return SEM_ERR_FUNC;
                }
                gen_input(TYPE_FLOAT64, var2, var1, g);
            }
        }
        throwToken(cache);
        destroyAStack(b->params);
        int status = arg(cache, T, g, b, rv);
        //----------------------semantika - start----------------------
            fData* data = getFuncData(T, tmp); //ziskame data dane fce
            int vCtr = vStackCtr(b->v);
            int aCtr = aStackCtr(b->params);
            if(vCtr != data->outArgC){
                destroyVStack(b->v);
                return SEM_ERR_FUNC;
            }
            if (data->inArgC == aCtr){//spravny pocet vstupnich argumentu
                int i = 0;
                aStackItem *itter = b->params->first;
                while(i != data->inArgC){//typy vstupnich argumentu
                    if(itter->type == ARG_VAR){
                        if (itter->data.var->type != data->inArgTypes[i]){
                            return SEM_ERR_FUNC;
                        }
                    } else{
                        if (itter->data.tok->tokenType != data->inArgTypes[i]){
                            return SEM_ERR_FUNC;
                        }
                    }
                    itter = itter->ptrNext;
                    i++;
                }
            } else{
                return SEM_ERR_FUNC;
            }

        //----------------------semantika - end----------------------
        if (status == SYN_RET_SUCCESS) {
            
            // ostatni builtin
            if (strcmp(prev->tok->attr.strAttr, "int2float") == 0) {
                status = gen_int2float(g);
            } else if (strcmp(prev->tok->attr.strAttr, "float2int") == 0) {
                status = gen_float2int(g);
            } else if (strcmp(prev->tok->attr.strAttr, "len") == 0) {
                status = gen_len(g);
            } else if (strcmp(prev->tok->attr.strAttr, "substr") == 0) {
                status = gen_substr(g);
            } else if (strcmp(prev->tok->attr.strAttr, "ord") == 0) {
                status = gen_ord(g);
            } else if (strcmp(prev->tok->attr.strAttr, "chr") == 0) {
                status = gen_chr(g);
            } else {
                // function call
            }
            return status;
        } else {
            return status;
        }
    } else if (sus) {
        return kontrola;
    } else if (tok->tokenType == TYPE_SEPARATOR) { //pokud bude token čárka, tak se vrátí MAYBE_SUCCESS
        return SYN_RET_MAYBE_SUCCESS;    
    } else if(tok->tokenType == TYPE_EOL){ //pokud token bude EOL, vrací se SUCCESS     
        throwToken(cache); 
        return SYN_RET_SUCCESS;
    } else {
        switch(flags & 7/*0b111*/) { // kontrola koncových znaků
            case EXP_END_EOL:           //poslední zna bude EOL
                if (tok->tokenType != TYPE_EOL) {
                    return SYN_RET_SYNTAX;
                }
            break;
            case EXP_END_SEMICOLON:     //poslední znak bude středník - pro funkci f_def
                if (tok->tokenType != TYPE_SEMICOLON) {
                    return SYN_RET_SYNTAX;
                }
            break;
            case EXP_END_SEPARATOR:     //poslední znak bude čárka - pro funkci right_next
                if (tok->tokenType != TYPE_SEPARATOR) {
                    return SYN_RET_SYNTAX;
                }
            break;
            case EXP_END_START_SECTION: //poslední znak bude chlupatá otevírací závorka - pro funkci f_set
                if (tok->tokenType != TYPE_START_SECTION) {
                    return SYN_RET_SYNTAX;
                }
            break;
            case EXP_END_EOL_OR_SEPARATOR:  //poslední znak bude EOL nebo čárka - pro funkci operation
                if (tok->tokenType != TYPE_SEPARATOR && tok->tokenType != TYPE_EOL) {
                    return SYN_RET_SYNTAX;
                }
            break;
        }
        throwToken(cache);
        return SYN_RET_SUCCESS;
    }

    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro přiřazení identifikátoru
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int f_set(tCache *cache, symTable *T, gen* g, gen* b, gen *rv){
    token* tok = readToken(cache, NEXT);
    if(tok->tokenType == TYPE_START_SECTION){           //zjistění že neupravujeme proměnnou ve foru
        return SYN_RET_SUCCESS;
    } else if(tok->tokenType == TYPE_IDENTIFIER){       //zjistění že ji upravujeme
        tok = readToken(cache, NEXT);
        if(tok->tokenType == TYPE_OP_SET){
            //throwToken(cache);
            return single_right(cache, EXP_END_START_SECTION, T, NULL, g, b, rv);
        }
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro deklarování a přiřazení identifikátoru. (volání single_right)
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @param T ukazatel na strukturu tabulky symbolů
 */
int f_def(tCache *cache, symTable *T, gen* g, gen* b, gen *rv){
    //int semancheck;
    setBookmark(cache);//nazem promenne
    token* tok = readToken(cache, NEXT);
    if(tok->tokenType == TYPE_SEMICOLON){           	//zjistění že nedefinujeme proměnnou ve foru
        return SYN_RET_SUCCESS;
    } else if(tok->tokenType == TYPE_IDENTIFIER){       //zjistení že ji definujeme
        tok = readToken(cache, NEXT);
        if(tok->tokenType == TYPE_OP_DEF){
            int type = 0;
            int kontrola = single_right(cache, EXP_END_SEMICOLON | FLAG_DECLARATION, T, &type, g, b, rv);
            if (kontrola != SYN_RET_SUCCESS) {
                return kontrola;
            }
            //----------------------semantika - start----------------------
                destroyVStack(b->v); //smazani pomocneho stacku na pocitani poctu promennych napravo a nalevo
            //----------------------semantika - end----------------------
            addVar(T, cache->bookmark->tok->attr.strAttr, type, g);
            sResult r;
            search(T, cache->bookmark->tok->attr.strAttr, &r);
            gen_declaration(r.data.var,g->rightSideTemp);
        }
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro  zavolání výrazu nebo termu
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int ret_vals(tCache *cache, symTable *T, gen *rv, gen* g){
    token* tok = readToken(cache, STAY);
    int kontrola;
    if(tok->tokenType == TYPE_EOL){ // epsilon
        throwToken(cache);
        return SYN_RET_SUCCESS;
    } else {
        kontrola = expression(cache, T, NULL, g, NULL);
        if (kontrola != SYN_RET_SUCCESS) {
            return kontrola;
        }
        tok = readToken(cache, NEXT); 
        if(tok->tokenType == TYPE_EOL){ // eol
            return SYN_RET_SUCCESS;
        } else if(tok->tokenType == TYPE_SEPARATOR){ // pokracovani
            return ret_next(cache, T, rv, g);  
        }
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro zavolání více výrazů nebo termů
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int ret_next(tCache *cache, symTable *T, gen *rv, gen* g){
    int kontrola;
    token* tok = readToken(cache, STAY);
    if(tok->tokenType == TYPE_EOL){ // epsilon
        throwToken(cache);
        return SYN_RET_SUCCESS;
    } else {
        kontrola = expression(cache, T, NULL, g, NULL);
        if (kontrola != SYN_RET_SUCCESS) {
            return kontrola;
        }
        tok = readToken(cache, NEXT); 
        if(tok->tokenType == TYPE_EOL){ // eps
            return SYN_RET_SUCCESS;
        } else if(tok->tokenType == TYPE_SEPARATOR){ // pokracovani
            return ret_next(cache, T, rv, g);  
        } 
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro zavolání více Identifikátorů
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int IDNext(tCache* cache, symTable *T, gen* g, gen* b, gen *rv)
{
    token* tok = readToken(cache, NEXT);
    setBookmark(cache);//ID
    if(tok->tokenType == TYPE_SEPARATOR)
    {
        tok = readToken(cache, NEXT);//ID
        if(tok->tokenType == TYPE_IDENTIFIER)
        {
            //----------------------semantika - start----------------------
            sResult r;
            if (strcmp(cache->bookmark->tok->attr.strAttr, "_") == 0) {
                vData* empty = malloc(sizeof(vData));
                empty->index = 0;
                empty->name = "_";
                empty->type = VAR_UNIVERSAL;
                appendVStack(g->v, empty);
                appendVStack(b->v, empty);
            } else {
                search(T, cache->bookmark->tok->attr.strAttr, &r);
                if (r.resultType != TYPE_VAR) {
                    destroyVStack(g->v);
                    return SEM_ERR_DEFINES;
                }
                appendVStack(g->v, r.data.var);
                appendVStack(b->v, r.data.var);
            }
            //----------------------semantika - end----------------------
            return IDNext(cache, T, g, b, rv);
        }
        return SYN_RET_SYNTAX;
    }
    else if (tok->tokenType == TYPE_OP_SET) // přepis na eps
    {
        return SYN_RET_SUCCESS;
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro zavolání single_right a right_next
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int right_value(tCache* cache, symTable *T, gen* g, gen* b, gen *rv)
{
    int kontrola = single_right(cache, EXP_END_EOL_OR_SEPARATOR | FLAG_ASSIGNMENT, T, NULL, g, b, rv);
    if(kontrola == SYN_RET_MAYBE_SUCCESS){ // nalezený separator
        return right_next(cache, T, g, b, rv);
    } else {
        //----------------------semantika - start----------------------
            destroyVStack(b->v); //smazani pomocneho stacku na pocitani poctu promennych napravo a nalevo
        //----------------------semantika - end----------------------
        
        return kontrola;
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce která rozlišuje přijatý token.
 * @param cache  pointer na inicializovanopu strukturu tCache
 * @return Vrací INT, STR, FLOAT64, nebo ID.
 */
int term(tCache *cache, symTable *T){
    token* tok = readToken(cache, NEXT);
    if(tok->tokenType == TYPE_INT || tok->tokenType == TYPE_FLOAT64 || tok->tokenType == TYPE_STRING || tok->tokenType == TYPE_IDENTIFIER){
        if(tok->tokenType == TYPE_IDENTIFIER){
            bool checkVar = isVar(T, tok->attr.strAttr, false);
            if(checkVar == false){
                return SEM_ERR_DEFINES;
            }
        }
        return SYN_RET_SUCCESS;
    }
    return SYN_RET_SYNTAX;
}

/**
 * @brief Funkce pro zavolání více single_right
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int right_next(tCache* cache, symTable *T, gen* g, gen* b, gen *rv)
{
    token* tok = readToken(cache, NEXT);
    if(tok->tokenType == TYPE_SEPARATOR)
    {
        int kontrola = single_right(cache, EXP_END_EOL_OR_SEPARATOR,T, NULL, g, b, rv);
        if(kontrola == SYN_RET_MAYBE_SUCCESS){ // nalezený separator
            return right_next(cache, T, g, b, rv);
        } else {
            return kontrola;
        }
    }
    else if(tok->tokenType == TYPE_EOL)
    {
        return SYN_RET_SUCCESS;
    }
    return SYN_RET_SYNTAX;
}

/** 
 * @brief Funkce ,která volá term //kontrola vstupnich hodnot fce
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int arg(tCache* cache, symTable *T, gen* g, gen* b, gen *rv)
{
    token* tok = readToken(cache, STAY);
    if(tok->tokenType == TYPE_END_BRACKET){///pokud nema parametry
        throwToken(cache);
        tok = readToken(cache, NEXT);
        if (tok->tokenType == TYPE_EOL) {
            return SYN_RET_SUCCESS;
        } else if (tok->tokenType == TYPE_SEPARATOR) {
            return SYN_RET_MAYBE_SUCCESS;
        }
    }
    else {
        cacheItem* id = getHead(cache);
        int termCheck = term(cache, T);
        if( termCheck != SYN_RET_SUCCESS) {
            return termCheck;
        }
        else{
            sResult r;
            switch (id->tok->tokenType) {
                case TYPE_INT:
                case TYPE_STRING:
                case TYPE_FLOAT64:
                appendAStack(g->params, id->tok, ARG_TOKEN);
                appendAStack(b->params, id->tok, ARG_TOKEN);
                appendAStack(rv->params, id->tok, ARG_TOKEN);
                break;
                case TYPE_IDENTIFIER:
                search(T, id->tok->attr.strAttr, &r);
                if (r.data.var->type == VAR_UNIVERSAL) {
                    return SEM_ERR_DEFINES;
                }
                appendAStack(g->params, r.data.var, ARG_VAR);
                appendAStack(b->params, r.data.var, ARG_VAR);
                appendAStack(rv->params, id->tok, ARG_TOKEN);
                break;
            }           
            return argNext(cache, T, g, b, rv);
        }

    }
    return SYN_RET_SYNTAX;   
}

/** 
 * @brief Pomocná funkce, která volá term
 * @param cache  pointer na inicializovanopu strukturu tCache
 */
int argNext(tCache* cache, symTable *T, gen* g, gen* b, gen *rv)
{
    token* tok = readToken(cache, NEXT);
    if(tok->tokenType == TYPE_SEPARATOR)
    {
        cacheItem* id = getHead(cache);
        int kontrola = term(cache, T);
        if(kontrola == SYN_RET_SUCCESS){
            sResult r;
            switch (id->tok->tokenType) {
                case TYPE_INT:
                case TYPE_STRING:
                case TYPE_FLOAT64:
                appendAStack(g->params, id->tok, ARG_TOKEN);
                appendAStack(b->params, id->tok, ARG_TOKEN);
                appendAStack(rv->params, id->tok, ARG_TOKEN);
                break;
                case TYPE_IDENTIFIER:
                search(T, id->tok->attr.strAttr, &r);
                if (r.data.var->type == VAR_UNIVERSAL) {
                    return SEM_ERR_DEFINES;
                }
                appendAStack(g->params, r.data.var, ARG_VAR);
                appendAStack(b->params, r.data.var, ARG_VAR);
                appendAStack(rv->params, id->tok, ARG_TOKEN);
                break;
            }           
            return argNext(cache, T, g, b, rv);
        }
        return kontrola;
    }
    else if(tok->tokenType == TYPE_END_BRACKET)
    {
        tok = readToken(cache, NEXT);
        if(tok->tokenType == TYPE_EOL){
            return SYN_RET_SUCCESS;
        }
    }
    return SYN_RET_SYNTAX;
}

/** 
 * @brief Funkce pro získání stavu precedenční analýzy
 * @return vrací error, otevírací/zacírací závorku, rovnost
 */
int precTable(int top, int current){
    if(top == TYPE_OP_ADD || top == TYPE_OP_SUB){   
        if(current == TYPE_OP_ADD || current == TYPE_OP_SUB || current == TYPE_END_BRACKET || current == TYPE_PREC_DOLLAR){
            return TYPE_PREC_END_BRACKET;
        } else {
            return TYPE_PREC_START_BRACKET;
        }
    } else if(top == TYPE_OP_MUL || top == TYPE_OP_DIV){
        if(current == TYPE_START_BRACKET || current == TYPE_IDENTIFIER || current == TYPE_INT || current == TYPE_FLOAT64 || current == TYPE_STRING || current == TYPE_PREC_TER){
            return TYPE_PREC_START_BRACKET;
        } else {
            return TYPE_PREC_END_BRACKET;
        }
    } else if(top == TYPE_END_BRACKET || top == TYPE_IDENTIFIER || top == TYPE_INT || top == TYPE_FLOAT64 || top == TYPE_STRING || top == TYPE_PREC_TER){
        if(!(current == TYPE_START_BRACKET || current == TYPE_IDENTIFIER || current == TYPE_INT || current == TYPE_FLOAT64 || current == TYPE_STRING || current == TYPE_PREC_TER)){
            return TYPE_PREC_END_BRACKET;
        }
    } else if(top == TYPE_PREC_DOLLAR){ //here
        if(!(current == TYPE_END_BRACKET || current == TYPE_PREC_DOLLAR)){
            return TYPE_PREC_START_BRACKET;
        }
    } else if(top == TYPE_START_BRACKET){
        if(current == TYPE_END_BRACKET || current == TYPE_PREC_DOLLAR){
            if(current == TYPE_END_BRACKET){
                return TYPE_PREC_EQUAL;
            }
        } else {
            return TYPE_PREC_START_BRACKET;
        }
    } else if(top >= TYPE_OP_LESS && top <= TYPE_OP_NOT_EQ){
        if(current == TYPE_END_BRACKET || current == TYPE_PREC_DOLLAR){
            return TYPE_PREC_END_BRACKET;
        } else if(current >= TYPE_OP_LESS && current <= TYPE_OP_NOT_EQ){
            return PREC_ERROR;
        } else {
            return TYPE_PREC_START_BRACKET;
        }
    }
    return PREC_ERROR;

    #undef PREC_FIR_AS
    #undef PREC_FIR_MD 
    #undef PREC_FIR_EI  
    #undef PREC_FIR_D  
    #undef PREC_FIR_S 
    #undef PREC_SEC_AS_END
    #undef PREC_SEC_MD_STA
    #undef PREC_SEC_SI_NOT 
    #undef PREC_SEC_D_NOT
    #undef PREC_SEC_S_EE
}

#define CAT_TERMS(tok) (tok->tokenType >= TYPE_IDENTIFIER && tok->tokenType <= TYPE_STRING) //pokud je to ID/int/float/str
#define CAT_MATH(tok) (tok->tokenType >= TYPE_OP_ADD && tok->tokenType <= TYPE_OP_DIV)  //pokud je znaménko
#define CAT_LOG(tok) (tok->tokenType >= TYPE_OP_LESS && tok->tokenType <= TYPE_OP_NOT_EQ) //pokud je porovnání
#define CAT_OPEN(tok) (tok->tokenType == TYPE_START_BRACKET)
#define CAT_CLOSE(tok) (tok->tokenType == TYPE_END_BRACKET)

/** 
 * @brief Pomocná funkce, která volá term
 * @param op  operátor
 * @param a  první hodnota
 * @param b  druha hodnota
 * 
 */
int precPrint(int op, precItem* a, precItem* b, int* returnID, int* retType, gen* g){                                    
        int aType, bType;
        if (a->dataType == TYPE_IDENTIFIER) {
            vData* data = getVarData(g->st, ((token*)(a->item))->attr.strAttr);
            aType = data->type;
        } else {
            aType = a->dataType;
        }
        if (b->dataType == TYPE_IDENTIFIER) {
            vData* data = getVarData(g->st, ((token*)(b->item))->attr.strAttr);
            bType = data->type;
        } else {
            bType = b->dataType;
        }

        if (aType == TYPE_STRING && (op == TYPE_OP_SUB || op == TYPE_OP_MUL || op == TYPE_OP_DIV)) {
            return SEM_ERR_TYPE;
        }

        if (aType == bType && aType != VAR_BOOL && bType != VAR_BOOL) {
            if (op >= TYPE_OP_LESS && op <= TYPE_OP_NOT_EQ) {
                *retType = VAR_BOOL;
            } else {
                *retType = aType;
            }
        } else {
            return SEM_ERR_TYPE;
        }
        
        if (op != TYPE_OP_LESS_EQ && op != TYPE_OP_GREATER_EQ && op != TYPE_OP_NOT_EQ) {
            *returnID = gen_calc(g,a,b,op);
        } else {
            *returnID = gen_noncalc(g,a,b,op);
        }
    return SYN_RET_SUCCESS; 
}



int precAnalysis(tCache* cache, cacheItem* headBack, precList* item, gen* g, int* returnID, int* retType){
    
    precInit(item);
    setHead(cache, headBack);
    token* tok = readToken(cache, STAY);
    int kontrola;
    int lastTerm;   //a
    int current;    //b
    int tableVal;
    bool Read = PREC_READ;

    kontrola = precAdd(item, TYPE_PREC_DOLLAR,0 , ARG_SPECIAL, cache);  //pridani prvni prvku do zasobniku "$"
    if(kontrola != SEM_RET_SUCCESS){
        return kontrola;
    }
    
    while(1){
        if(!(CAT_TERMS(tok) || CAT_MATH(tok) || CAT_LOG(tok) || CAT_OPEN(tok) || CAT_CLOSE(tok)) && Read == PREC_READ){
            Read = PREC_STOP_READ;
        }
        item->saveHead = item->head;
        lastTerm = precCurTer(item);    //a - zavolání funkce top
        item->head = item->saveHead;
        if(Read){
            current = tok->tokenType;       //b
        } else {
            current = TYPE_PREC_DOLLAR;
        }
        tableVal = precTable(lastTerm, current);    //161 102
        if(tableVal == PREC_ERROR){
            return tableVal;
        }      
        switch (tableVal){
            
            case TYPE_PREC_EQUAL:   // "="
                kontrola = precAdd(item, tok->tokenType, tok->tokenType, ARG_TOKEN, cache);    //push(b)
                if(kontrola != SEM_RET_SUCCESS){
                    return kontrola;
                }
                if(Read){
                    throwToken(cache);  //načtení dalšího tokenu
                    tok = readToken(cache, STAY);
                }
                break;      
            
            case TYPE_PREC_START_BRACKET:   // "<"
                kontrola = precAdd(item, TYPE_PREC_START_BRACKET, 0, ARG_SPECIAL, cache);  //zaměnění "a" za "a<"
                if(kontrola != SEM_RET_SUCCESS){
                    return kontrola;
                }
                item->bracCntr += 1;
                kontrola = precAdd(item, tok->tokenType, tok->tokenType, ARG_TOKEN, cache);  //push(b)
                if(kontrola != SEM_RET_SUCCESS){
                    return kontrola;
                }
                if(Read){
                    throwToken(cache);  //načtení dalšího tokenu
                    tok = readToken(cache, STAY);
                }
                break;
            
            case TYPE_PREC_END_BRACKET:     // ">"
                if(item->bracCntr){   //jestli je <y
                    if(item->head->type >= TYPE_IDENTIFIER && item->head->type <= TYPE_STRING){  //první pravidlo z prec. analýzy pravidel - E -> ID/int/float/string
                        if(item->head->ptrPrev->type == TYPE_PREC_START_BRACKET){ 
                            precChange(item); //změna "<" za E
                            
                            
                            item->bracCntr -= 1;
                            break;
                        }
                    } else if(item->head->type == TYPE_PREC_TER){   //druhé - E -> E operátor E
                        if(item->head->ptrPrev->type >= TYPE_OP_ADD && item->head->ptrPrev->type <= TYPE_OP_NOT_EQ){
                            if(item->head->ptrPrev->ptrPrev->type == TYPE_PREC_TER){
                                if(item->head->ptrPrev->ptrPrev->ptrPrev->type == TYPE_PREC_START_BRACKET){
                                    int dataType;
                                    int resultID;
                                    kontrola = precPrint(item->head->ptrPrev->type, item->head->ptrPrev->ptrPrev, item->head, &resultID, &dataType, g);
                                    if(kontrola != CODEGEN_RET_SUCCESS){
                                        return kontrola;
                                    }
                                    if (retType) {
                                        *retType = dataType;
                                    }
                                    if (returnID) {
                                        *returnID = resultID;
                                    }                                  
                                    precDelete(item);   //smazání E
                                    precDelete(item);   //smazání operátoru
                                    precDelete(item);   //smazání E
                                    precDelete(item);   //nahrazení "<" za E
                                    precAdd(item, TYPE_PREC_TER, dataType, ARG_ID, (void*)(&resultID));
                                    
                                    item->bracCntr -= 1;
                                    break;
                                }
                            }
                        }
                    } else if(item->head->type == TYPE_END_BRACKET){   //třetí E -> (E)
                        if(item->head->ptrPrev->type == TYPE_PREC_TER){
                            if(item->head->ptrPrev->ptrPrev->type == TYPE_START_BRACKET){
                                if(item->head->ptrPrev->ptrPrev->ptrPrev->type == TYPE_PREC_START_BRACKET){
                                    precDelete(item);   //smazání ")"
                                    precChange(item);   //nahrazení "<" za E
                                    precDeletePrev(item);   //smazání "("
                                    
                                   
                                    item->bracCntr -= 1;
                                    break;
                                }
                            }
                        }
                    
                    }
                }
                return PREC_ERROR;
            default:
                return PREC_ERROR;
        }
        if(item->first->ptrNext->type == TYPE_PREC_TER && !(item->first->ptrNext->ptrNext) && !(CAT_TERMS(tok) || CAT_MATH(tok) || CAT_LOG(tok) || CAT_OPEN(tok) || CAT_CLOSE(tok))){
            precDestroy(item);
            return PREC_SUCCESS;
        }
    }
}

/**
 * Zpracování výrazů
 */
int expression(tCache* cache, symTable *T, int* semType, gen* g, int* resultID) {
    #define CAT_TERMS(tok) (tok->tokenType >= TYPE_IDENTIFIER && tok->tokenType <= TYPE_STRING) //pokud je to ID/int/float/str
    #define CAT_MATH(tok) (tok->tokenType >= TYPE_OP_ADD && tok->tokenType <= TYPE_OP_DIV)  //pokud je znaménko
    #define CAT_LOG(tok) (tok->tokenType >= TYPE_OP_LESS && tok->tokenType <= TYPE_OP_NOT_EQ) //pokud je porovnání
    #define CAT_OPEN(tok) (tok->tokenType == TYPE_START_BRACKET)
    #define CAT_CLOSE(tok) (tok->tokenType == TYPE_END_BRACKET)
    token* cur = readToken(cache, STAY);
    token* back = cur;
    
    bool expr_ok = true;
    bool can_end = true;
    bool stop = false;
    int bracketCounter = 0;
    
    int type = 0;
    
    cacheItem* headBack = getHead(cache);
    precList item;
    
    // kontrola prvního tokenu
    if (CAT_TERMS(cur) || CAT_OPEN(cur)) { // počítadlo dodělat, kontroly
        if (CAT_OPEN(cur)) {
            bracketCounter++;
            can_end = false;
        }
        throwToken(cache);
        cur = readToken(cache, STAY);
    } else {
        return SYN_RET_SYNTAX;
    }

    while (!stop) {
        if (CAT_TERMS(back)) { // po termu může následovat jen znaménko a zavírací závorka
            ///basic sémantická kontrola -> neřeší změnu při použití relačního operátoru
            if (type == 0) { // nastavení kompatibilního typu
                if (back->tokenType == TYPE_IDENTIFIER) {
                    if (strcmp(back->attr.strAttr, "_") == 0) {
                        return SEM_ERR_DEFINES;
                    }
                    vData* var = getVarData(T, back->attr.strAttr);
                    if (var) { 
                        type = var->type;
                    } else {
                        return SEM_ERR_DEFINES;
                    }
                } else {
                    type = back->tokenType;
                }
                if (semType) { // pokud tuhle feature nepotřebuju, použiju NULL
                    *semType = type;
                }
            } else { // ověření typové kompatibility výrazu
                if (back->tokenType == TYPE_IDENTIFIER) {
                    if (strcmp(back->attr.strAttr, "_") == 0) {
                        return SEM_ERR_DEFINES;
                    }
                    vData* var = getVarData(T, back->attr.strAttr);
                    if (var) {
                        type = var->type;
                    } else {
                        return SEM_ERR_DEFINES;
                    }
                    if (type != var->type) {
                        return SEM_ERR_TYPE;
                    }
                } else {
                    if (type != back->tokenType) {
                        return SEM_ERR_TYPE;
                    }
                }
            }
            if (CAT_CLOSE(cur)) {
                bracketCounter--;
                if (bracketCounter == 0) { // při vyrovnaném poměru otevřených a zavřených závorek je výraz v pořádku
                    can_end = true;
                } else if (bracketCounter < 0) { // pokud podteče počet závorek, pak jsme jistě mimo výraz, měli bychom zastavit a nechat rozhodnout volajícího
                    stop = true;
                } else { // pokud je zavíracích méně než otvíracích, tak zatím nemáme validní výraz
                    can_end = false;
                }
            } else if (!(CAT_MATH(cur) || CAT_LOG(cur))) {
                expr_ok = can_end;
                stop = true;
            } else {    // token je znaménko
                can_end = false;
            }
        } else if (CAT_MATH(back) || CAT_LOG(back)) { // po operátoru může následovat otvírací závorka nebo term
            if (CAT_OPEN(cur)) { 
                bracketCounter++;
                can_end = false;
            } else if (!CAT_TERMS(cur)) { 
                expr_ok = can_end;
                stop = true;
            } else {    // token je term
                if (back->tokenType == TYPE_OP_DIV) { // runtime error při detekci dělení nulou
                    if ((cur->tokenType == TYPE_INT && cur->attr.intAttr == 0) || (cur->tokenType == TYPE_FLOAT64 && cur->attr.floatAttr == 0.f)) { 
                        return SYN_RET_RUNTIME;
                    }
                }
                can_end = true;
            }
        } else if (CAT_OPEN(back)) { // po otevřené závorce může být další nebo term
            if (CAT_OPEN(cur)) {
                bracketCounter++;
            } else if (!CAT_TERMS(cur)) {
                expr_ok = can_end;
                stop = true;
            } else { // token je term
                can_end = false; // sice je to term, ale je obvious, že po otevřené závorce není validní skončit
            }
        } else if (CAT_CLOSE(back)) { // po uzavřené závorce může být další nebo znaménko
            if (CAT_CLOSE(cur)) {
                bracketCounter--;
                if (bracketCounter == 0) { // při vyrovnaném poměru otevřených a zavřených závorek je výraz v pořádku
                    can_end = true;
                } else if (bracketCounter < 0) { // pokud podteče počet závorek, pak jsme jistě mimo výraz, měli bychom zastavit a nechat rozhodnout volajícího
                    stop = true;
                } else { // pokud je zavíracích méně než otvíracích, tak zatím nemáme validní výraz
                    can_end = false;
                }
            } else if (!(CAT_MATH(cur) || CAT_LOG(cur))) {
                expr_ok = can_end;
                stop = true;
            } else {    // token je znaménko
                can_end = false;
            }
        }
        if (expr_ok && !stop) {
            throwToken(cache);
            back = cur;
            cur = readToken(cache, STAY);
        }
    }
    // určení návratové hodnoty
    if (!expr_ok) {
        fprintf(stderr, "Chybná syntax výrazu\n");
        return SYN_RET_SYNTAX;
    } else {
        return precAnalysis(cache, headBack, &item, g, resultID, semType);
    }

    #undef CAT_TERMS
    #undef CAT_MATH
    #undef CAT_LOG
    #undef CAT_OPEN
    #undef CAT_CLOSE
}

/**
 * Debug funkce, která vypisuje informace o tokenu a textovou reprezentaci vybraných typů tokenů
 */
void DEBUG_print_token(token *t) {
    switch (t->tokenType) {
        case TYPE_IDENTIFIER:
            fprintf(stderr,"TYPE_IDENTIFIER\nattr: %s\n",t->attr.strAttr);
        break;
        case TYPE_KEYWORD:
            fprintf(stderr,"TYPE_KEYWORD\nattr: %u\n",t->attr.kwAttr);
        break;
        case TYPE_INT:
            fprintf(stderr,"TYPE_INT\nattr: %lu\n",t->attr.intAttr);
        break;
        case TYPE_FLOAT64:
            fprintf(stderr,"TYPE_FLOAT64\nattr: %g\n",t->attr.floatAttr);
        break;
        case TYPE_STRING:
            fprintf(stderr,"TYPE_STRING\nattr: %s\n",t->attr.strAttr);
        break;
        case TYPE_NULL:
            fprintf(stderr,"Nepodařilo se získat token (TYPE_NULL)\n");
        break;
        default:
            fprintf(stderr,"TYPE_%u\n",t->tokenType);
        break;
    }
}

char* DEBUG_tokchar(token *t) {
    if (t != NULL) {
        switch (t->tokenType) {
            case TYPE_IDENTIFIER:
                return "identifier";
            break;
            case TYPE_KEYWORD:
                return "keyword";
            break;
            case TYPE_INT:
                return "int literal";
            break;
            case TYPE_FLOAT64:
                return "float literal";
            break;
            case TYPE_STRING:
                return "string literal";
            break;
            case TYPE_START_SECTION:
                return "{";
            break;
            case TYPE_END_SECTION:
                return "}";
            break;
            case TYPE_START_BRACKET:
                return "(";
            break;
            case TYPE_END_BRACKET:
                return ")";
            break;
            case TYPE_OP_ADD:
                return "+";
            break;
            case TYPE_OP_SUB:
                return "-";
            break;
            case TYPE_OP_MUL:
                return "*";
            break;
            case TYPE_OP_DIV:
                return "/";
            break;
            case TYPE_OP_SET:
                return "=";
            break;
            case TYPE_OP_DEF:
                return ":=";
            break;
            case TYPE_OP_LESS:
                return "<";
            break;
            case TYPE_OP_LESS_EQ:
                return "<=";
            break;
            case TYPE_OP_GREATER:
                return ">";
            break;
            case TYPE_OP_GREATER_EQ:
                return ">=";
            break;
            case TYPE_OP_EQUALS:
                return "==";
            break;
            case TYPE_OP_NOT_EQ:
                return "!=";
            break;
            case TYPE_SEPARATOR:
                return ",";
            break;
            case TYPE_SEMICOLON:
                return ";";
            break;
            case TYPE_EOL:
                return "EOL";
            break;
            case TYPE_EOF:
                return "EOF";
            break;
            default:
                return "unknown";
            break;

        }
    }
    return "null";
}

/**
 * @brief   fce na počítání počtu položek ve vStacku (počtu proměnných na levé straně)
 * 
 * @param   ukazetel na vStack
 * @return  počet proměnných na levé straně
 */
int vStackCtr(vStack* v){
    int ctr = 0;
    vStackItem *iter = v->first;
    while(iter != NULL){
        ctr++;
        iter = iter->ptrNext;
    }
    return ctr;
}
/**
 * @brief   fce na počítání počtu položek ve aStacku (počtu argumentů fce)
 * 
 * @param   ukazetel na aStack
 * @return  počet argumentů fce
 */
int aStackCtr(aStack* a){
    int ctr = 0;
    aStackItem *iter = a->first;
    while(iter != NULL){
        ctr++;
        iter = iter->ptrNext;
    }
    return ctr;
}