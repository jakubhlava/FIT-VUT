/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file codegen.c
 * @brief Pomocné funkce pro generování kódu v jazyce IFJcode20
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 * @date 11/2020
 */

#include "codegen.h"
/**
 * @brief Funkce pro převod float konstanty na formát pro jazyk IFJcode20
 * JE NUTNÉ UVOLNIT PAMĚT PO char*
 * @param   tok     token pro převod
 * @param   out     pointer na výstupní buffer
 * @return  CODEGEN_RET_SUCCESS     při úspěchu
 * @return  CODEGEN_ERR_ALLOC       při chybě mallocu
 */
int convertConst(token* tok, char** out) {
    if (tok->tokenType == TYPE_FLOAT64) {
        *out = malloc(sizeof(char)*24);
        if (*out == NULL) {
            return CODEGEN_ERR_ALLOC;
        }
        snprintf(*out, 24, "%a", tok->attr.floatAttr);
        return CODEGEN_RET_SUCCESS;
    }
    return CODEGEN_ERR_ALLOC;
}

void init_gen(gen* g) {
    g->st = NULL;
    g->globalCounter = 0;
    g->rightSideTemp = 0;
    g->v = malloc(sizeof(vStack));
    g->params = malloc(sizeof(aStack));
}

void destroyGen(gen* g) {
    destroyVStack(g->v);
    free(g->v);
    destroyAStack(g->params);
    free(g->params);   
}

/**
 * Inicializuje zásobník tokenů k tisku
 */
void initPQueue(pQueue *s) {
    s->top = NULL;
    s->head = NULL;
}

/**
 * Připojuje token na zásobník tokenů k tisku
 */
int printAppend(pQueue* s, token* tok) {
    if (s->top == NULL) {
        s->top = malloc(sizeof(pQueueItem));
        s->head = s->top;
        s->top->tok = tok;
        s->top->next = NULL;
        s->top->prev = NULL;
    } else {
        pQueueItem* new = malloc(sizeof(pQueueItem));
        new->prev = s->head;
        new->next = NULL;
        new->tok = tok;
        s->head->next = new;
        s->head = new;
    }
    return CODEGEN_RET_SUCCESS;
}

/**
 * Vyčistí zásobník tokenů po použití
 */
void destroyPQueue(pQueue *s) {
    pQueueItem* worker = s->top;
    pQueueItem* tmp = worker;
    while(worker!=NULL) {
        tmp = worker;
        worker = worker->next;
        free(tmp);
    }
    s->top = NULL;
    s->head = NULL;
}


/**
 * Generuje hlavičku programu
 */
void gen_header() {
    printf(".IFJcode20\n");
    printf("JUMP $main\n");
}

/**
 * Generuje hlavičku mainu
 */
void gen_mainheader() {
    printf("LABEL $main\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

/**
 * Generuje hlavičku funkce
 */
void gen_func_header(char* identifier, fData* func) {
    printf("LABEL $%s\n", identifier);
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    for (int i = 0; i < func->inArgC; i++) {
        printf("DEFVAR LF@$%s$%i\n",identifier,i);
    }
}

int gen_temp_var(gen* g) {
    printf("DEFVAR LF@$temps$%d\n",g->globalCounter);
    return g->globalCounter++;
}

void gen_temp_set_from_tok(gen* g, int id, token* src) {
    char* temp = NULL;
    sResult r;
    switch (src->tokenType) {
        case TYPE_STRING:
            printf("MOVE LF@$temps$%d string@%s\n", id ,src->attr.strAttr);
        break;
        case TYPE_INT:
            printf("MOVE LF@$temps$%d int@%ld\n", id,src->attr.intAttr);
        break;
        case TYPE_FLOAT64:
            convertConst(src, &temp);
            printf("MOVE LF@$temps$%d float@%s\n", id,temp);
            free(temp);
            temp = NULL;
        break;
        case TYPE_IDENTIFIER:
            search(g->st, src->attr.strAttr, &r);
            if (r.resultType == TYPE_VAR) {
                printf("MOVE LF@$temps$%d LF@%s_%d\n", id,r.data.var->name, r.data.var->index);
            }
        break;
    }
}

/**
 * Generuje deklaraci proměnné a prvotní přiřazení do ní
 */
void gen_declaration(vData* var, int tempVarId) { 
    if (strcmp(var->name, "_") == 0) {
        return;
    }
    printf("DEFVAR LF@%s_%d\n",var->name, var->index);
    printf("MOVE LF@%s_%d LF@$temps$%d\n",var->name, var->index, tempVarId);
}

void gen_set_from_temp(vData* var, int tempVarId) {
    if (strcmp(var->name, "_") == 0) {
        return;
    }
    printf("MOVE LF@%s_%d LF@$temps$%d\n",var->name, var->index, tempVarId);
}


/**
 * Generuje jednoduché přiřazení do proměnné (bez vyhodnocení funkce)
 * Pro vícenásobné přiřazení je potřeba volat vícenásobně
 */
void gen_simple_set(vData* var, token* right, symTable* st) {
    char* temp = NULL;
    sResult r;
    if (strcmp(var->name, "_") == 0) {
        return;
    }
    switch (right->tokenType) {
        case TYPE_STRING:
            printf("MOVE LF@%s_%d string@%s\n",var->name, var->index ,right->attr.strAttr);
        break;
        case TYPE_INT:
            printf("MOVE LF@%s_%d int@%ld\n",var->name, var->index,right->attr.intAttr);
        break;
        case TYPE_FLOAT64:
            convertConst(right, &temp);
            printf("MOVE LF@%s_%d float@%s\n",var->name, var->index,temp);
            free(temp);
            temp = NULL;
        break;
        case TYPE_IDENTIFIER:
            search(st, right->attr.strAttr, &r);
            if (r.resultType == TYPE_VAR) {
                printf("MOVE LF@%s_%d LF@%s_%d\n",var->name, var->index,r.data.var->name, r.data.var->index);
            }
        break;
    }
}

/**
 * Generuje načítání od uživatele - příkazy inputi, inputs a inputf
 */
void gen_input(int type, vData* var, vData* err, gen* g) {
    char* convType;
    switch (type) {
        case VAR_INT:
            convType = "int";
        break;
        case VAR_STRING:
            convType = "string";
        break;
        case VAR_FLOAT64:
            convType = "float";
        break;
    }
    int tempid;
    if (var->type == VAR_UNIVERSAL) {
        tempid = gen_temp_var(g);
        printf("READ LF@$temps$%d %s\n",tempid,convType);
    } else {
        printf("READ LF@%s_%d %s\n",var->name,var->index,convType);
    }
    if (err->type != VAR_UNIVERSAL) {
        int checkid = g->globalCounter++;
        if (var->type == VAR_UNIVERSAL) {
            printf("JUMPIFNEQ $check$%d nil@nil LF@$temps$%d\n",checkid,tempid);
            free(var);
        } else {
            printf("JUMPIFNEQ $check$%d nil@nil LF@%s_%d\n",checkid,var->name,var->index);
        }
        printf("MOVE LF@%s_%d int@1\n", err->name, err->index);
        printf("LABEL $check$%d\n",checkid);
    } else {
        if (var->type == VAR_UNIVERSAL) {
            free(var);
        }
        free(err);
    }
}

/**
 * Generuje reprezentaci příkazu print
 */
void gen_print(pQueue* s, symTable* st) {
    char* temp = NULL;
    sResult r;
    pQueueItem* worker = s->top;
    while(worker != NULL) {
        switch (worker->tok->tokenType) {
            case TYPE_STRING:
                printf("WRITE string@%s\n",worker->tok->attr.strAttr);
            break;
            case TYPE_INT:
                printf("WRITE int@%ld\n",worker->tok->attr.intAttr);
            break;
            case TYPE_FLOAT64:
                convertConst(worker->tok, &temp);
                printf("WRITE float@%s\n",temp);
                free(temp);
                temp = NULL;
            break;
            case TYPE_IDENTIFIER:
                search(st, worker->tok->attr.strAttr, &r);
                if (r.resultType == TYPE_VAR) {
                    printf("WRITE LF@%s_%d\n",r.data.var->name, r.data.var->index);
                }
            break;
        }
        worker = worker->next;
    }
}

/**
 * Generuje return
 */
void gen_return(bool main) {
    printf("POPFRAME\n");
    if (main) {
        printf("EXIT int@0\n");
    } else {
        printf("RETURN\n");
    }
    
}


/**
 * Generuje int2float na základě parametrů předaných v struktuře generátoru
 */
int gen_int2float(gen* g) {
    int type;
    void* in = popAStack(g->params, &type);
    vData* out = popVStack(g->v);
    if (type == ARG_VAR) {
        vData* inVar = (vData*)in;
        if (out->type != VAR_FLOAT64 || inVar->type != VAR_INT) {
        return SEM_ERR_FUNC;
        }
        if (strcmp(inVar->name, "_") == 0) {
            free(inVar);
            return CODEGEN_RET_SUCCESS;
        }
        printf("INT2FLOAT LF@%s_%d LF@%s_%d\n",out->name, out->index, inVar->name, inVar->index);
    } else if (type == ARG_TOKEN) {
        token* inTok = (token*)in;
        if (out->type != VAR_FLOAT64 || inTok->tokenType != TYPE_INT) {
            return SEM_ERR_FUNC;
        }
        printf("INT2FLOAT LF@%s_%d int@%ld\n",out->name, out->index, inTok->attr.intAttr);
    }
    
    return CODEGEN_RET_SUCCESS;
}

/**
 * Generuje float2int na základě parametrů ve struktuře generátoru
 */
int gen_float2int(gen* g) {
    int type;
    void* in = popAStack(g->params, &type);
    vData* out = popVStack(g->v);
    if (type == ARG_VAR) {
        vData* inVar = (vData*)in;
        if (out->type != VAR_INT || inVar->type != VAR_FLOAT64) {
        return SEM_ERR_FUNC;
        }
        if (strcmp(inVar->name, "_") == 0) {
            free(inVar);
            return CODEGEN_RET_SUCCESS;
        }
        printf("FLOAT2INT LF@%s_%d LF@%s_%d\n",out->name, out->index, inVar->name, inVar->index);
    } else if (type == ARG_TOKEN) {
        token* inTok = (token*)in;
        if (out->type != TYPE_INT || inTok->tokenType != VAR_FLOAT64) {
            return SEM_ERR_FUNC;
        }
        char* temp;
        convertConst(inTok, &temp);
        printf("FLOAT2INT LF@%s_%d float@%s\n",out->name, out->index, temp);
        free(temp);
    }
    
    return CODEGEN_RET_SUCCESS;
}

/**
 * Převede operátor na instrukci
 */
char* decodeInstr(int op) {
    switch (op) {
        case TYPE_OP_ADD:
            return "ADD";
        case TYPE_OP_SUB:
            return "SUB";
        case TYPE_OP_MUL:
            return "MUL";
        case TYPE_OP_DIV:
            return "DIV";
        case TYPE_OP_LESS:
            return "LT";
        case TYPE_OP_GREATER:
            return "GT";
        case TYPE_OP_EQUALS:
            return "EQ";
        default:
            return "";
     }
}

int gen_calc(gen* g, precItem* a, precItem* b, int op) {
    char* instr;
    instr = decodeInstr(op);
    if (a->dataType == TYPE_INT) {
        if (op == TYPE_OP_DIV) {
            instr = "IDIV";
        } 
    } else if (a->dataType == TYPE_IDENTIFIER) {
        vData* v = getVarData(g->st, ((token*)(a->item))->attr.strAttr);
        if (v->type == TYPE_INT && op == TYPE_OP_DIV) {
            instr = "IDIV";
        } else if (v->type == TYPE_STRING) {
            instr = "CONCAT";
        }
    } else if (a->dataType == TYPE_STRING) {
        instr = "CONCAT";
    }
    int retId = gen_temp_var(g);
    token* tok;
    char* temp;
    printf("%s LF@$temps$%d ",instr,retId);
    vData* v;
    int* id;
    switch (a->itemType) {
        case ARG_TOKEN:
            tok = (token*)(a->item);
            switch (tok->tokenType) {
                case TYPE_INT:
                    printf("int@%ld ",tok->attr.intAttr);
                    break;
                case TYPE_STRING:
                    printf("string@%s ",tok->attr.strAttr);
                    break;
                case TYPE_FLOAT64:
                    convertConst(tok, &temp);
                    printf("float@%a ",tok->attr.floatAttr);
                    free(temp);
                    break;
                case TYPE_IDENTIFIER:
                    v = getVarData(g->st, tok->attr.strAttr);
                    printf("LF@%s_%d ",v->name,v->index);
                    break;
            }
            break;
        case ARG_ID:
            id = (int*)(a->item);
            printf("LF@$temps$%d ",*id);
        break;
    }
    switch (b->itemType) {
        case ARG_TOKEN:
            tok = (token*)(b->item);
            switch (tok->tokenType) {
                case TYPE_INT:
                    printf("int@%ld\n",tok->attr.intAttr);
                    break;
                case TYPE_STRING:
                    printf("string@%s\n",tok->attr.strAttr);
                    break;
                case TYPE_FLOAT64:
                    convertConst(tok, &temp);
                    printf("float@%a\n",tok->attr.floatAttr);
                    free(temp);
                    break;
                case TYPE_IDENTIFIER:
                    v = getVarData(g->st, tok->attr.strAttr);
                    printf("LF@%s_%d\n",v->name,v->index);
                    break;
            }
        break;
        case ARG_ID:
            id = (int*)(a->item);
            printf("LF@$temps$%d\n",*id);
        break;
    }
    return retId;
}

int gen_noncalc(gen* g, precItem* a, precItem* b, int op) {
    int retId = gen_temp_var(g);
    token* tok;
    char* temp;
    vData* v;
    int* id;
    for (int i = 0; i < 2; i++) {
        printf("PUSHS ");
        switch (a->itemType) {
            case ARG_TOKEN:
                tok = (token*)(a->item);
                switch (tok->tokenType) {
                    case TYPE_INT:
                        printf("int@%ld\n",tok->attr.intAttr);
                        break;
                    case TYPE_STRING:
                        printf("string@%s\n",tok->attr.strAttr);
                        break;
                    case TYPE_FLOAT64:
                        convertConst(tok, &temp);
                        printf("float@%a\n",tok->attr.floatAttr);
                        free(temp);
                        break;
                    case TYPE_IDENTIFIER:
                        v = getVarData(g->st, tok->attr.strAttr);
                        printf("LF@%s_%d\n",v->name,v->index);
                        break;
                }
                break;
            case ARG_ID:
                id = (int*)(a->item);
                printf("LF@$temps$%d\n",*id);
            break;
        }
        printf("PUSHS ");
        switch (b->itemType) {
            case ARG_TOKEN:
                tok = (token*)(b->item);
                switch (tok->tokenType) {
                    case TYPE_INT:
                        printf("int@%ld\n",tok->attr.intAttr);
                        break;
                    case TYPE_STRING:
                        printf("string@%s\n",tok->attr.strAttr);
                        break;
                    case TYPE_FLOAT64:
                        convertConst(tok, &temp);
                        printf("float@%a\n",tok->attr.floatAttr);
                        free(temp);
                        break;
                    case TYPE_IDENTIFIER:
                        v = getVarData(g->st, tok->attr.strAttr);
                        printf("LF@%s_%d\n",v->name,v->index);
                        break;
                }
            break;
            case ARG_ID:
                id = (int*)(a->item);
                printf("LF@$temps$%d\n",*id);
            break;
        }
        switch (op) {
            case TYPE_OP_GREATER_EQ:
                if (i == 0) {
                    printf("GTS\n");
                } else {
                    printf("EQS\n");
                }
                break;
            case TYPE_OP_LESS_EQ:
                if (i == 0) {
                    printf("LTS\n");
                } else {
                    printf("EQS\n");
                }
                break;
            case TYPE_OP_NOT_EQ:
                if (i == 1) {
                    printf("EQS\n");
                } 
                break;
        }
    }
    if (op == TYPE_OP_GREATER_EQ || op == TYPE_OP_LESS_EQ) {
        printf("ORS\n");
        printf("POPS LF@$temps$%d\n",retId);
    } else if (op == TYPE_OP_NOT_EQ) {
        printf("NOTS\n");
        printf("POPS LF@$temps$%d\n",retId);
        printf("CLEARS\n");
    }
    return retId;
}



/**
 * Generuje funkci len
 */
int gen_len(gen* g) {
    int type;
    void* in = popAStack(g->params, &type);
    vData* out = popVStack(g->v);
    if (type == ARG_VAR) {
        vData* inVar = (vData*)in;
        if (strcmp(inVar->name, "_") == 0) {
            free(inVar);
            return CODEGEN_RET_SUCCESS;
        }
        printf("STRLEN LF@%s_%d LF@%s_%d\n",out->name, out->index, inVar->name, inVar->index);
    } else if (type == ARG_TOKEN) {
        token* inTok = (token*)in;
        printf("STRLEN LF@%s_%d string@%s",out->name, out->index, inTok->attr.strAttr);
    }
    return CODEGEN_RET_SUCCESS;
}

/**
 * Generuje funkci ord 
 */
int gen_ord(gen* g) {
    vData* err = popVStack(g->v);
    vData* output = popVStack(g->v);
    if (!err || !output) {
        return SEM_ERR_FUNC;
    }
    int posType = 0, strType = 0;
    void* pos = popAStack(g->params, &posType);
    void* str = popAStack(g->params, &strType);
    int postemp = gen_temp_var(g);
    int strtemp = gen_temp_var(g);
    switch (posType) {
        case ARG_TOKEN:
            printf("MOVE LF@$temps$%d int@%ld\n",postemp, ((token*)pos)->attr.intAttr);
        break;
        case ARG_VAR:
            printf("MOVE LF@$temps$%d LF@%s_%d\n",postemp, ((vData*)pos)->name, ((vData*)pos)->index);
        break;
    }
    switch (strType) {
        case ARG_TOKEN:
            printf("MOVE LF@$temps$%d string@%s\n",strtemp, ((token*)str)->attr.strAttr);
        break;
        case ARG_VAR:
            printf("MOVE LF@$temps$%d LF@%s_%d\n",strtemp, ((vData*)str)->name, ((vData*)str)->index);
        break;
    }
    int len = gen_temp_var(g);
    printf("STRLEN LF@$temps$%d LF@$temps$%d\n",len,strtemp);
    int less = gen_temp_var(g);
    printf("LT LF@$temps$%d LF@$temps$%d LF@$temps$%d\n", less, len, postemp);
    int jump = g->globalCounter++;
    printf("JUMPIFEQ $comp$%d LF@$temps$%d bool@true\n", jump, less);
    if (strcmp(err->name, "_") != 0) {
        printf("MOVE LF@%s_%d int@1\n", err->name, err->index);
    }
    if (strcmp(output->name, "_") != 0) {
        printf("STRI2INT LF@%s_%d LF@$temps$%d LF@$temps$%d\n", output->name, output->index, strtemp, postemp);
    }
    printf("LABEL $comp$%d\n", jump);
    return CODEGEN_RET_SUCCESS;
}

/**
 * Generování funkce chr
 */
int gen_chr(gen* g) {
    vData* err = popVStack(g->v);
    vData* output = popVStack(g->v);
    if (!err || !output) {
        return SEM_ERR_FUNC;
    }
    int type;
    void* ascii = popAStack(g->params, &type);
    int temp = gen_temp_var(g);
    switch (type) {
        case ARG_TOKEN:
            printf("MOVE LF@$temps$%d int@%ld\n", temp, ((token*)ascii)->attr.intAttr);
            break;
        case ARG_VAR:
            printf("MOVE LF@$temps$%d LF@%s_%d\n", temp, ((vData*)ascii)->name, ((vData*)ascii)->index);
            break;
    }
    int less = gen_temp_var(g), greater = gen_temp_var(g);
    printf("LT LF@$temps$%d LF@$temps$%d int@0\n", less, temp);
    printf("GT LF@$temps$%d LF@$temps$%d int@255\n", greater, temp);
    int jump = g->globalCounter++;
    printf("JUMPIFEQ $comp$%d LF@$temps$%d bool@false\n", jump, less);
    printf("JUMPIFEQ $comp$%d LF@$temps$%d bool@false\n", jump, greater);
    if (strcmp(err->name, "_") != 0) {
        printf("MOVE LF@%s_%d int@1\n",err->name,err->index);
    }
    if (strcmp(output->name, "_") != 0) {
        printf("INT2CHAR LF@%s_%d LF@$temps$%d\n", output->name, output->index, temp);
    }
    printf("LABEL $comp$%d\n",jump);
    return CODEGEN_RET_SUCCESS;
}

/**
 * Generování funkce substr
 */
int gen_substr(gen* g) {
    vData* err = popVStack(g->v);
    vData* out = popVStack(g->v);
    if (!err || !out) {
        return SEM_ERR_FUNC;
    }
    int lenType;
    void* len = popAStack(g->params, &lenType);
    int startType;
    void* start = popAStack(g->params, &startType);
    int strType;
    void* string = popAStack(g->params, &strType);
    int lentemp = gen_temp_var(g);
    int starttemp = gen_temp_var(g);
    int stringtemp = gen_temp_var(g);
    switch(lenType) {
        case ARG_TOKEN:
            printf("MOVE LF@$temps$%d int@%ld\n",lentemp, ((token*)len)->attr.intAttr);
        break;
        case ARG_VAR:
            printf("MOVE LF@$temps$%d LF@%s_%d\n",lentemp, ((vData*)len)->name, ((vData*)len)->index);
        break;
    }
    switch(startType) {
        case ARG_TOKEN:
            printf("MOVE LF@$temps$%d int@%ld\n",starttemp, ((token*)start)->attr.intAttr);
        break;
        case ARG_VAR:
            printf("MOVE LF@$temps$%d LF@%s_%d\n",starttemp, ((vData*)start)->name, ((vData*)start)->index);
        break;
    }
    switch(strType) {
        case ARG_TOKEN:
            printf("MOVE LF@$temps$%d string@%s\n",stringtemp, ((token*)string)->attr.strAttr);
        break;
        case ARG_VAR:
            printf("MOVE LF@$temps$%d LF@%s_%d\n",stringtemp, ((vData*)string)->name, ((vData*)string)->index);
        break;
    }
    if (strcmp(err->name, "_") != 0) {
        int reallen = gen_temp_var(g);
        printf("STRLEN LF@$temps$%d LF@$temps$%d\n",reallen, stringtemp);
        int lt0 = gen_temp_var(g);
        printf("LT LF@$temps$%d LF@$temps$%d int@0\n", lt0, starttemp);
        int gtrl = gen_temp_var(g);
        printf("GT LF@$temps$%d LF@$temps$%d LF@$temps$%d\n", gtrl, starttemp, reallen);
        int cl0 = gen_temp_var(g);
        printf("LT LF@$temps$%d LF@$temps$%d int@0\n", cl0, lentemp);
        int errJump = g->globalCounter++;
        printf("JUMPIFNEQ $comp$%d LF@$temps$%d bool@true\n", errJump, lt0);
        printf("JUMPIFNEQ $comp$%d LF@$temps$%d bool@true\n", errJump, gtrl);
        printf("JUMPIFNEQ $comp$%d LF@$temps$%d bool@true\n", errJump, cl0);
        printf("MOVE LF@%s_%d int@1\n",err->name, err->index);
        printf("LABEL $comp$%d\n", errJump);
    }
    if (strcmp(out->name, "_") != 0) {
        
    }
    return CODEGEN_RET_SUCCESS;
}

/**
 * Pomocné stacky
 */
void initVStack(vStack* v) {
    v->first = NULL;
    v->head = NULL;
}

void destroyVStack(vStack* v) {
    while (popVStack(v) != NULL);
}

int appendVStack(vStack* v, vData* data) {
    vStackItem* new = malloc(sizeof(vStackItem));
    if (new) {
        new->var = data;
        new->ptrNext = NULL;
    } else {
        return CODEGEN_ERR_ALLOC;
    }
    if (v->first == NULL) {
        v->first = new;
        new->ptrBack = NULL;
    } else {
        v->head->ptrNext = new;
        new->ptrBack = v->head;
    }
    v->head = new;
    return CODEGEN_RET_SUCCESS;
}

vData* popVStack(vStack* v) {
    if (v) {
        if (v->head) {
            vData* ret = v->head->var;
            vStackItem* popped = v->head;
            if (v->head == v->first) {
                v->first = NULL;
            }
            v->head = v->head->ptrBack;
            free(popped);
            return ret;
        }
    }
    return NULL;
}


/**
 * Stack (primárně na argumenty funkcí)
 */

void initAStack(aStack* a) { /// inicializace na null
    a->first = NULL;
    a->head = NULL;
}

void destroyAStack(aStack* a) { /// mělo by stack korektně vynulovat a uvlonit paměť
    int throwaway;
    while (popAStack(a, &throwaway) != NULL);
}

/**
 * Připojování prvku na konec aStacku
 * @param   data    pointer na token nebo vData, která ukládáme na zásobník
 * @param   type    ARG_TOKEN nebo ARG_VAR podle toho, co do stacku uládáme, abychom byli schopni korektního přetypování po vyjmutí prvku ze stacku
 */
int appendAStack(aStack* a, void* data, int type) {
    aStackItem* new = malloc(sizeof(aStackItem));
    if (new) {
        argData arg;
        switch(type) {
            case ARG_TOKEN:
            arg.tok = (token*)data;
            break;
            case ARG_VAR:
            arg.var = (vData*)data;
            break;
            default:
            return ERR_INTERNAL;
        }
        new->data = arg;
        new->ptrNext = NULL;
        new->type = type;
    } else {
        return CODEGEN_ERR_ALLOC;
    }
    if (a->first == NULL) {
        a->first = new;
        new->ptrBack = NULL;
    } else {
        a->head->ptrNext = new;
        new->ptrBack = a->head;
    }
    a->head = new;
    return CODEGEN_RET_SUCCESS;
}

/**
 * Vybrání posledního prvku ze zásobníku
 * @param retType   pointer na integer ve volající funkci, do kterého je uložena hodnota ARG_TOKEN nebo ARG_VAR
 *                  podle typu vráceného pointeru, aby bylo možné jej korektně přetypovat
 */
void* popAStack(aStack* a, int* retType) {
    if (a) {
        if (a->head) {
            void* ret = NULL;
            switch (a->head->type) {
                case ARG_TOKEN:
                    ret = a->head->data.tok;
                break;
                case ARG_VAR:
                    ret = a->head->data.var;
                break;
            }
            *retType = a->head->type;
            aStackItem* popped = a->head;
            if (a->head == a->first) {
                a->first = NULL;
            }
            a->head = a->head->ptrBack;
            free(popped);
            return ret;
        }
    }
    return NULL;
}