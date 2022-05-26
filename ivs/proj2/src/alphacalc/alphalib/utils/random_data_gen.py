## 
# @file random_data_gen.py
# @brief Skript pro generování sady náhodných dat pro profiling.
# @author Jakub Hlava (xhlava52)
# @date 03/2020

from random import randint as ri

counts = [10,100,1000] # nastavení variant s počtem vstupů

for count in counts:
    f = open("profiledata_{0}.txt".format(count), "w")
    f.write(" ".join([str(ri(0,200)) for x in range(count)]))
    f.close()
