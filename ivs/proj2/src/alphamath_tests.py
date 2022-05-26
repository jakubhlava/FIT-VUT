import unittest
from fractions import Fraction

from alphacalc.alphalib.alphamath import *
import math
class TestAdd(unittest.TestCase):   ### test funkce sčítání

    def test_add(self): #sčítání s negativním číslem
        data = [2, -5]
        result = add(*data)
        self.assertEqual(result, -3)

    def test_add_fractions(self):   #součet zlomků
        data = [Fraction(1,8), Fraction(-3, 9)]
        result = add(*data)
        self.assertEqual(result, Fraction(-5, 24))

    def test_add_fraction_result_int(self): #součet zlomků s celočíselným výsledkem
        data = [Fraction(1,8), Fraction(7, 8)]
        result = add(*data)
        self.assertEqual(result, 1)

    def test_add_letters(self):     #sčítání neplatných znaků
        self.assertRaises(TypeError, add, "a", "b")
    
    def test_add_decimals(self):
        data = [3.3, 1.2]
        result = add(*data)
        self.assertEqual(result, 4.5)

class TestSub(unittest.TestCase):   ###odčítání

    def test_sub_ints(self): #odčíténí celých čísel
            data = [2, -5]
            result = sub(*data)
            self.assertEqual(result, 7)

    def test_sub_fractions(self): #odčítání zlomků
        data = [Fraction(1,2), Fraction(1, 3)]
        result = sub(*data)
        self.assertEqual(result, Fraction(1, 6))

    def test_sub_fractions_result_int(self): #odčítání zlomků s celočíselným výsledkem
        data = [Fraction(1,2), Fraction(1, 2)]
        result = sub(*data)
        self.assertEqual(result, 0)

    def test_sub_letters(self): #očítání neplatných znaků
        self.assertRaises(TypeError, sub, "a", "b")

    def test_sub_decimals(self): #odčítání desetinných čísel
        data = [3.3, 1.2]
        result = sub(*data)
        self.assertEqual(result, 3.3-1.2)

class TestMul(unittest.TestCase):   ### test násobení

    def test_mul(self): #násobení celých čísel
            data = [2, -5]
            result = mul(*data)
            self.assertEqual(result, -10)

    def test_mul_fractions_result_fraction(self): #násobení zlomků
        data = [Fraction(1,2), Fraction(2, 3)]
        result = mul(*data)
        self.assertEqual(result, Fraction(1,3))

    def test_mul_fractions_result_int(self): #násobení zlomků s celočíselným výsledkem
        data = [Fraction(2,5), Fraction(5, 2)]
        result = mul(*data)
        self.assertEqual(result, 1)

    def test_mul_letters(self): #násobení neplatných znaků
        self.assertRaises(TypeError, mul, "a", "b")

    def test_mul_decimals(self): #násobení desetinných čísel
        data = [3.3, 1.2]
        result = mul(*data)
        self.assertEqual(result, 3.3*1.2)

class TestDiv(unittest.TestCase):

    def test_div(self): #dělení celých čísel
            data = [2, -5]
            result = div(*data)
            self.assertEqual(result, -0.4)

    def test_div_fractions(self): #dělení zlomků
        data = [Fraction(1,2), Fraction(2, 3)]
        result = div(*data)
        self.assertEqual(result, Fraction(3, 4))

    def test_div_letters(self): #dělení neplatných znaků
        self.assertRaises(TypeError, div, "a", "b")

    def test_div_decimals(self): #dělení desetinných čísel
        data = [3.3, 1.2]
        result = div(*data)
        self.assertEqual(result, 2.75)

class TestPow(unittest.TestCase): #test umocnění

    def test_pow_neg_exponent(self): #umocnění záporným číslem
            data = [2, -5]
            result = pow(*data)
            self.assertEqual(result, Fraction(1,32))

    def test_pow_ints(self):    #umocnění celým číslem
            data = [2, 5]
            result = pow(*data)
            self.assertEqual(result, 32)

    def test_pow_Int_fractionExp(self): #umocnění zlomkem
        data = [4, Fraction(1, 2)]
        result = pow(*data)
        self.assertEqual(result, 2)

    def test_pow_negInt_fractionExp(self): #umocnění záporného čísla zlomkem
        self.assertRaises(ValueError, pow, -4, Fraction(1,2))

    def test_pow_letters(self): #umocnění s neplatnými znaky
        self.assertRaises(TypeError, pow, "a", "b")

class TestSqrt(unittest.TestCase):  #test odmocnění

    def test_sqrt(self): #druhá odmocnina
        data = [4, 2]
        result = sqrt(*data)
        self.assertEqual(result, 2)

    def test_sqrt_secondarg_fraction(self): #odmocnění zlomkem
        data = [4, Fraction(1, 2)]
        result = sqrt(*data)
        self.assertEqual(result, 16)

    def test_sqrt_neg_num(self): #odmocňování záporného čísla
        self.assertRaises(ValueError, sqrt, -4, 2)

    def test_sqrt_letters(self): #odmocnění neplatných znaků
        self.assertRaises(TypeError, sqrt, "a", "b")

    def test_sqrt_decimals(self):   #odmocňování desetinného čísla
        data = [0.25, 2]
        result = sqrt(*data)
        self.assertEqual(result, 0.5)

class TestFactorial(unittest.TestCase): #test faktoriálu

    def test_fac_neg_num(self): #faktoriál záporného čísla
        self.assertRaises(ValueError, fac, -5)

    def test_fac_0(self):   #faktoriál nuly
        result = fac(0)
        self.assertEqual(result, 1)

    def test_fac_int(self): #faktoriál celého čísla
        result = fac(5)
        self.assertEqual(result, 120)

    def test_fac_letters(self): #faktoriál neplatného znaku
        self.assertRaises(TypeError, fac, "a")
        
    def test_fac_decimal(self): #faktroriál desetinného čísla
        self.assertRaises(TypeError, fac, 2.4)

class TestSin(unittest.TestCase):   #test sinu
    
    def test_sin_270(self):
        result = sin(270)
        self.assertAlmostEqual(result, -0.17604595)
        
    def test_sin_90(self):
        result = sin(90)
        self.assertAlmostEqual(result, 0.89399666)
    
    def test_sin_0(self):
        result = sin(0)
        self.assertEqual(result, 0)
    
    def test_sin_pi(self):
        result = sin(math.pi)
        self.assertAlmostEqual(result, 0)
    
    def test_sin_pihalf(self):
        result = sin(math.pi/2)
        self.assertEqual(result, 1)

    def test_sin_letters(self): #sin neplatného znaku
        self.assertRaises(TypeError, sin, "a")

class TestCos(unittest.TestCase):   #test cosinu
    
    def test_cos_270(self):
        result = cos(270)
        self.assertAlmostEqual(result, 0.98438195)
        
    def test_cos_180(self):
        result = cos(180)
        self.assertAlmostEqual(result, -0.59846007)
    
    def test_cos_0(self):
        result = cos(0)
        self.assertEqual(result, 1)

    def test_cos_pi(self):
        result = cos(math.pi)
        self.assertEqual(result, -1)

    def test_cos_8(self):
        result = cos(8)
        self.assertAlmostEqual(result, -0.1455000338)

    def test_cos_letters(self): #cos neplatného znaku
        self.assertRaises(TypeError, cos, "a")

class TestTan(unittest.TestCase): #test tangens
    
    def test_tan_pi(self):
        result = tan(math.pi)
        self.assertAlmostEqual(result, 0)

    def test_tan_8(self):
        result = tan(8)
        self.assertAlmostEqual(result, -6.799711455)

    def test_tan_min2(self):
        result = tan(-2)
        self.assertAlmostEqual(result, 2.185039863)
        
    def test_tan_notdef_pihalf(self):
        self.assertRaises(ValueError, tan, math.pi/2)

    def test_tan_letters(self): #tangens neplatného znaku
        self.assertRaises(TypeError, tan, "a")
    
    def test_tan_0(self):
        result = tan(0)
        self.assertEqual(result, 0)

class TestLn(unittest.TestCase):    #test přirozeného logaritmu
    
    def test_ln_neg_num(self):  #zlogaritmování zápporného čísla
        self.assertRaises(ValueError, ln, -10)

    def test_ln_0(self):
        self.assertRaises(ValueError, ln, 0)

    def test_ln_e(self):
        result = ln(math.e)
        self.assertEqual(result, 1)

    def test_ln_8(self):
        result = ln(8)
        self.assertAlmostEqual(result, 2.079441542)

    def test_ln_letters(self): #zlogaritmování neplatného znaku
        self.assertRaises(TypeError, ln, "a")
    

class TestLog(unittest.TestCase):   #test logaritmu se základem 10

    def test_log_neg_num(self):
        self.assertRaises(ValueError, log, -10)

    def test_log_0(self):
        self.assertRaises(ValueError, log, 0)

    def test_log_1000(self):
        result = log(1000)
        self.assertAlmostEqual(result, 3)

    def test_log_letters(self): #zlogaritmování neplatného znaku
        self.assertRaises(TypeError, log, "a")

class TestAbs(unittest.TestCase):   #test absolutní hodnoty
    def test_abs_8(self):
        result = abs(8)
        self.assertEqual(result, 8)

    def test_abs_neg8(self):
        result = abs(-8)
        self.assertEqual(result, 8)

    def test_abs_letters(self): #absolutní hodnota neplatného znaku
        self.assertRaises(TypeError, abs, "a")

if __name__ == '__main__':
    unittest.main()