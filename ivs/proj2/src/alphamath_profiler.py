## 
# @file alphamath_profiler.py
# @brief Skript, sloužící pro profilování knihovny alphamath
# @author Jakub Hlava (xhlava52)
# @date 03/2020

import am_profiling_script as aps
import profile

# spuštění profilování pomocí modulu profile
profile.run("aps.calculateDeviation()")