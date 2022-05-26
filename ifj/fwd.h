/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file fwd.h
 * @brief Dopředné deklarace maker a struktur
 * @author Jakub Hlava (xhlava52)
 * @author Thi Bao Ngoc Vu (xvuthi00)
 * @author Lukáš Kraus (xkraus13)
 * @author Jan Pryč (xprycj00)
 * @date 11/2020
 */

#ifndef _DECLARATION_HEADER
#define _DECLARATION_HEADER

// ===============================================================================
//  LEXIKÁLNÍ ANALÝZA
// ===============================================================================

typedef union tokenAttribute {
    uint64_t intAttr;
    double floatAttr;
    char *strAttr;
    int kwAttr;
} tAttr;

typedef struct lexToken {
    int tokenType; // Typ tokenu viz definice v lexical.h
    tAttr attr; 
    int line;
} token;

/**
 * Definice typů tokenů, které vytváří scanner
*/
#define TYPE_KEYWORD        100 /// klíčové slovo
#define TYPE_IDENTIFIER     101 /// identifikátor
#define TYPE_INT            102 /// hodnota typu celé čislo
#define TYPE_FLOAT64        103 /// hodnota typu C-double
#define TYPE_STRING         104 /// hodnota typu řetězec
#define TYPE_START_SECTION  105 /// otevřená složená závorka
#define TYPE_END_SECTION    106 /// zavřená složená závorka
#define TYPE_START_BRACKET  107 /// otevřená závorka
#define TYPE_END_BRACKET    108 /// zavřená závorka
#define TYPE_OP_ADD         109 /// operátor +
#define TYPE_OP_SUB         110 /// operátor -
#define TYPE_OP_MUL         111 /// operátor *
#define TYPE_OP_DIV         112 /// operátor /
#define TYPE_OP_SET         113 /// operátor = (přiřazení)
#define TYPE_OP_DEF         114 /// operátor := (definice proměnné)
#define TYPE_OP_LESS        115 /// operátor <
#define TYPE_OP_LESS_EQ     116 /// operátor <=
#define TYPE_OP_GREATER     117 /// operátor >
#define TYPE_OP_GREATER_EQ  118 /// operátor >=
#define TYPE_OP_EQUALS      119 /// operátor ==
#define TYPE_OP_NOT_EQ      120 /// operátor !=
#define TYPE_SEPARATOR      121 /// oddělovač ,
#define TYPE_SEMICOLON      122 /// oddělovač ;
#define TYPE_EOL            123 /// konec řádky
#define TYPE_EOF            148 /// konec souboru
#define TYPE_NULL           149 /// defaultní typ

/**
 * Definice typů tokenů, které vytváří precedenční analýza
*/
#define TYPE_PREC_DOLLAR            161 /// dolar
#define TYPE_PREC_START_BRACKET     162 /// hranatá otevírací závorka 
#define TYPE_PREC_END_BRACKET       163 /// hranatá uzavírací závorka
#define TYPE_PREC_EQUAL             164 /// rovná se 
#define TYPE_PREC_TER               165 /// z ID/int/float/string udělám terminál       
#define PREC_ERROR                  5 /// prostě chyba
#define PREC_NOT_DONE               true
#define PREC_IS_DONE                false
#define PREC_SUCCESS                0 /// precedenční analýza je hotová
#define PREC_STOP_READ              false
#define PREC_READ                   true
#define PREC_TYPE_COMP_ERR          5 /// chyba 5 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech
/**
 * Definice názvů klíčových slov pro jejich zjednodušené předávání v tokenu
 */ 
#define KW_IF               151 /// klíčové slovo "if"
#define KW_ELSE             152 /// klíčové slovo "else"
#define KW_FOR              153 /// klíčové slovo "for"
#define KW_FUNC             154 /// klíčové slovo "func"
#define KW_RET              155 /// klíčové slovo "return"
#define KW_INT              156 /// klíčové slovo "int"
#define KW_STR              157 /// klíčové slovo "string"
#define KW_FLOAT            158 /// klíčové slovo "float64"
#define KW_PACKAGE          159 /// klíčové slovo "package"
#define NOT_A_KW            160 /// návratová hodnota, pokud kontrolované slovo není klíčové

// Návratová kódy
#define LEX_SUCCESS         0
#define LEX_ERR_ANALYSIS    1
#define LEX_ERR_INTERNAL    2
#define LEX_FINISHED        99

// ===============================================================================
//  TABULKA SYMBOLŮ
// ===============================================================================


//  Důležitá makra


// datové typy proměnných a parametrů
#define VAR_INT         102
#define VAR_STRING      104
#define VAR_FLOAT64     103
#define VAR_BOOL        200
#define VAR_UNIVERSAL   255     /// _

// typy nalezených identifikátorů
#define TYPE_VAR        10
#define TYPE_FUNC       11
#define TYPE_NONE       19

// POMOCNÁ MAKRA

// návratové hodnoty funkcí
#define STATUS_OK       0
#define STATUS_INT_ERR  1   // VNITŘNÍ CHYBA
#define STATUS_PROG_ERR 2   // CHYBA PROGRAMÁTORA

// návratové hodnoty compareKeys
// určují pokračování stromem, pokud je identifikátor menší => LEFT, větší => RIGHT
// použito i ve fcích: checkNodeExist, findSameVar
#define COMP_EQUALS     20
#define COMP_LEFT       21
#define COMP_RIGHT      22

// návratové hodnoty checkVNodeExist
// návratová hodnota říká, že nebyla nalezena shoda
#define NO_MATCH        30

// POMOCNÉ STRUKTURY

// data o proměnné, ukládaná do tabulky symbolů
typedef struct varData {
    int index;                      // pořadí v rámci volání funkce (pro odlišení při generování výsledného kódu)
    int type;                       // typ proměnné
    char* name;               // název proměnné
} vData;

// data o funkcích ukládaná do tabulky symbolů
typedef struct funcData {
    int inArgC;                     // počet vstupních parametrů
    int outArgC;                    // počet výstupních parametrů
    int* inArgTypes;                // pole s typy vstupních parametrů
    char** inArgNames;        // pole s názvy vstupních parametrů
    int* outArgTypes;               // pole s typy výstupních parametrů
} fData;

// union, aby mohl být pouze jden datový typ pro prvky stromu
typedef union symData {
    vData* var;                     // data v případě, že symbol je proměnná
    fData* func;                    // data v případě, že symbol je funkce
} sData;

// prvek binárního vyhledávacího stromu pro identifikátory
typedef struct treeNode {
    char* key;                      // vyhledávací klíč - jméno identifikátoru funkce/proměnné
    sData data;                     // užitečná data - informace o proměnné nebo funkci
    struct treeNode* lPtr;          
    struct treeNode* rPtr;          // pointery na levý a pravý podstrom
} tNode;

// prvek zásobníku scopů
typedef struct scopeStackItem {
    struct scopeStackItem* prev;    // předchozí prvek na "zásobníku"
    tNode* root;                    // kořen stromu proměnných v daném rámci
} sItem;

// prvek zásobíku volání
typedef struct callStackItem {
    struct callStackItem* prev;     // předchozí prvek na "zásobníku"
    sItem* sStack;                  // zásobník rámců volané funkce
    fData* data;                    // informace o funkci
} cItem;

//  ROZHRANÍ TABULKY SYMBOLŮ

/*
 *  STRUKTURY DŮLEŽITÉ PRO PRÁCI S TABULKOU SYMBOLŮ
 */

// struktura samotné tabulky symbolů
typedef struct symT {
    tNode* funcTree;                // strom funkcí
    cItem* cStack;                  // zásobník volání funkcí
} symTable;

// struktura pro předávání výsledků vyhledávání
typedef struct searchResult {
    int resultType;              // typ výsledku viz výše
    sData data;                    // data o nalezeném symbolu
} sResult;

// ===============================================================================
//  SYNTAKTICKÁ A SÉMANTICKÁ ANALÝZA
// ===============================================================================

#define SYN_RET_SUCCESS         0
#define SYN_RET_EOF             1
#define SYN_RET_SYNTAX          2
#define SYN_RET_REDEF           3
#define SYN_RET_SEMANTIC_OTHER  7
#define SYN_RET_RUNTIME         9

#define SYN_RET_MAYBE_SUCCESS   10

#define SYN_RET_INTERNAL        99

#define NO_BOOKMARK         1

#define SEM_RET_SUCCESS     0
#define SEM_ERR_DEFINES     3   // sémantická chyba - nedefinovaná funkce, proměnná, pokus o redefinici, ...
#define SEM_ERR_NEWTYPE     4   // sémantická chyba - při odvozování datového typu nově definované proměnné
#define SEM_ERR_TYPE        5   // sémantická chyba - typové kompatibility ve výrazech
#define SEM_ERR_FUNC        6   // sémantická chyba - špatný počet nebo typ parametrů při volání funkce nebo návratu z funkce
#define SEM_ERR_SEMANTIC    7   // ostatní sémantické chyby
#define SEM_ERR_DIVBYZERO   9   // sémantická chyba dělení nulou
#define SEM_ERR_INTERNAL    99  // interní chyba překladače neovlivněná překládaným programem (typicky chyba alokace paměti, ...)

// ===============================================================================
//  GENERÁTOR KÓDU
// ===============================================================================

#define CODEGEN_RET_SUCCESS     0
#define CODEGEN_ERR_SYNTAX      1
#define CODEGEN_ERR_ALLOC       50

typedef struct varStackItem {
    vData* var;
    struct varStackItem* ptrNext;
    struct varStackItem* ptrBack;
} vStackItem;

typedef struct varStack {
    vStackItem* first;
    vStackItem* head;
} vStack;

typedef union argument {
    token* tok;
    vData* var;
} argData;

#define ARG_TOKEN   0
#define ARG_VAR     1
#define ARG_ID      2
#define ARG_SPECIAL 3

typedef struct argStackItem {
    int type;
    argData data;
    struct argStackItem* ptrNext;
    struct argStackItem* ptrBack;
} aStackItem;

typedef struct argStack {
    aStackItem* first;
    aStackItem* head;
} aStack;

typedef struct generator {
    symTable* st;
    int globalCounter;
    int rightSideTemp;
    vStack* v;
    aStack* params;
} gen;


typedef struct printQueueItem {
    token* tok;
    struct printQueueItem* next;
    struct printQueueItem* prev;
} pQueueItem;

typedef struct printQueue {
    pQueueItem* top;
    pQueueItem* head;
} pQueue;

typedef struct precItem {   //pro precenční analýzu
    int type;
    int itemType;
    int dataType;
    void* item;
    struct precItem* ptrNext;
    struct precItem* ptrPrev;
} precItem;

typedef struct precListt {
    precItem* first;
    precItem* head;
    precItem* saveHead;
    precItem* terPosition;
    int bracCntr;
    int resType;
} precList;


// MAIN

#define SUCCESS         0   // úspěch
#define ERR_LEXICAL     1   // chyba v rámci lexikální analýzy
#define ERR_SYNTAX      2   // chyba v rámci syntaktické analýzy
#define ERR_DEFINES     3   // sémantická chyba - nedefinovaná funkce, proměnná, pokus o redefinici, ...
#define ERR_NEWTYPE     4   // sémantická chyba - při odvozování datového typu nově definované proměnné
#define ERR_TYPE        5   // sémantická chyba - typové kompatibility ve výrazech
#define ERR_FUNC        6   // sémantická chyba - špatný počet nebo typ parametrů při volání funkce nebo návratu z funkce
#define ERR_SEMANTIC    7   // ostatní sémantické chyby
#define ERR_DIVBYZERO   9   // sémantická chyba dělení nulou
#define ERR_INTERNAL    99  // interní chyba překladače neovlivněná překládaným programem (typicky chyba alokace paměti, ...)

#endif