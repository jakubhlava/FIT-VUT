#!/usr/bin/python3.8

# Author: Jakub Hlava (xhlava52)

import argparse
import sys
from lxml import etree
import operator
import re
import traceback
from sys import exit

sourcefile = None
inputfile = None

re_stresc = re.compile(r"(\\[0-9]{3})")

## Třída pro paměťové rámce
class Frame():

    def __init__(self):
        self.vars = dict() # Slovník proměnných
        self.notinit = list() # Seznam neinicializovaných proměnných pro oddělení od nil@nil

    def __repr__(self): 
        return "\n".join([f"{key} : {self.vars[key]}" for key in self.vars.keys()])

    def is_not_initialized(self, varname):
        if varname in self.vars.keys() and varname in self.notinit:
            return True
        else:
            return False

    def exists(self, varname):
        "Kontroluje, zda je proměnná definovaná"
        return varname in self.vars.keys()

    def defvar(self, varname):
        "Vytvoření neinicializované proměnné"
        if not self.exists(varname):
            self.vars[varname] = None
            self.notinit.append(varname)
        else:
            sys.stderr.write("chyba 52: redefinice funkce\n")
            exit(52) # chyba 52: redefinice funkce

    def setvar(self, varname, value):
        "Vložení hodnoty do proměnné a případné vyjmutí ze seznamu neinicializovaných"
        if self.exists(varname):
            if type(value) == str:
                value = parse_str(value)
            self.vars[varname] = value
            if varname in self.notinit:
                self.notinit.remove(varname)
        else:
            sys.stderr.write("chyba 54: prace s neexistujici promennou\n")
            exit(54) # chyba 54: prace s neexistujici promennou

    def get(self, varname):
        "Vrací hodnotu proměnné"
        if self.exists(varname):
            if self.vars[varname] != None:
                return self.vars[varname]
            elif varname not in self.notinit:
                return None
            else:
                sys.stderr.write("chyba 56: cteni neinic. promenne\n")
                exit(56) # chyba 56: cteni neinic. promenne
        else:
            sys.stderr.write("chyba 54: prace s neexistujici promennou\n")
            exit(54) # chyba 54: prace s neexistujici promennou

class Interpreter():

    def __init__(self, instlist):
        self.GF = Frame() ## Global Frame, je jen jeden, rovnou inicializuji
        self.LF = None # reference na aktuální LF a TF
        self.TF = None
        self.FStack = [] # zásobník rámců
        self.pc = 0 # počítadlo instrukcí
        self.labels = {} # slovník páru label -> číslo instrukce
        self.callstack = [] # pomocný zásobník pro CALL a RET
        self.stack = [] # zásobník pro PUSHS a POPS
        self.state = 0 # TODO
        try:
            self.source = sorted(instlist, key=lambda x: int(x.get("order"))) # LXML Element objekty reprezentující instrukce, seřazeny
        except:
            sys.stderr.write("chyba 32: nepodařilo se korektně načíst a seřadit zdrojový kód, zkontrolujte XML")
            exit(32)
        self.index = {} # indexování skoky vs. neavazující posloupnost instrukcí
        for i in range(len(self.source)):
            self.index[int(self.source[i].get("order"))] = i

        self.search_for_labels()

    def __repr__(self):
        frames = "\n========================================\n".join([repr(x) for x in [self.GF, self.LF, self.TF]])
        pc = f"PC: {self.pc}"
        FStlen = f"FStack len: {len(self.FStack)}"
        return "\n".join([pc, FStlen, frames])

    def search_for_labels(self):
        for i in self.source:
            if i.get("opcode") == "LABEL":
                if (i[0].text in self.labels.keys()):
                    sys.stderr.write("chyba 52: redefinice návěští\n")
                    exit(52)
                self.labels[i[0].text] = int(i.get("order"))

    def execute(self):
        "Provede jednu instrukci ze seznamu"
        inst = self.source[self.pc]
        self.pc += 1
        #print(f"calling self._{inst.get('opcode').lower()}")
        try:
            func = getattr(self, f"_{inst.get('opcode').lower()}")
        except AttributeError:
            sys.stderr.write("chyba 32: neznámá instrukce\n")
            exit(32)
        except Exception as e:
            sys.stderr.write("chyba 99: interní chyba: {e}\n")
            exit(99)
        try:
            params = sorted(inst[0:], key=lambda x: x.tag)
            validparams = ["arg1", "arg2", "arg3"]
            #func(*inst.iter("arg1","arg2","arg3"))
            for p in range(len(params)):
                if params[p].tag != validparams[p]:
                    sys.stderr.write("chyba 32: neznámý tag")
                    exit(32)
            func(*params)
        except Exception as e:
            sys.stderr.write(f"chyba 32: chyba při vykonávání instrukce: {e}\n")
            traceback.print_exc()
            exit(32)
        if self.pc >= len(self.source):
            self.state = 1

    def get_state(self):
        return self.state

    def getfobj(self, parsed_var):
        "Z názvu proměnné získá objekt rámce"
        if parsed_var["frame"] in ["GF", "LF", "TF"]:
            frame = getattr(self, parsed_var["frame"])
        if frame != None:
            return frame
        else:
            sys.stderr.write("chyba 55: pristup k nedefinovanemu ramci\n")
            exit(55)

    ###########################################################################
    #  Implementace instrukcí IPPcode21, _instrukce
    ###########################################################################

    def _defvar(self, arg):
        var = parse_var(arg.text) 
        frame = self.getfobj(var)
        frame.defvar(var["name"])  
    
    def _createframe(self):
        self.TF = Frame()

    def _pushframe(self):
        if self.TF == None:
            sys.stderr.write("chyba 55: pristup k nedefinovanemu ramci\n")
            exit(55)
        self.LF = self.TF
        self.FStack.append(self.TF)
        self.TF = None

    def _popframe(self):
        if len(self.FStack) == 0:
            sys.stderr.write("chyba 55: pristup k nedefinovanemu ramci\n")
            exit(55)
        self.TF = self.FStack.pop()
        if len(self.FStack) > 0:
            self.LF = self.FStack[-1]
        else:
            self.LF = None

    def _move(self, arg1, arg2):
        v_out = parse_var(arg1.text)
        frame_out = self.getfobj(v_out)
        if arg2.get("type") == "var":
            s_in = parse_var(arg2.text)
            frame_in = self.getfobj(s_in)
            frame_out.setvar(v_out["name"],frame_in.get(s_in["name"]))
        else:
            s_in = parse_const(arg2)
            #print("Parsed const:",s_in)
            if s_in == None and type(s_in) == str:
                s_in = ""
            frame_out.setvar(v_out["name"],s_in) 
        
    def _call(self, arg):
        self.callstack.append(self.pc)
        if arg.text in self.labels.keys():
            self.pc = self.index[self.labels[arg.text]]
        else:
            sys.stderr.write("chyba 52: neexistující návěští\n")
            exit(52)
    
    def _return(self):
        if len(self.callstack) > 0:
            self.pc = self.callstack.pop()
        else:
            sys.stderr.write("chyba 56: prázdný zásobník volání\n")
            exit(56)

    def _pushs(self, arg):
        if arg.get("type") == "var":
            v = parse_var(arg.text)
            frame = self.getfobj(v)
            self.stack.append(frame.get(v["name"]))
        else:
            c = parse_const(arg)
            self.stack.append(c)

    def _pops(self, arg):
        if len(self.stack) > 0:
            v = parse_var(arg.text)
            frame = self.getfobj(v)
            frame.setvar(v["name"], self.stack.pop())
        else:
            sys.stderr.write("chyba 56: prázdný zásobník\n")
            exit(56)

    def calculate(self, op, arg1, arg2, out, types=[int]):
        """Pomocná funkce pro aritmetické a logické instrukce"""
        op1 = None
        op2 = None
        op1 = self.parse_symb(arg1)
        if type(op1) not in types:
            sys.stderr.write(f"chyba 53: nekompatibilní typy operandu: {type(op1)}\n")
            exit(53)
        if op != operator.not_:
            op2 = self.parse_symb(arg2)
            if type(op2) != type(op1) and not (op == operator.eq and (op1 == None or op2 == None)):
                sys.stderr.write(f"chyba 53: nekompatibilní typy operandu: {type(op1)} a {type(op2)}\n")
                exit(53)
        v = parse_var(out.text)
        frame = self.getfobj(v)
        if op == operator.eq and (op1 == None or op2 == None) and not (op1 == op2):
            frame.setvar(v["name"], False)
        elif op != operator.not_:
            frame.setvar(v["name"], op(op1,op2))
        else:
            frame.setvar(v["name"], op(op1))

    def _add(self, arg1, arg2, arg3):
        self.calculate(operator.add, arg2, arg3, arg1)
    
    def _sub(self, arg1, arg2, arg3):
        self.calculate(operator.sub, arg2, arg3, arg1)
    
    def _mul(self, arg1, arg2, arg3):
        self.calculate(operator.mul, arg2, arg3, arg1)
    
    def _idiv(self, arg1, arg2, arg3):
        if arg3.get("type") == "var":
            v = parse_var(arg3.text)
            frame = self.getfobj(v)
            if frame.get(v["name"]) == 0:
                sys.stderr.write("chyba 57: dělení nulou\n")
                exit(57)
        else:
            try:
                const = int(arg3.text)
            except:
                sys.stderr.write("chyba 53: nekompatibilní datový typ\n")
                exit(53)
            if const == 0:
                sys.stderr.write("chyba 57: dělení nulou\n")
                exit(57)
        self.calculate(operator.floordiv, arg2, arg3, arg1)

    def _lt(self, arg1, arg2, arg3):
        self.calculate(operator.lt, arg2, arg3, arg1, [int, bool, str])

    def _gt(self, arg1, arg2, arg3):
        self.calculate(operator.gt, arg2, arg3, arg1, [int, bool, str])

    def _eq(self, arg1, arg2, arg3):
        self.calculate(operator.eq, arg2, arg3, arg1, [int, bool, str, type(None)])

    def _and(self, arg1, arg2, arg3):
        self.calculate(operator.and_, arg2, arg3, arg1, [bool])

    def _or(self, arg1, arg2, arg3):
        self.calculate(operator.or_, arg2, arg3, arg1, [bool])

    def _not(self, arg1, arg2):
        self.calculate(operator.not_, arg2, None, arg1, [bool])
    
    def _int2char(self, arg1, arg2):
        v = parse_var(arg1.text)
        frame = self.getfobj(v)
        char = None
        if arg2.get("type") == "var":
            invar = parse_var(arg2.text)
            inframe = self.getfobj(invar)
            inval = inframe.get(invar["name"])
            if type(inval) != int:
                sys.stderr.write("chyba 53: očekáván int na pozici symb2\n")
                exit(53)
            try:
                char = chr(inval)
            except:
                sys.stderr.write(f"chyba 58: {inval} neni validni hodnota unicode\n")
                exit(58)
        else:
            inconst = parse_const(arg2)
            if type(inconst) != int:
                sys.stderr.write("chyba 53: očekáván int\n")
                exit(53)
            try:
                char = chr(inconst)
            except:
                sys.stderr.write(f"chyba 58: {inconst} neni validni hodnota unicode\n")
                exit(58)
        frame.setvar(v["name"],char)
            
    def _stri2int(self, arg1, arg2, arg3):
        source = None
        index = None
        source = self.parse_symb(arg2)
        if type(source) != str:
            sys.stderr.write("chyba 53: očekáván string na pozici symb1\n")
            exit(53)
        index = self.parse_symb(arg3)
        if type(index) != int:
            sys.stderr.write("chyba 53: očekáván int na pozici symb2\n")
            exit(53)
        if index >= len(source) or index < 0:
            sys.stderr.write("chyba 58: index mimo rozsah pole\n")
            exit(58)
        v = parse_var(arg1.text)
        frame = self.getfobj(v)
        frame.setvar(v["name"], ord(source[index]))

    def _read(self, arg1, arg2):
        t = parse_type(arg2.text)
        try:
            if t == bool: # ošetření bool převodů
                line = get_input_line()
                if line.lower() == "true":
                    inp = True
                else:
                    inp = False
                    
            else:
                inp = t(get_input_line())
        except:
            inp = None
        v = parse_var(arg1.text)
        frame = self.getfobj(v)
        frame.setvar(v["name"], inp)

    def _write(self, arg1):
        s1 = self.parse_symb(arg1)
        if s1 == None:
            pass
        elif type(s1) == bool:
            if s1:
                print("true",end="")
            else:
                print("false",end="")
        else:
            print(s1,end="")

    def _concat(self, arg1, arg2, arg3):
        in1 = self.parse_symb(arg2)
        in2 = self.parse_symb(arg3)
        if type(in1) != str or type(in2) != str:
            sys.stderr.write("chyba 53: concat: očekáván typ string\n")
            exit(53)
        outv = parse_var(arg1.text)
        outf = self.getfobj(outv)
        outf.setvar(outv["name"], in1+in2)

    def _strlen(self, arg1, arg2):
        inp = self.parse_symb(arg2)
        if type(inp) != str:
            sys.stderr.write("chyba 53: strlen: očekáván typ string\n")
            exit(53)
        outv = parse_var(arg1.text)
        outf = self.getfobj(outv)
        outf.setvar(outv["name"],len(inp))

    def _getchar(self, arg1, arg2, arg3):
        in_str = self.parse_symb(arg2)
        index = self.parse_symb(arg3)
        if type(in_str) != str:
            sys.stderr.write("chyba 53: getchar: na pozici symb1 očekáván typ string\n")
            exit(53)
        if type(index) != int:
            sys.stderr.write("chyba 53: getchar: na pozici symb2 očekáván typ int\n")
            exit(53)
        if index >= len(in_str) or index < 0:
            sys.stderr.write("chyba 58: index mimo rozsah pole\n")
            exit(58)
        v = parse_var(arg1.text)
        frame = self.getfobj(v)
        frame.setvar(v["name"], in_str[index])

    def _setchar(self, arg1, arg2, arg3):
        v = parse_var(arg1.text)
        frame = self.getfobj(v)
        in_str = frame.get(v["name"])
        if type(in_str) != str:
            sys.stderr.write("chyba 53: setchar: na pozici var očekáván typ string\n")
            exit(53)
        index = self.parse_symb(arg2)
        if type(index) != int:
            sys.stderr.write("chyba 53: setchar: na pozici symb1 očekáván typ int\n")
            exit(53)
        changeto = self.parse_symb(arg3)
        if type(changeto) != str:
            sys.stderr.write("chyba 53: setchar: na pozici symb2 očekáván typ string\n")
            exit(53)
        if index >= len(in_str) or index < 0 or in_str == "":
            sys.stderr.write("chyba 58: index mimo rozsah pole\n")
            exit(58)
        if len(changeto) == 0:
            sys.stderr.write("chyba 58: očekáván neprázdný řetězec na pozici symb2\n")
            exit(58)
        out_str = in_str[:index] + changeto[0] + in_str[index+1:]
        frame.setvar(v["name"], out_str)

    def _type(self, arg1, arg2):
        symb = self.parse_symb(arg2, notinitialized=True)
        v = parse_var(arg1.text)
        frame = self.getfobj(v)
        if symb == (None, False):
            frame.setvar(v["name"], "")
        elif symb is None:
            frame.setvar(v["name"], "nil")
        elif type(symb) == int:
            frame.setvar(v["name"], "int")
        elif type(symb) == str:
            frame.setvar(v["name"], "string")
        elif type(symb) == bool:
            frame.setvar(v["name"], "bool")  

    def _label(self, arg1):
        pass

    def _jump(self, arg1):
        if arg1.text in self.labels.keys():
            self.pc = self.index[self.labels[arg1.text]]
        else:
            sys.stderr.write("chyba 52: neexistující návěští\n")
            exit(52)

    def iseq(self, symb1, symb2):
        """Pomocná funkce pro zjištění ekvivalence <symb> neterminálů"""
        s1 = self.parse_symb(symb1)
        s2 = self.parse_symb(symb2)
        if (s1 == None and s2 == None):
            return True
        if (s1 == None or s2 == None):
            return False
        if type(s1) == type(s2):
            if s1 == s2:
                return True
            else:
                return False
        else:
            sys.stderr.write("chyba 53: nekompatibilní operandy při porovnání\n")
            exit(53)

    def _jumpifeq(self, arg1, arg2, arg3):
        if self.iseq(arg2,arg3):
            self._jump(arg1)
        else:
            if arg1.text not in self.labels.keys():
                sys.stderr.write("chyba 52: neexistující návěští\n")
                exit(52)

    def _jumpifneq(self, arg1, arg2, arg3):
        if not self.iseq(arg2,arg3):
            self._jump(arg1)
        else:
            if arg1.text not in self.labels.keys():
                sys.stderr.write("chyba 52: neexistující návěští\n")
                exit(52)

    def _exit(self, arg1):
        symb = self.parse_symb(arg1)
        if type(symb) != int:
            sys.stderr.write("chyba 53: exit očekává int\n")
            exit(53)
        else:
            if symb > 49 or symb < 0:
                sys.stderr.write("chyba 57: neplatná návratová hodnota instrukce exit\n")
                exit(57)
            else:
                exit(symb)

    def _dprint(self, arg1):
        symb = self.parse_symb(arg1)
        sys.stderr.write(symb)

    def _break(self):
        sys.stderr.print(repr(self))

    def parse_symb(self, symb, notinitialized=False):
        """Pomocná funkce pro získání hodnoty ze <symb> neterminálu"""
        val = None
        if symb.get("type") == "var":
            v = parse_var(symb.text)
            frame = self.getfobj(v)
            if notinitialized:
                if frame.is_not_initialized(v["name"]):
                    return (None, False)
            val = frame.get(v["name"])
        else:
            val = parse_const(symb)
        return val

def get_input_line():
    """Rozhraní pro instrukci READ, aby bylo možné načítat ze vstupního souboru i STDIN"""
    global inputfile
    if inputfile == None:  
        return input()
    else:
        return inputfile.readline().replace("\n","") # ořez \n

def parse_const(conststr): 
    """Získá hodnotu konstanty, korektně ji přetypuje a vrátí"""
    if conststr.get("type") == "string":
        value = parse_str(conststr.text)
    elif conststr.get("type") == "int":
        value = int(conststr.text)
    elif conststr.get("type") == "bool":
        if conststr.text == "true":
            value = True
        else:
            value = False
    elif conststr.get("type") == "nil":
        value = None
    return value

def parse_var(var):
    """Rozdělí neterminál <var> na rámec a název proměnné"""
    parts = var.split("@", maxsplit=1)
    return {
        "frame": parts[0],
        "name": parts[1]
    }

def parse_type(t):
    """Převede <type> na typovou třídu"""
    if t == "int":
        return int
    elif t == "string":
        return str
    elif t == "bool":
        return bool
    else:
        sys.stderr.write(f"chyba 57: neexistující typ: {t}\n")
        exit(57)

def parse_str(value):
    """Nahradí escapované znaky v řetězci"""
    if value == None:
        value = ""
    escaped = set(re_stresc.findall(value))
    ret = value
    for e in escaped:
        ret = ret.replace(e, chr(int(e[1:])))
    return ret

argparser = argparse.ArgumentParser(prog="interpret.py", usage="%(prog)s [--help] [--source=soubor.xml] [--input=soubor]", add_help=False)
argparser.add_argument("--source", type=str, nargs=1, help="vstupní soubor s XML reprezentací zdrojového kódu")
argparser.add_argument("--input", type=str, nargs=1, help="soubor se vstupy pro interpretaci zadaného zdrojového kódu")
argparser.add_argument("--help", action='store_true', help="zobrazí tuto nápovědu, nelze kombinovat s jinými parametry")
args = argparser.parse_args()

# Ukončení s chybou u kombinace --help s argumentem kombinace chybějících argumentů --source a --input
if (args.source == args.input == None and args.help == False) or (args.help == True and (args.source != None or args.input != None)):
    sys.stderr.write("chyba: nepovolená kombinace vstupnich parametru\n")
    print()
    argparser.print_help()
    exit(10)

# Generovaná nápověda
if args.help:
    argparser.print_help()
    exit(0)

# Otevření souborů na základě spouštěcích argumentů a přiřazení jejich objektů do globálních proměnných pro pozdější použití
# V případě, že jeden ze souborů není dostupný, použije se STDIN
if args.source != None:
    try:
        f = open(args.source[0], "r", encoding="utf-8")
    except Exception as e:
        sys.stderr.write(f"chyba: nelze otevřít soubor: {args.source[0]}: {e}\n")
        exit(11)
    sourcefile = f
if args.input != None:
    try:
        f = open(args.input[0], "r", encoding="utf-8")
    except Exception as e:
        sys.stderr.write(f"chyba: nelze otevřít soubor: {args.input[0]}: {e}\n")
        exit(11)
    inputfile = f
    
source = None
if sourcefile == None:
    source = sys.stdin.read()
else:
    source = sourcefile.read()

# Zpracování XML pomocí lxml
parser = etree.XMLParser(encoding="utf-8")
source = source.encode("utf-8")
try:
    sourcetree = etree.fromstring(source, parser=parser)
except Exception as e:
    sys.stderr.write(f"chyba 31: špatný formát XML: {str(e)}\n")
    exit(31)

if sourcetree.tag != "program":
    sys.stderr.write("chyba 32: nesprávný kořenový tag")
    exit(32)

# Inicializace intrepretu
i = Interpreter(sourcetree)
# Validace
orders = [int(x.get("order")) for x in i.source]
if len(i.source) == 0:
    exit(0)
if len(set(orders)) != len(orders):
    sys.stderr.write("chyba 32: nejasné pořadí instrukcí - duplikáty")
    exit(32)
if orders[0] < 1:
    sys.stderr.write("chyba 32: chybná posloupnost instrukcí")
    exit(32)
if sourcetree.get("language") != "IPPcode21":
    sys.stderr.write(f"chyba 32: neznámý jazyk {sourcetree.get('language')}")
    exit(32)
if len([x.tag for x in i.source if x.tag != "instruction"]) > 0:
    sys.stderr.write("chyba 32: neznámé tagy v XML reprezentaci")
    exit(32)
while(i.get_state() == 0): # state 0 -> lze interpretovat, state > 0 -> interpretace dokončena
    i.execute()

if inputfile != None:
    inputfile.close()
if sourcefile != None:
    sourcefile.close()