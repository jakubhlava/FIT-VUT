/**
 * Implementace překladače imperativního jazyka IFJ20
 * @file lexical.h 
 * @brief Lexikální analyzátor pro jazyk IFJ20
 * @author Jakub Hlava (xhlava52)
 * @date 10/2020
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <stdbool.h>

#include "expstr.h"
#include "fwd.h"

#ifndef _LEXICAL_HEADER
#define _LEXICAL_HEADER


/**
 * Definice názvů stavů, ve kterých se může nacházet KA scanneru
 */ 
#define STATE_START         200 /// počáteční stav
#define STATE_ID            201 /// identifikátor
#define STATE_LEADING_ZERO  220 /// první nula v integeru
#define STATE_INT           202 /// integer
#define STATE_INT_DOT       203 /// integer s tečkou
#define STATE_FLOAT         204 /// float
#define STATE_FLOAT_EXP     205 /// nedokončený float s exponentem
#define STATE_FLOAT_EXP_FIN 206 /// dokončený float s exponentem
#define STATE_FLOAT_OP      219 /// float + e + operator
#define STATE_STR           207 /// nedokončený string
#define STATE_STR_ESC       208 /// escape v rámci stringu
#define STATE_SLASH         209 /// lomitko z komentáře
#define STATE_COMMENTLINE   210 /// inline komentář končící EOL
#define STATE_COMMENTML     211 /// multiline komentář
#define STATE_STAR          212 /// hvězdička z ukončovací sekvence multiline
#define STATE_SET           213 /// přiřazení
#define STATE_COLON         214 /// nedokončený operátor definice proměnné
#define STATE_LESS          215 /// operátor <
#define STATE_GREATER       216 /// operátor >
#define STATE_EXCLAMATION   217 /// nedokončený op. nerovná se
#define STATE_EOL           218 /// \n
#define STATE_EXIT          250 /// slouží k ukončení načítání znaků při chybě nebo úspěšném sestavení tokenu

// Status kódy pro zpracování řetězců
#define STATUS_OK               0 
#define STATUS_CONV_ERROR       1
#define STATUS_INTERNAL_ERROR   2

/**
 * Debugovací definice
 */

#define DEBUG

#ifdef DEBUG
    #define LEXDUMP char* dump = copyEStr(&tempEStr); fprintf(stderr,"LEX_DUMP\nbuf: %c (%u)\ntokType: %u\nlastState: %u\nstate: %u\nnextState: %u\nattr: %s\n",buf,buf,tok->tokenType,lastState,state,nextState,dump); free(dump);
#else
    #define LEXDUMP
#endif

/***********************
 *  Deklarace funkcí
 **********************/

int getToken(token *tok, char *buffered);
int isKW(char *attr);
char *processString(char *attr, bool freeAttr, int *status);

#endif