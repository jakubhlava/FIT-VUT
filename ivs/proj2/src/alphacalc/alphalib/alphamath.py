##
 # @file alphamath.py
 # @brief Knihovna pro kalkulačku
 # @author Lukáš Kraus (xkraus13)
 # @date březen 2020

import math
from decimal import *
from fractions import *

##
 # @brief funkce sčíta 2 hodnoty
 # @param a     čitatel
 # @param b     čitatel
 # @return součet čísel "a" a "b"

def add(a, b):
    if type(a) == str or type(b) == str:
        raise TypeError
    return a + b

##
 # @brief funkce odčíta 2 hodnoty
 # @param a     menšenec
 # @param b     menšitel
 # @return rozdíl čísel "a" a "b"

def sub(a, b):
    if type(a) == str or type(b) == str:
        raise TypeError
    return a - b

##
 # @brief funkce násobí 2 hodnoty
 # @param a     činitel
 # @param b     činitel
 # @return součin čísel "a" a "b"
 
def mul(a, b):
    if type(a) == str or type(b) == str:
        raise TypeError
    return a * b

##
 # @brief funkce dělí 2 hodnoty
 # @param a     dělenec
 # @param b     dělitel
 # @return podíl čísel "a" a "b"
 
def div(a, b):
    if type(a) == str or type(b) == str:
        raise TypeError
    if b == 0:
        raise ValueError
    return a / b

##
 # @brief funkce mocní číslo "a" exponentem "b"
 # @param a    mocněnec
 # @param b    mocnitel
 # @return mocnina cisla "a" na "b"
 
def pow(a, b):
    if type(a) == str or type(b) == str:
        raise TypeError
    return math.pow(a,b)

##
 # @brief funkce odmocňuje číslo "a" hodnotou "b"
 # @param a    odmocněnec
 # @param b    odmocnitel
 # @return odmocnina čísla "a" 
 
def sqrt(a, b):
    if type(a) == str or type(b) == str:
        raise TypeError
    if a < 0:
        raise ValueError
    if type(a) == Fraction:
        a = float(a)
    if type(b) == Fraction:
        b = float(b)  
    return Decimal(a) ** (1/Decimal(b))

##
 # @brief funkce zjišťuje faktorial cisla "a"
 # @param a     číslo pro faktorizaci
 # @return faktorial cisla "a"
 
def fac(a):
    if type(a) != int and type(a) != float:
        raise TypeError
    if a % 1 != 0:
        raise TypeError
    if a < 0:
        raise ValueError
    return math.factorial(a)

##
 # @brief funkce zjišťuje sinus hodnotu cisla "a"
 # @param a     úhel v radianech
 # @return sinus čísla "a"
 
def sin(a):
    if type(a) == str:
        raise TypeError
    return math.sin(a)

##
 # @brief funkce zjišťuje cosinus hodnotu cisla "a"
 # @param a     úhel v radianech
 # @return cosinus uhlu "a"
 
def cos(a):
    if type(a) == str:
        raise TypeError
    return math.cos(a)

##
 # @brief funkce zjišťuje přirozený logaritmus cisla "x"
 # @param x     logaritmované číslo
 # @return přirozený logaritmus cisla "x"
 
def ln(x):
    if type(x) == str:
        raise TypeError
    if x <= 0:
        raise ValueError
    return log(x, math.e)

##
 # @brief funkce zlogaritmuje číslo "x"
 # @param x     logaritmované číslo
 # @param base  základ logaritmu
 # @return logaritmus cisla "x" o zakladu "base"
 
def log(x, base=10):
    if type(x) == str or type(base) == str:
        raise TypeError
    if x <= 0 or base <= 0 or base == 1:
        raise ValueError
    return math.log(x,base)

##
 # @brief funkce zjišťuje tangens úhlu "a"
 # @param a     úhel v radiánech
 # @return tangens úhlu "a"
 
def tan(a):
    if type(a) == str:
        raise TypeError
    if math.isclose(cos(a),0,abs_tol=1e-16):
        raise ValueError
    return math.tan(a)

##
 # @brief funkce zjišťuje absolutní hodnotu "a"
 # @param a     hodnota, u které chceme zjistit absolutní hodnotu
 # @return absolutní hodnota "a"
 
def abs(a):
    if type(a) == str:
        raise TypeError
    return Decimal(a).copy_abs()
