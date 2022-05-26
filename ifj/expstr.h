/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file expstr.h
 * @brief Knihovna pro práci s rostoucími řetězci textu
 * @author Jakub Hlava (xhlava52)
 * @date 10/2020
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef _EXPSTR_H
#define _EXPSTR_H

#define START_SIZE 64 // počáteční velikost alokované paměti pro řetězec
#define EXPAND_RATE 1.5 // násobitel zvětšování pole po naplnění kapacity

#define EXPSTR_ERR_UNINITIALIZED -1
#define EXPSTR_ERR_REALLOC 100

typedef struct ExpandingString {
    uint32_t len; /// Aktuální velikost alokované paměti/maximální délka řetězce
    uint32_t termPos; /// Pozice, na které leži ukončovací znak/aktuální délka řetězce
    char *data; /// Ukazatel na paměť, kterou má řetězec právě alokovanou
} expStr;

typedef struct TypeArray {
    int* array;
    uint32_t len;
} typeArr;

int initEStr(expStr *str); // Inicializace expandujícího řetězce
int appendEStr(expStr *str, char c); // Přidání charu na konec řetězce
int extendEStr(expStr *str, char *ext); // Připojení pole znaků na konec expStr řetězce
char *copyEStr(expStr *str); // Zkopírování expandujícího řetězce do pevného pole charů
uint64_t convEStr2Int(expStr *str); // Konverze expandujícího řetězce na int pro účely uložení do tokenu
double convEStr2Double(expStr *str); // Konverze expandujícího řetězce na double pro účely uložení do tokenu
int compEStr(expStr *str1, char *str2); // porovnání expandujícího řetězce s pevným
int expandEStr(expStr *str); // Zvětší velikost expandujícího řetězce EXPAND_RATE-krát
int mExpandEStr(expStr *str, unsigned int min); // Zvětší velikost expandujícího řetězce minimálně o velikost min
void destroyEStr(expStr *str); // Dealokuje a zničí expandující řetězec
void clearEStr(expStr *str); // Vyčístí data expStr posunutím terminátoru na 0. pozici

void initTA(typeArr* ta);
int appendTA(typeArr* ta, int type);
void destroyTA(typeArr* ta);

#endif
