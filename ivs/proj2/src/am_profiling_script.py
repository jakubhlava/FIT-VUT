## 
# @file am_profiling_script.py
# @brief Skript pro výpočet výběrové směrodatné odchylky
# Skript slouží pro účely profilingu knihovny alphamath.
# @author Jakub Hlava (xhlava52)
# @date 03/2020

import alphacalc.alphalib.alphamath as am
import sys
import profile

## Funkce pro výpočet směrodatné odchylky
#
# Funkce načítá čásla ze standardního vstupu, vypočte z nich směrodatnou odchylku
# tu pak vypíše na standardní výstup.
def calculateDeviation():
    inputData = sys.stdin.read()
    inputInts = [float(x) for x in inputData.split()]
    N = len(inputInts)
    deviation = am.sqrt(am.mul(am.div(1,N-1),am.sub(sum([am.pow(x,2) for x in inputInts]),am.mul(N,am.pow(am.mul(am.div(1,N),sum(inputInts)),2)))),2)
    print(deviation)
