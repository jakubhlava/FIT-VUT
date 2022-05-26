/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file ifj20c.h
 * @brief Hlavní modul překladače pro jazyk IFJ20
 * @author Jakub Hlava (xhlava52)
 * @author Thi Bao Ngoc Vu (xvuthi00)
 * @author Lukáš Kraus (xkraus13)
 * @author Jan Pryč (xprycj00)
 * @date 10/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include "syntactic.h"
#include "symtable.h"
#include "fwd.h"

#define INARGS(name, ...)  int tempin_##name[] = {__VA_ARGS__}
#define OUTARGS(name, ...)  int tempout_##name[] = {__VA_ARGS__}
#define BUILTIN(name, incount, outcount) int* in_##name = malloc(sizeof(int)*incount); memcpy(in_##name, tempin_##name, sizeof(int)*incount); int* out_##name = malloc(sizeof(int)*outcount); memcpy(out_##name, tempout_##name, sizeof(int)*outcount); addFunc(table, #name, incount, outcount, in_##name, out_##name)
#define BUILTIN_NOARG(name, outcount) int* out_##name = malloc(sizeof(int)*outcount); memcpy(out_##name, tempout_##name, sizeof(int)*outcount); addFunc(table, #name, 0, outcount, NULL, out_##name)
#define BUILTIN_NORET(name, incount) int* in_##name = malloc(sizeof(int)*incount); memcpy(in_##name, tempin_##name, sizeof(int)*incount); addFunc(table, #name, incount, 0, in_##name, NULL)

void prefill(symTable* ST);
int fillCache(tCache* tokenCache);
int firstPass(symTable* ST, tCache* tokenCache);
int compile(symTable* ST, tCache* tokenCache, gen* g, gen* b, gen *rv);
