//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Test Driven Development - priority queue code
//
// $NoKeywords: $ivs_project_1 $tdd_code.cpp
// $Author:     Jakub Hlava <xhlava52@stud.fit.vutbr.cz>
// $Date:       $2020-02-16
//============================================================================//
/**
 * @file tdd_code.cpp
 * @author Jakub Hlava
 * 
 * @brief Implementace metod tridy prioritni fronty.
 */

#include <stdlib.h>
#include <stdio.h>

#include "tdd_code.h"

//============================================================================//
// ** ZDE DOPLNTE IMPLEMENTACI **
//
// Zde doplnte implementaci verejneho rozhrani prioritni fronty (Priority Queue)
// 1. Verejne rozhrani fronty specifikovane v: tdd_code.h (sekce "public:")
//    - Konstruktor (PriorityQueue()), Destruktor (~PriorityQueue())
//    - Metody Insert/Remove/Find a GetHead
//    - Pripadne vase metody definovane v tdd_code.h (sekce "protected:")
//
// Cilem je dosahnout plne funkcni implementace prioritni fronty implementovane
// pomoci tzv. "double-linked list", ktera bude splnovat dodane testy 
// (tdd_tests.cpp).
//============================================================================//

PriorityQueue::PriorityQueue()
{
}

PriorityQueue::~PriorityQueue()
{
	while (GetHead() != NULL) {
		Remove(GetHead()->value);
	}
}

void PriorityQueue::Insert(int value)
{
    if (first == NULL) {
        first = new Element_t;
        first->pNext = NULL;
        first->pPrev = NULL;
        first->value = value;
    }
    else 
    {
        Element_t* temp = first;
        while (temp->pNext != NULL) {
            if (temp->pNext->value >= value)
                break;
            else
                temp = temp->pNext;
        }
        Element_t* inserted = new Element_t;
        if (temp == first && value < first->value) {
            inserted->pPrev = NULL;
            temp->pPrev = inserted;
            inserted->pNext = temp;
            inserted->value = value;
            first = inserted;
        }
        else 
        {
            inserted->pPrev = temp;
            inserted->pNext = temp->pNext;
            if (temp->pNext != NULL) {
                temp->pNext->pPrev = inserted;
            }
            temp->pNext = inserted;
            inserted->value = value;
        }
    }
    
}

bool PriorityQueue::Remove(int value)
{
    Element_t *temp = first;
    while (temp != NULL) {
        if (temp->value == value) {
            if (temp->pNext == NULL && temp->pPrev == NULL) {
                first = NULL;
            } else if (temp->pNext == NULL) {
                temp->pPrev->pNext = temp->pNext;
            }
            else if (temp->pPrev == NULL) {
                temp->pNext->pPrev = temp->pPrev;
                first = temp->pNext;
            }
            else
            {
                temp->pNext->pPrev = temp->pPrev;
                temp->pPrev->pNext = temp->pNext;
            }
			delete temp;
            return true;
        }
        else {
            temp = temp->pNext;
        }
    }
    return false;
}

PriorityQueue::Element_t *PriorityQueue::Find(int value)
{
    Element_t* temp = first;
    while (temp != NULL) {
        if (temp->value == value) {
            return temp;
        }
        else
        {
            temp = temp->pNext;
        }
    }
    return NULL;
}

PriorityQueue::Element_t *PriorityQueue::GetHead()
{
    return first;
}


/*** Konec souboru tdd_code.cpp ***/
