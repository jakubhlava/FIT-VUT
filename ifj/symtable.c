/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file symtable.c
 * @brief Tabulka symbolů pro překladač jazyka IFJ20
 * @author Jakub Hlava (xhlava52)
 * @author Thi Bao Ngoc Vu (xvuthi00)
 * @date 11/2020
 */

#include "symtable.h"

/**
 * Porovnává dva řetězcové klíče
 * @param   search  hledaný klíč
 * @param   node    klíč aktuálního nodu
 * @return  viz symtable.h "návratové hodnoty"
 */
int compareKeys(char* search, char* node) {
    int pos = 0;
    char buf1 = search[pos];
    char buf2 = node[pos];
    while (buf1 != '\0' && buf2 != '\0') {
        // priorita _ (ascii 95) při řazení náhradou za - (ascii 45)
        if (buf1 == '_') buf1 = '-';
        if (buf2 == '_') buf2 = '-';
        if (buf1 == buf2) {
            pos++;
        } else if (buf1 < buf2) {
            return COMP_LEFT;
        } else {
            return COMP_RIGHT; 
        }
        buf1 = search[pos];
        buf2 = node[pos];
    }
    if (buf1 == '\0' && buf2 == '\0') { 
        return COMP_EQUALS;
    }
    // kratší řetězec považuji za menší
    else if (buf1 == '\0') {
        return COMP_LEFT;
    } else if (buf2 == '\0') {
        return COMP_RIGHT;
    } else {
        return STATUS_INT_ERR;
    }
}

/**
 * fce vrací scoup, ve kterém se nacházíme
 * 
 * @param return    pointer na scope, ve kterém se nacházíme
 */
sItem* actualScope(symTable* T){
    return T->cStack->sStack;
}

/**
 * fce vrací položku callStacku, ve které se nacházíme
 * 
 * @param return    pointer na položku callStacku, ve kterém se nacházíme
 */
tNode* actualRoot(symTable* T){
    return T->cStack->sStack->root;
}

/**
 * vytvoření nody do stromu
 * @param key           klíč - jméno identifikátoru funkce/proměnné
 * @param data          užitečná data - informace o proměnné nebo funkci
 * @param prevNode      pointer předchozího nodu, na který se má nový napojit
 * 
 * @return              pointer na vytvořený node
 */
tNode* createNode( char* key, sData data){
    tNode* newNode =(tNode*) malloc(sizeof(struct treeNode));
    if(newNode == NULL){
        return NULL;
    }
    newNode->key = key;
    newNode->lPtr = NULL;
    newNode->rPtr = NULL;
    newNode->data = data;
    return newNode;
}

/**
 * přidává položku do binárního stromu na správnou pozici
 * před použitím je potřeba volat fci findSameVar nebo checkNodeExist, aby se případně nevytvořila další noda se stejným klíčem 
 * 
 * @param node      pointer na kořen stromu, do kterého chceme vložit
 * @param key       vyhledávací klíč - jméno identifikátoru funkce/proměnné
 * @param data      užitečná data - informace o proměnné nebo funkci
 * 
 * @return          pointer na vytvořenou nodu (NULL v případe neúspěšné alokace, při neúspěchu vrací NULL)
 */
int insertNode(tNode* node , char* key, sData data){
    if ( compareKeys(key, node->key) == COMP_RIGHT)    //jestli je klic mensi vetsi nez klic nodu
    {
        if(node->rPtr != NULL){                             //kontrola pritomnosti potomku
            insertNode( node->rPtr, key, data);
        }
        else{
            node->rPtr = createNode(key, data);
            if(node->rPtr == NULL){
                return STATUS_INT_ERR;
            }
        } 
    }
    else if(compareKeys(key, node->key) == COMP_LEFT)
    {
        if(node->lPtr != NULL){
            insertNode(node->lPtr, key, data);
        }
        else{
            node->lPtr = createNode(key, data);
            if(node->lPtr == NULL){
                return STATUS_INT_ERR;
            }
        }
    }
    else if(compareKeys(key, node->key) == COMP_EQUALS)     //stejne klice == error tento else neni potreba teda, ci?
    {
        return STATUS_PROG_ERR;
    }
    return STATUS_OK;
}

/**
 * Rekurzivně volaný worker pro prohledávání binárních stromů
 * @param   tree    pointer na strom k prohledání
 * @param   query   hledaný výraz
 * 
 * @return  pointer na nalezený node nebo NULL, pokud nic nenajde
 */
tNode* searchWorker(tNode* tree, char* query) {
    if (tree == NULL) {
        return NULL;
    }
    switch(compareKeys(query, tree->key))
    {
        case COMP_EQUALS:
            return tree;
        break;
        case COMP_LEFT:
            if (tree->lPtr == NULL) {
                return NULL;
            } else {
                return searchWorker(tree->lPtr, query);
            }
        break;
        case COMP_RIGHT:
            if (tree->rPtr == NULL) {
                return NULL;
            } else {
                return searchWorker(tree->rPtr, query);
            }
        break;
    }
    return NULL;
}

/**
 * Vyhledávání v rámci hranic viditelnosti proměnných
 * @param   T       pointer na tabulku symbolů
 * @param   query   hledaný identifikátor
 * @param   result  pointer na strukturu výsledku
 * 
 * @return  status podle symtable.h
 */
void search(symTable* T, char* query, sResult* result) {
    if (strcmp(query, "_") == 0) {
        vData* empty = malloc(sizeof(vData));
        empty->index = 0;
        empty->name = "_";
        empty->type = VAR_UNIVERSAL;
        result->resultType = TYPE_VAR;
        result->data.var = empty;
        return;
    }
    sItem* scope; 
    if (T->cStack == NULL) {
        scope = NULL;
    } else {
        scope = T->cStack->sStack;
    }
    int rType = TYPE_NONE;
    tNode* workerRes;
    while (scope != NULL) {
        workerRes = searchWorker(scope->root, query);
        if (workerRes == NULL) {
            scope = scope->prev;
        } else {
            rType = TYPE_VAR;
            result->data.var = workerRes->data.var;
            break;
        }
    }
    if (rType == TYPE_NONE) {
        workerRes = searchWorker(T->funcTree, query);
        if (workerRes != NULL) {
            rType = TYPE_FUNC;
            result->data.func = workerRes->data.func;
        }
    }
    result->resultType = rType;
}

bool isFunc(symTable *T, char* query) {
    sResult r;
    search(T, query, &r);
    if (r.resultType == TYPE_FUNC) {
        return true;
    } else {
        return false;
    }
}

bool isVar(symTable *T, char* query, bool accept_) {
    if (strcmp(query, "_") == 0) { // vyhodnoceni specialne promenne _
        if (accept_) { 
            return true; // pokud nam _ nevadi, pak je validni
        }
        return false; // vratime false, coz vyvola sem. chybu 3
    }
    sResult r;
    search(T, query, &r);
    if (r.resultType == TYPE_VAR) {
        return true;
    } else {
        return false;
    }
}

fData* getFuncData(symTable *T, char* query) {
    sResult r;
    search(T, query, &r);
    if (r.resultType != TYPE_FUNC) {
        return NULL;
    } else {
        return r.data.func;
    }
}

vData* getVarData(symTable *T, char* query) {
    sResult r;
    search(T, query, &r);
    if (r.resultType != TYPE_VAR) {
        return NULL;
    } else {
        return r.data.var;
    }
}

/**
 * přemístíme se o scope výš
 * je nutno si uložit aktuální scope do pomocné proměnné a po volání fce scope pomoci pomocné proměnné obnovit
 * využito ve fci: findSameVar
 * @param T tabulka symbolů    
 * 
 * @return  stavový kód podle symtable.h
 */
int scopeUp(symTable *T){
    if(T->cStack->sStack->prev == NULL){
        return STATUS_INT_ERR;
    }
    T->cStack->sStack = T->cStack->sStack->prev;
    return STATUS_OK;
}

/**
 * přemístíme se o položku v callStacku výš
 * je nutno si uložit aktuální polohu v callStacku do pomocné proměnné a po volání fce polohu v callStacku pomoci pomocné proměnné obnovit
 * využito ve fci: findSameVar
 * @param T tabulka symbolů    
 * 
 * @return  stavový kód podle symtable.h
 */
int callStackUp(symTable *T){
    if(T->cStack->prev == NULL){
        return STATUS_INT_ERR;
    }
    T->cStack = T->cStack->prev;
    return STATUS_OK;
}

/**
 * hledání shody zadaného klíče s nějakým existujícím nodem ve stromu
 * využita ve fci" findSameVar
 * @param search    hledaný klíč
 * @param node      kořen stromu, ve kterém se vyhledává
 * 
 * @return          stavový kód podle symtable.h*/
int checkNodeExist(char* search, tNode* node){
    if(node == NULL){
        return NO_MATCH;
    }
    else if ( compareKeys(search, node->key) == COMP_RIGHT){    //jestli je klic mensi vetsi nez klic nodu
        if(node->rPtr != NULL){                             //kontrola pritomnosti potomku
            checkNodeExist(search, node->rPtr);
        }
    }
    else if(compareKeys(search, node->key) == COMP_LEFT){
        if(node->lPtr != NULL){                             //kontrola pritomnosti potomku
            checkNodeExist(search, node->lPtr);
        }
    }
    else if(compareKeys(search, node->key) == COMP_EQUALS)     //stejne klice == error
    {
        return COMP_EQUALS;
    }
    return NO_MATCH;

}

/**
 * funkce hledá zadaný klíč proměnné v zadaném stromě, a případně se postupně vzhledává ve vyšších scopech a callStacích
* @param T          tabulka symbolů
* @param node       zadaný kořen stromu, ve kterém se má začít vyhledávat
* @param search     hledaný klíč
*/
int findSameVar(symTable* T, tNode* node, char* search){
    sItem* saveActualScope = T->cStack->sStack;//nutno uložit aktualni polohu scopu a callstacku
    cItem* saveActualCallStack = T->cStack;
    if(checkNodeExist(search, node) == COMP_EQUALS){//prohledame zadany strom
        T->cStack->sStack = saveActualScope;//navraceni spravneho callStacku a scopu pred ukoncenim fce
        T->cStack = saveActualCallStack;
        return COMP_EQUALS;
    }
    //pokud v zadanem stromu nebyla nalezena shoda
    int found; //pomocna promenna pro srovnani vysledku fce nalezena shoda
    while(scopeUp(T) != STATUS_INT_ERR){//podminka: muzeme o scope vys?
        scopeUp(T);//ano - pujdeme o scope vys
        found = checkNodeExist(search, T->cStack->sStack->root);//zkontrolujeme strom noveho aktualniho scopu
            if (found == COMP_EQUALS){//nalezena shoda
            T->cStack->sStack = saveActualScope;
            T->cStack = saveActualCallStack;
            return COMP_EQUALS;
        }
    }//nemuzeme uz o scope vys
    //nenalezena shoda
    T->cStack->sStack = saveActualScope;
    T->cStack = saveActualCallStack;
    return NO_MATCH;
}

/**
 * zařazuje načtenou proměnnou do binárního stromu v aktuálním stromu
 * @param T         tabulka symbolů
 * @param key       vyhledávací klíč - jméno identifikátoru funkce/proměnné
 * @param vIndex    pořadí ve volání funkce
 * @param vType     typ proměnné
 * 
 * @return          stavový kód podle symtable.h
 */
int addVar(symTable *T, char* key, int vType, gen* g){
    
    if(T->cStack == NULL){//jestli neni zalozeny callStack - zalozime
        return STATUS_INT_ERR;
    }
    if(T->cStack->sStack == NULL){//jestli neni zalozeny scope - zalozime
        if(startScope(T) == STATUS_INT_ERR){
            return STATUS_INT_ERR;
        }
    }
    vData* newVarData = (vData*) malloc(sizeof(struct funcData)); //alokujeme strukturu promenne
    if(newVarData == NULL){
        return STATUS_INT_ERR;
    }
    newVarData->index = g->globalCounter++;
    newVarData->type = vType;
    newVarData->name = key;
    sData varD;
    varD.var = newVarData;
    if (T->cStack->sStack->root == NULL) { //pokud to bude nova polozka stromu
        T->cStack->sStack->root = createNode(key, varD);
        if(T->cStack->sStack->root == NULL){
            return STATUS_INT_ERR;
        }
        return STATUS_OK;
    } else {//pokud strom uz existuje
        int status = insertNode(T->cStack->sStack->root, key,varD);
        if (status == STATUS_PROG_ERR) {
            free(newVarData);
        }
        return status;  // distribuce návratové hodnoty vzhůru
    }
}

/**
 * přidává novou položku do binárního stromu fcí symTable->funcTree
 * @param T             pointer na tabulku symbolů
 * @param key           vyhledávací klíč - jméno identifikátoru funkce/proměnné
 * @param inArgNum      počet vstupních parametrů
 * @param outArgNum     počet výstupních parametrů
 * @param inArgType     pole s typy vstupních parametrů
 * @param outArgType    pole s typy výstupních parametrů
 * @return              stavový kód podle symtable.h
 */
int addFunc(symTable* T, char* key, int inArgNum, int outArgNum, int* inArgType, int* outArgType){
    
    fData* newFuncData = (fData*)malloc(sizeof(struct funcData));
    if(newFuncData == NULL){
        return STATUS_INT_ERR;
    }
    newFuncData->inArgC = inArgNum;                     //prirazeni hodnot
    newFuncData->outArgC = outArgNum;
    newFuncData->inArgTypes = inArgType;
    newFuncData->outArgTypes = outArgType;
    sData funcD;
    funcD.func = newFuncData;

    if(T->funcTree == NULL){
        T->funcTree = createNode(key, funcD);               //pridavase od korene stromu
        if (T->funcTree == NULL) {
            return STATUS_INT_ERR;
        }
        return STATUS_OK;
    } else {  //pokud strom existuje
        int status = insertNode(T->funcTree, key, funcD);
        if (status == STATUS_PROG_ERR) {
            free(newFuncData);
        }
        return status;               //prohledava se /pridavase od korene stromu
    }
}

/**
 * inicializace tabulky symbolů
 * @param T  pointer na tabulku symbolů
 */
void initSymTable (symTable *T){
    T->funcTree = NULL;
    T->cStack = NULL; 
}

/**
 * Destrukce tabulky symbolů
 * @param T  pointer na tabulku symbolů
 */
void destroySymTable(symTable *T){
    deleteFuncTree(T->funcTree);
    cItem* cStackWorker = T->cStack;
    while(cStackWorker != NULL) {
        endCall(T);
        cStackWorker = T->cStack;
    }
}

/**
 * vytváří novou položku v zásobníku volání fcí
 * @param T  pointer na tabulku symbolů
 * 
 * @return   STATUS_OK
 * @return   STATUS_INT_ERR při chybě alokace
 * @return   STATUS_PROG_ERR při pokusu zavolat nedefinovanou funkci
 */
int startCall(symTable *T, char* identifier){ 
    sResult s;
    search(T, identifier, &s);
    if (s.resultType != TYPE_FUNC) {
        return STATUS_PROG_ERR;
    } 
    cItem* new = (cItem*) malloc(sizeof(struct callStackItem));
    if (new == NULL){
        return STATUS_INT_ERR;
    }
    new->prev = T->cStack;
    new->sStack = NULL;
    new->data = s.data.func;
    T->cStack = new;    //posunuti vrcholu zasobniku
    return STATUS_OK;
}


void getCall(symTable* T, fData** result) {
    if (T->cStack != NULL) {
        *result = T->cStack->data;
    } else {
        *result = NULL;
    }
}

/**
 * vytváří novou položku v zásobníku rámců
 * @param T  pointer na tabulku symbolů
 * 
 * @return   stavový kód podle symtable.h
 */
int startScope (symTable *T){
    if(T->cStack == NULL){  //pokud je zasobnik volani prazdny - zalozime
        return STATUS_PROG_ERR;
    } else {   
        sItem* new = (sItem*) malloc(sizeof(struct scopeStackItem));
        if (new == NULL){
            return STATUS_INT_ERR;
        }
        new->prev = T->cStack->sStack;
        new->root = NULL;
        T->cStack->sStack = new;
    }
    return STATUS_OK;
}

/**
 * ruší strom proměnných
 * @param T pointer na kořen stromu
 */
void deleteVarTree(tNode* n){
    if(n != NULL){
        deleteVarTree(n->lPtr);
        deleteVarTree(n->rPtr);
        free(n->data.var);
        free(n);
    }
}

/**
 * ruší strom funkcí
 * @param T pointer na kořen stromu
 */
void deleteFuncTree(tNode* n) {
    if(n != NULL){
        deleteFuncTree(n->lPtr);
        deleteFuncTree(n->rPtr);
        free(n->data.func->inArgTypes);
        free(n->data.func->outArgTypes);
        free(n->data.func);
        free(n);
    }
}

/**
 * fce na odstranění scopu
 * 
 * @param T tabulka symbolů
 * 
 * @return  stavový kód podle symtable.h
 */
int endScope(symTable *T) {
    if (T->cStack->sStack == NULL) {
        return STATUS_INT_ERR;
    }
    deleteVarTree(T->cStack->sStack->root);
    sItem* tmp = T->cStack->sStack;
    T->cStack->sStack = tmp->prev;
    free(tmp);
    return STATUS_OK;
}

/**
 * ukončení volání funkce
 * @param T     pointer na tabulku symbolů
 * 
 * @return      status podle symtable.h
 */
int endCall(symTable* T){
    if (T->cStack == NULL) {
        return STATUS_INT_ERR;
    }
    sItem* scopePtr = T->cStack->sStack;
    while(scopePtr != NULL) {
        deleteVarTree(scopePtr->root);
        sItem* tmp = scopePtr;
        scopePtr = scopePtr->prev;
        free(tmp);
    }
    cItem* tmp = T->cStack;
    T->cStack = T->cStack->prev;
    free(tmp);
    return STATUS_OK;
}
