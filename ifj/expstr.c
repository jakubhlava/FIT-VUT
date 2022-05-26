/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file expstr.c
 * @brief Knihovna pro práci s rostoucími řetězci textu
 * @author Jakub Hlava (xhlava52)
 * @date 10/2020
 */


#include "expstr.h"

/** Funkce pro inicializaci expStr
 * Naalokuje počáteční paměť a inicializuje parametry struktury
 * @param str   pointer na neinicializovanou strukturu expStr
 * 
 * NETESTUJE, ZDA JE STRUKTURA INICIALIZOVANÁ, PŘI REINICIALIZACI VZNIKÁ MEMORY LEAK!
 */
int initEStr(expStr *str) {
    str->data = malloc(sizeof(char)*START_SIZE); // Alokace paměti pro START_SIZE charů
    if (str->data == NULL) {
        return EXPSTR_ERR_REALLOC;
    }
    if (str->data) { 
        //str->lastWrite = 0;
        str->termPos = 0;
        str->len = START_SIZE;
        str->data[0] = '\0';
    } 
    return 0;
}

/**
 * Přidá na konec expStr znak nebo jej před přidáním znaku zvětší
 * @param   str pointer na inicializovanou strukturu expStr
 * @param   c   znak pro přidání na konec řetězce
 */
int appendEStr(expStr *str, char c) {
    if (str->data != NULL) {
        if (str->termPos == str->len-1) {
            if (expandEStr(str) == EXPSTR_ERR_REALLOC) {
                return EXPSTR_ERR_REALLOC;
            }
        }
        str->data[str->termPos] = c;
        str->data[str->termPos+1] = '\0';
        str->termPos++;
    } else {
        return EXPSTR_ERR_UNINITIALIZED;
    }
    return 0;
}

/**
 * Připojí na konec expStr řetězce další statický řetězec, případně jej zvětší
 * @param   str pointer na inicializovanou strukturu expStr
 * @param   ext pointer na statický řetězec
 */
int extendEStr(expStr *str, char* ext) {
    if (str->data != NULL) {
        int extLen = strlen(ext);
        int freeSpace = (str->len-1)-str->termPos; // len - 1 je maximální index, odečtením max. indexu od indexu terminujícího znaku získám volné místo
        if (freeSpace < extLen) { 
            if (mExpandEStr(str, extLen-freeSpace) == EXPSTR_ERR_REALLOC) {
                return EXPSTR_ERR_REALLOC;
            }
        }
        strcpy(&str->data[str->termPos], ext);
        str->termPos += extLen;
    } else {
        return EXPSTR_ERR_UNINITIALIZED;
    }
    return 0;
}


/**
 * Expanduje data struktury expStr o EXPAND_RATE-násobek aktuální velikosti
 * @param   str pointer na inicializovanou strukturu expStr
 */
int expandEStr(expStr *str) {
    if (str->data != NULL) {
        int newLen = str->len*EXPAND_RATE;
        char* newData = realloc(str->data, sizeof(char)*newLen);
        if(newData) {
            str->data = newData;
            str->len = newLen;
        } else {
            return EXPSTR_ERR_REALLOC;
        }
    } else {
        return EXPSTR_ERR_UNINITIALIZED;
    }
    return 0;
}

/**
 * Expanduje data struktury expStr EXPAND_RATE-krát, minimálně všask o velikost min
 * @param   str pointer na inicializovanou strukturu expStr
 * @param   min minimální velikost rozšíření řetězce
 */
int mExpandEStr(expStr *str, unsigned int min) {
    if (str->data != NULL) {
        int newLen = str->len*EXPAND_RATE + min; // pravděpodobně velmi neefektivní
        char* newData = realloc(str->data, sizeof(char)*newLen);
        if (newData) {
            str->data = newData;
            str->len = newLen;
        } else {
            return EXPSTR_ERR_REALLOC;
        }
    } else {
        return EXPSTR_ERR_UNINITIALIZED;
    }
    return 0;
}

/**
 * Dealokuje a ničí strukturu expStr
 * @param   str pointer na inicializovanou nebo neinicializovanou strukturu expStr
 */
void destroyEStr(expStr *str) {
    if (str == NULL) {
        return;
    }
    free(str->data);
    str->len = 0;
    str->termPos = 0;
}

/**
 * Zkopíruje expStr do pevného pole charů
 * @param   str     pointer na inicializovanou strukturu expStr
 * @return  *char   pole charů o velikosti str->len
*/
char *copyEStr(expStr *str){
    char *retStr = malloc(sizeof(char)*str->termPos+1);
    if (retStr == NULL) {
        return NULL;
    }
    memcpy(retStr, str->data, str->termPos+1);
    return retStr;
}

/**
 * Nastaví expStr jako vyčištěný, aby se uložená data přepsala
 * @param   str     pointer na inicializovanou strukturu expStr
 */
void clearEStr(expStr *str) {
    // Vyčištění se provádí posunem nulového znaku na 0. pozici, čímž dojde k přepisu existujících dat
    str->data[0] = '\0';
    str->termPos = 0;
}

/**
 * Převede expStr na 64bitový unsigned integer
 * @param   str     pointer na inicializovanou strukturu expStr
 */
uint64_t convEStr2Int(expStr *str) {
    char* normalStr = copyEStr(str);
    uint64_t num = strtoll(normalStr, NULL, 10);
    free(normalStr);
    return num;
}

/**
 * Převede expStr na double
 * @param   str     pointer na inicializovanou strukturu expStr
 */
double convEStr2Double(expStr *str) {
    char* normalStr = copyEStr(str);
    double num = strtod(normalStr, NULL);
    free(normalStr);
    return num;
}


/**
 * @brief Inicializace pole typů
 * @param   ta      pointer na pole typů
 */
void initTA(typeArr* ta) {
    ta->array = NULL;
    ta->len = 0;
}

/**
 * @brief Připojení typu na konec pole
 * @param   ty      pointer na inicializované pole typů
 * @param   type    prvek pro připojení
 * @return  0                   při úspěchu
 * @return  EXPSTR_ERR_REALLOC  při chybě alokace
 */
int appendTA(typeArr* ta, int type) {
    if (ta->len == 0) {
        ta->array = malloc(sizeof(int)*++ta->len);
        ta->array[ta->len-1] = type;
        return 0;
    } else {
        int* new = realloc(ta->array, sizeof(int)*++ta->len);
        if (new) {
            ta->array = new;
            ta->array[ta->len-1] = type;
            return 0;
        } else {
            free(ta->array);
            return EXPSTR_ERR_REALLOC;
        }
    }
}

/**
 * @brief Deinicializace pole typů
 * @param   ta      pointer na pole typů
 */
void destroyTA(typeArr* ta) {
    free(ta->array);
    ta->array = NULL;
    ta->len = 0;
}