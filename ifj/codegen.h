/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file codegen.h
 * @brief Pomocné funkce pro generování kódu v jazyce IFJcode20
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 * @date 11/2020
 */


#ifndef _CODEGEN_HEADER
#define _CODEGEN_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <stdbool.h>
#include "lexical.h"
#include "symtable.h"
#include "fwd.h"


void initPQueue(pQueue *s);
int printAppend(pQueue* s, token* tok);
void destroyPQueue(pQueue *s);

void initVStack(vStack* v);
void destroyVStack(vStack* v);
int appendVStack(vStack* v, vData* data);
vData* popVStack(vStack* v);

void initAStack(aStack* a);
void destroyAStack(aStack* a);
int appendAStack(aStack* a, void* data, int type);
void* popAStack(aStack* a, int* retType);

void gen_header();
void gen_mainheader();
void gen_func_header(char* identifier, fData* func);
void gen_declaration(vData* var, int tempVarId);
void gen_simple_set(vData* var, token* right, symTable* st);
void init_gen(gen* g);
void destroyGen(gen* g);
void gen_footer();      // spuštění funkce main
void gen_input(int type, vData* var, vData* err, gen* g);
void gen_print(pQueue *s, symTable *st);
void gen_return(bool main);
int gen_temp_var(gen* g);
void gen_temp_set_from_tok(gen* g, int id, token* src);
int gen_int2float(gen* g);
int gen_float2int(gen* g);
int gen_len(gen* g);
int gen_ord(gen* g);
int gen_chr(gen* g);
int gen_substr(gen* g);


char* decodeInstr(int op);
int gen_calc(gen* g, precItem* a, precItem* b, int op);
int gen_noncalc(gen* g, precItem* a, precItem* b, int op);
void gen_set_from_temp(vData* var, int tempVarId);

int convertConst(token* tok, char** out);

#endif