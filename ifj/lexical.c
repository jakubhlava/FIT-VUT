/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file lexical.c
 * @brief Lexikální analyzátor pro jazyk IFJ20
 * @author Jakub Hlava (xhlava52)
 * @date 10/2020
*/

#include "lexical.h"

/**
 * Načítá znaky dokud nenalezne validní token nebo lexikální chybu podle pravidel jazyka IFJ20
 * @param   tok         pointer na strukturu token, do které je načten případný nalezený nebo chybový token
 * @param   buffered    pointer na znak, který slouží jako buffer pro předávání načtených, ale automatem nezpracovaných znaků
 * @param   EOLState    hodnota podle lexical.h, která značí, jestli příští token může, musí nebo nesmí být TYPE_EOL
 * 
 * @return  stavový kód podle lexical.h
 */
int getToken(token *tok, char *buffered) {
    int state = STATE_START;
    int nextState = STATE_START;
    char buf = *buffered; // načtení poslední znaku z bufferu vyšší funkce
    int ret = LEX_SUCCESS; // návratová hodnota, nastavuje se v průběhu běhu funkce
    bool clearBuf = false; // vyčištění zpětného bufferu před koncem stavu v případě, že znak již není potřeba

    // DEBUG
    #ifdef DEBUG
    int lastState = state;
    #endif

    expStr tempEStr; // řetězec pro ukládání načítaného textu pro pozdější převod na atribut tokenu
    int status = initEStr(&tempEStr);
    if (status == EXPSTR_ERR_REALLOC) { // chyba při nepodařené alokaci
        nextState = STATE_EXIT;
        ret = LEX_ERR_INTERNAL;
    }
    while (nextState != STATE_EXIT) {
        if (buf == 0) {
            buf = getchar(); // načtení znaku z stdin
        }
        bool ignore = false; // pro případ komentářového stavu - blokuje tvorbu atributu
        // ----------------------------------------
        //  Logika přechodů mezi stavy
        // ----------------------------------------
        switch (state) {
            // Počáteční stav automatu
            case STATE_START: 
                // ----------------------------------------
                //  Detekce víceznakových tokenů
                // ----------------------------------------
                if (buf == '_' || (buf >= 'a' && buf <= 'z') || (buf >= 'A' && buf <= 'Z')) {
                    nextState = STATE_ID; // detekce identifikátoru a klíčového slova
                } else if (buf == '0') {
                    nextState = STATE_LEADING_ZERO;
                } else if (buf >= '1' && buf <= '9') {
                    nextState = STATE_INT; // detekce integeru nebo floatu
                } else if (buf =='"') {
                    nextState = STATE_STR; // detekce řetězce
                    ignore = true;
                } else if (buf == '=') {
                    nextState = STATE_SET; // detekce operátoru přiřazení a rovnosti
                } else if (buf == ':') { 
                    nextState = STATE_COLON; // detekce operátoru deklarace
                } else if (buf == '>') {
                    nextState = STATE_GREATER; // detekce operátorů > a >=
                } else if (buf == '<') {
                    nextState = STATE_LESS; // detekce operátor < a <=
                } else if (buf == '!') {
                    nextState = STATE_EXCLAMATION; // detekce operátoru !=
                } else if (buf == '/') {
                    nextState = STATE_SLASH; // detekce komentářů nebo operátoru dělení
                    ignore = true;
                // ----------------------------------------
                // Detekce a vyhodnocení EOL
                // ----------------------------------------
                } else if (buf == '\n') {
                    tok->tokenType = TYPE_EOL;
                    clearBuf = true;
                    nextState = STATE_EXIT;
                // ----------------------------------------
                //  Jednoznakové tokeny (self explaining)
                // ----------------------------------------
                } else if (buf == '+') { 
                    tok->tokenType = TYPE_OP_ADD; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == '-') {
                    tok->tokenType = TYPE_OP_SUB; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == '*') {
                    tok->tokenType = TYPE_OP_MUL; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == ',') {
                    tok->tokenType = TYPE_SEPARATOR; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == ';') {
                    tok->tokenType = TYPE_SEMICOLON; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == '(') {
                    tok->tokenType = TYPE_START_BRACKET; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == ')') {
                    tok->tokenType = TYPE_END_BRACKET; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == '{') {
                    tok->tokenType = TYPE_START_SECTION; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == '}') {
                    tok->tokenType = TYPE_END_SECTION; 
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == EOF) {
                    tok->tokenType = TYPE_EOF;
                    ret = LEX_FINISHED;
                    nextState = STATE_EXIT;
                // ----------------------------------------
                //  Bílé znaky
                // ----------------------------------------
                } else if (buf < 33) {
                    nextState = STATE_START;
                    clearBuf = true;
                    ignore = true;
                // ----------------------------------------
                //  Ostatní znaky
                // ----------------------------------------
                } else {
                    ret = LEX_ERR_ANALYSIS; 
                    nextState = STATE_EXIT;
                }
            break;
            // předpoklad detekce identifikátorů nebo klíčového slova
            case STATE_ID:
                if (buf == '_' || (buf >= 'a' && buf <= 'z') || (buf >= 'A' && buf <= 'Z') || (buf >= '0' && buf <= '9')) { 
                    nextState = STATE_ID;
                } else { // neočekávaný znak -> konec tokenu
                    char* attr = copyEStr(&tempEStr);
                    if (attr == NULL) {
                        nextState = STATE_EXIT;
                        ret = LEX_ERR_INTERNAL;
                        continue;
                    }
                    int type = isKW(attr); // ověření, zda jde o klíčové slovo nebo identifikátor
                    if (type == NOT_A_KW) { 
                        tok->tokenType = TYPE_IDENTIFIER;
                        tok->attr.strAttr = attr;
                    } else {
                        tok->tokenType = TYPE_KEYWORD;
                        tok->attr.kwAttr = type;
                        free(attr);
                    }
                    nextState = STATE_EXIT;
                }
            break;
            // prvni nula v integeru
            case STATE_LEADING_ZERO:
                if (buf == 'e' || buf == 'E') {
                    nextState = STATE_FLOAT_EXP;
                } else if (buf == '.') { // přechod do mezistavu k typu float
                    nextState = STATE_INT_DOT;
                } else if (buf >= '0' && buf <= '9') {
                    ret = LEX_ERR_ANALYSIS;  
                    nextState = STATE_EXIT;
                } else {
                    tok->tokenType = TYPE_INT;
                    tok->attr.intAttr = convEStr2Int(&tempEStr);
                    nextState = STATE_EXIT;
                }
            break;
            // předpoklad detekce integeru nebo floatu
            case STATE_INT:
                if (buf >= '0' && buf <= '9') {
                    nextState = STATE_INT;
                } else if (buf == 'e' || buf == 'E') {
                    nextState = STATE_FLOAT_EXP;
                } else if (buf == '.') { // přechod do mezistavu k typu float
                    nextState = STATE_INT_DOT;
                } else {
                    tok->tokenType = TYPE_INT;
                    tok->attr.intAttr = convEStr2Int(&tempEStr);
                    nextState = STATE_EXIT;
                }
            break;
            // mezistav, integeru s desetinnou tečkou bez desetinné části
            case STATE_INT_DOT:
                if (buf >= '0' && buf <= '9') {
                    nextState = STATE_FLOAT;                   
                } else {
                    ret = LEX_ERR_ANALYSIS;  
                    nextState = STATE_EXIT;
                }
            break;
            // předpoklad detekce floatu
            case STATE_FLOAT:
                if (buf >= '0' && buf <= '9') { 
                    nextState = STATE_FLOAT;
                } else if (buf == 'e' || buf == 'E') {
                    nextState = STATE_FLOAT_EXP;
                } else {
                    tok->tokenType = TYPE_FLOAT64;
                    tok->attr.floatAttr = convEStr2Double(&tempEStr);
                    nextState = STATE_EXIT;
                }
            break;
            // mezistav floatu s znažkou exponentu, bez exponentu
            case STATE_FLOAT_EXP:
                if (buf >= '0' && buf <= '9') {
                    nextState = STATE_FLOAT_EXP_FIN;
                } else if ( buf == '+' || buf == '-') {
                    nextState = STATE_FLOAT_OP;
                } else {
                    ret = LEX_ERR_ANALYSIS; 
                    nextState = STATE_EXIT;
                }
            break;
            /// mezistav s operatorem bez exponentu
            case STATE_FLOAT_OP:
                if (buf >= '0' && buf <= '9') {
                    nextState = STATE_FLOAT_EXP_FIN;
                } else {
                    ret = LEX_ERR_ANALYSIS; 
                    nextState = STATE_EXIT;
                }
            break;
            // stav floatu, při kterém se načítá velikost exponentu
            case STATE_FLOAT_EXP_FIN:
                if (buf >= '0' && buf <= '9') {
                    nextState = STATE_FLOAT_EXP_FIN;
                } else {
                    tok->tokenType = TYPE_FLOAT64;
                    tok->attr.floatAttr = convEStr2Double(&tempEStr);
                    nextState = STATE_EXIT;
                } 
            break;
            // předpoklad načítání řetězce
            case STATE_STR:
                if (buf == '"') {
                    tok->tokenType = TYPE_STRING;
                    char* attr = copyEStr(&tempEStr);
                    if (attr == NULL) {
                        nextState = STATE_EXIT;
                        ret = LEX_ERR_INTERNAL;
                        continue;
                    }
                    int stat;
                    tok->attr.strAttr = processString(attr, true, &stat); // zpracování stringu z formátu IFJ20  na formát IFJcode20
                    if (tok->attr.strAttr == NULL) { // ošetření možnách chyb při zpracování
                        switch (stat) {
                            case STATUS_INTERNAL_ERROR:
                                ret = LEX_ERR_INTERNAL;
                            break;
                            case STATUS_CONV_ERROR:
                                ret = LEX_ERR_ANALYSIS;
                            break;
                        }
                        nextState = STATE_EXIT;
                        continue;
                    }
                    clearBuf = true;
                    nextState = STATE_EXIT;
                } else if (buf == '\\') {
                    nextState = STATE_STR_ESC;
                } else if (buf == '\n') { // pokud řetězec přesahuje do dalšího řádku, jedná se o chybu
                    nextState = STATE_EXIT;
                    ret = LEX_ERR_ANALYSIS; 
                } else if (buf == EOF) { // pokud řetězec není do konce souboru ukončen, pak se také jedná o chybu
                    nextState = STATE_EXIT;
                    ret = LEX_ERR_ANALYSIS; 
                } else {
                    nextState = STATE_STR;
                }
            break;
            // mezistav escapeovaných znaků, ověření jejich validity (zpracování probíhá až na závěr v processString)
            case STATE_STR_ESC:
                if (buf == '"' || buf == 'n' || buf == 't' || buf == '\\' || buf == 'x') {
                    nextState = STATE_STR;
                } else {
                    nextState = STATE_EXIT;
                    ret = LEX_ERR_ANALYSIS; 
                }
            break;
            // rozhodování mezi dělením, jedno- a víceřádkovým komentářem
            case STATE_SLASH:
                if (buf == '/') {
                    nextState = STATE_COMMENTLINE;
                    ignore = true;
                } else if (buf == '*') {
                    nextState = STATE_COMMENTML;
                    ignore = true;
                } else {
                    ignore = false;
                    tok->tokenType = TYPE_OP_DIV;
                    nextState = STATE_EXIT;
                }
            break;
            // předpoklad jednořádkového komentáře
            case STATE_COMMENTLINE:
                if (buf == '\n') {
                    nextState = STATE_START; // za koncem komentáře hledáme další token, komentář není token
                    clearEStr(&tempEStr);
                    ignore = true;
                } else {
                    nextState = STATE_COMMENTLINE;
                }
            break;
            // předpoklad víceřádkového komentáře
            case STATE_COMMENTML:
                if (buf == '*') {
                    nextState = STATE_STAR;
                } else if (buf == EOF) { // víceřádkový komentář musí být ukončen před koncem souboru
                    nextState = STATE_EXIT;
                    ret = LEX_ERR_ANALYSIS; 
                } else {
                    nextState = STATE_COMMENTML;
                }
            break;
            // mezistav možného končícího multiline komentáře
            case STATE_STAR:
                if (buf == '/') {
                    nextState = STATE_START;
                    ignore = true;
                    clearEStr(&tempEStr);
                } else {
                    nextState = STATE_COMMENTML;
                }
            break;
            // předpoklad operátoru přiřazení nebo rovnosti
            case STATE_SET:
                if (buf == '=') {
                    tok->tokenType = TYPE_OP_EQUALS; 
                    clearBuf = true;
                } else {
                    tok->tokenType = TYPE_OP_SET;     
                }
                nextState = STATE_EXIT;
            break;
            // mezistav operátoru deklarace proměnné
            case STATE_COLON:
                if (buf == '=') {
                    tok->tokenType = TYPE_OP_DEF;
                    clearBuf = true;
                } else {
                    ret = LEX_ERR_ANALYSIS; 
                }
                nextState = STATE_EXIT;
            break;
            // stav pro < a <=
            case STATE_LESS:
                if (buf == '=') {
                    tok->tokenType = TYPE_OP_LESS_EQ;
                    clearBuf = true;
                } else {
                    tok->tokenType = TYPE_OP_LESS;
                }
                nextState = STATE_EXIT;
            break;
            // stav pro > a >=
            case STATE_GREATER:
                if (buf == '=') {
                    tok->tokenType = TYPE_OP_GREATER_EQ;
                    clearBuf = true;
                } else {
                    tok->tokenType = TYPE_OP_GREATER;
                }
                nextState = STATE_EXIT;
            break;
            // mezistav a vyhodnocení pro !=
            case STATE_EXCLAMATION:
                if (buf == '=') {
                    tok->tokenType = TYPE_OP_NOT_EQ;
                    clearBuf = true;
                } else {
                    ret = LEX_ERR_ANALYSIS; 
                }
                nextState = STATE_EXIT;
            break;
        }
        // ----------------------------------------
        //  Konec logiky přechodů
        // ----------------------------------------
        if (!clearBuf) {
            *buffered = buf; // uložení aktuálního stavu do bufferu
        } else {
            *buffered = '\0';
            clearBuf = false;
        }
        if (!ignore && !clearBuf) { 
            int status = appendEStr(&tempEStr, buf); // připojení aktuálního znaku na konec řetězce
            if (status == EXPSTR_ERR_REALLOC || status == EXPSTR_ERR_UNINITIALIZED) { // ošetření chyb odesláním chybového stavu výše
                nextState = STATE_EXIT;
                ret = LEX_ERR_INTERNAL;
            }
        }
        #ifdef DEBUG
        lastState = state;
        if (ret == LEX_ERR_ANALYSIS) {
            LEXDUMP
        }
        #endif
        state = nextState; // přechod mezi stavy
        buf = 0; // vynulování posledního znaku kvůli signalizaci pro načítání
    }

    destroyEStr(&tempEStr); // bezpečné uvolnění paměti po expStr
    
    return ret;
}

/**
 * Zjišťuje zda a případně jaké klíčové slovo je řetězec
 * @param   attr    řetězec, načtený při skenování tokenu
 * @return  typ klíčového slova podle lexical.h nebo NOT_A_KW pokud se o klíčové slovo nejedná
 */
int isKW(char *attr) {
    if (strcmp(attr, "if") == 0) {
        return KW_IF;
    } else if (strcmp(attr, "else") == 0) {
        return KW_ELSE;
    } else if (strcmp(attr, "for") == 0) {
        return KW_FOR;
    } else if (strcmp(attr, "func") == 0) {
        return KW_FUNC;
    } else if (strcmp(attr, "return") == 0) {
        return KW_RET;
    } else if (strcmp(attr, "int") == 0) {
        return KW_INT;
    } else if (strcmp(attr, "string") == 0) {
        return KW_STR;
    } else if (strcmp(attr, "float64") == 0) {
        return KW_FLOAT;
    } else if (strcmp(attr, "package") == 0) {
        return KW_PACKAGE;
    } else {
        return NOT_A_KW;
    }
}

/**
 * Funkce pro převedení řetězce z formátu v jazyce IFJ20 do formátu akceptovaného jazykem IFJcode20
 * @param   attr    řetězec načtený při skenování tokenu
 * @return  pointer na převedený řetězec nebo NULL, pokud se převod nepodaří
 */
char *processString(char *attr, bool freeAttr, int *status) {
    expStr processedString; 
    initEStr(&processedString); 
    char processing = attr[0]; // pracovní proměnná pro zpracovávaný znak
    uint64_t counter = 0; // pozice v řetězci
    *status = STATUS_OK;
    while (processing != '\0') {
        if (processing <= 32) { // vyhodnocení bílých znaků -> přepis na ascii kód
            char formatted[5];
            sprintf(formatted, "\\0%02u", processing); // zformátování kódu znaku
            extendEStr(&processedString, formatted);
        } else if (processing == '\\') { // zpracování escape sekvencí
            char temp = attr[++counter]; // druhý pracovní znak
            switch (temp) {
                case 'n': // newline
                    extendEStr(&processedString, "\\010");
                break;
                case 't': // tabulátor
                    extendEStr(&processedString, "\\009");
                break;
                case '\\': // lomítko
                    extendEStr(&processedString, "\\092");
                break;
                case '"': // uvozovka
                    appendEStr(&processedString, '"');
                break;
                // zpracování vkládání ASCII znaků pomocí escape sekvence
                case 'x': ; // ; kvůli tomu, že za návěštím musí následovat statement
                    char conv[3] = {attr[++counter],attr[++counter],'\0'}; // načtení dvou dalších znaků pro převod na číslo
                    char *endPtr;
                    int converted = strtol(conv,&endPtr,16); // konverze HEX čísla na znak
                    if (endPtr != &conv[2]) {
                        *status = STATUS_CONV_ERROR;
                        break;
                    }
                    char formatted[5];
                    sprintf(formatted, "\\%03u", converted); // formátování znaku do pořadovaného formátu pro IFJcode20
                    extendEStr(&processedString, formatted);
                break;
            }
        } else if (processing == '#') { // křížek musí dle pravidel IFJcode20 být také zadán v ASCII
            extendEStr(&processedString, "\\035");
        } else { // prostě vložení ostatních tisknutelných znaků
            appendEStr(&processedString, processing);
        }
        processing = attr[++counter]; // posun v řetězci
    }
    if (freeAttr) { // dealokace původního řetězce pro zjednodušení práce
        free(attr);
    }
    char *retStr = NULL; // NULL kvůli návratu z STATUS_CONV_ERROR
    if (*status == STATUS_OK) {
        retStr = copyEStr(&processedString);
        if (retStr == NULL) { // ochrana při neúspěčném volání malloc
            *status = STATUS_INTERNAL_ERROR;
        }
    }   
    destroyEStr(&processedString);
    return retStr;
}