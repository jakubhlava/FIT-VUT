## 
# @file __main__.py
# @brief Hlavní okno kalkulačky a jeho funkcionalita
# @author Jakub Hlava (xhlava52)
# @author Lukáš Kraus (xkraus13)
# @author Thi Bao Ngoc Vu (xvuthi00)
# @date 04/2020

try:
    from GUI.alphacalcUI import Ui_MainWindow
    import alphalib.utils.currency as convertLib
    import alphalib.alphamath as amath
except ImportError:
    from alphacalc.GUI.alphacalcUI import Ui_MainWindow
    import alphacalc.alphalib.utils.currency as convertLib
    import alphacalc.alphalib.alphamath as amath
import sys
from PySide2.QtWidgets import QApplication, QMainWindow, QWhatsThis, QFrame
from PySide2 import QtCore, QtGui
from math import ceil
from pkg_resources import resource_filename
from decimal import Decimal

# -----------------------------------------------------------------------------------------------
# Globální a stavové proměnné
# -----------------------------------------------------------------------------------------------

__version__ = "1.0"

## @var lastNum
# Poslední číslo z displeje, pro potřeby výpočtů
lastNum = 0

## @var ansNum
# Poslední výsledek pro funkci tlačítka Ans
ansNum = 0

## @var memNum
# Proměnná pro ukládání hodnot funkce M
memNum = 0

## @var displayedNumber
#  Číslo zobrazené na hlavní displeji kalkulačky
displayedNumber = "0"

## @var memSaved
# Určuje, zda je v memNum uložená hodnota
# True      pokud je uložená hodnota
# False     pokud není uložená hodnota
memSaved = False

## @var operationPicked 
# Určuje, zda uživatel už vybral operaci
# True      pokud je vybrána operace (a tedy s výběrem další operace se provede výpočet)
# False     pokud není vybrána operace (a s výběrem další operace se jen vyčistí displej)
operationPicked = False

## @var displayDot
# Určuje, zda je zobrazena desetinná čárka
# True      pokud v číslu je desetinná čárka
# False     pokud v číslu není desetinná čárka
displayDot = False

## @var operationID
# Identifikátor prováděné operace 
# obsahuje identifikátor operace, která se poté vybírá z dictionary operations
# @see      MainWindow.operations
operationID = "empty"

## @var calculated
# Určuje, zda je číslo na displeji uživatelský vstup nebo výsledek výpočtu
# True      výsledek výpočtu
# False     uživatelský vstup
calculated = False

## @var error
# Indikuje, zda se kalkulačka nachází v chybovém stavu
error = False

fontFile = resource_filename(__name__, 'GUI/resources/RobotoMono-Regular.ttf')
iconFile = resource_filename(__name__, 'GUI/resources/icon.png')

# -----------------------------------------------------------------------------------------------

# -----------------------------------------------------------------------------------------------
# Hlavní okno programu
# -----------------------------------------------------------------------------------------------

## Třída hlavního okna
# 
# Načte vygenerovaný základ okna z modulu alphacalc_ui a aplikuje úpravy a funkce na tlačítka
class MainWindow(QMainWindow):
    ## Inicializace okna, jeho úpravy a propojení tlačítek s funkcemi
    def __init__(self):
        ## Seznam operací s funkcemi pro jejich vykonání
        # 
        # Obsahuje vždy dvojici "klíč":funkce
        # Slouží pro zjednodušení práce s operacemi, stačí zde přidat klíč
        # a poté nastavovat klíč operace do proměnné operationID
        # @see operationID
        # @see MainWindow.calculate
        self.operations = {
            "conversion": self.convertCurrency,
            "add": self.calculateAdd,
            "sub": self.calculateSub,
            "mul": self.calculateMul,
            "div": self.calculateDiv,
            "sqrt": self.calculateSqrt,
            "sqrt2": self.calculateSqrt2,
            "pow": self.calculatePow,
            "pow2": self.calculatePow2,
            "fac": self.calculateFac,
            "sin": self.calculateSin,
            "cos": self.calculateCos,
            "tan": self.calculateTan,
            "log": self.calculateLog,
            "ln": self.calculateLn,
            "abs": self.calculateAbs,
            "inv": self.calculateInv,
            "per": self.calculatePer,
            "empty": self.doNothing
        }

        ## Databáze chybových hlášení
        self.errorMessages = {
            "divNull": "Ani AlphaCalc neumí dělit nulou :(",
            "sqrtNeg": "Záporné číslo odmocnit neumím :(",
            "facFloat": "Faktorizovat umím jen celé číslo!",
            "facNeg": "Faktorizovat umím jen kladná čísla!",
            "logNeg": "Logaritmovat lze pouze kladné číslo!",
            "logBase": "Logaritmus o tomto základu neznám!",
            "tanBad": "Toto číslo nemá tangens! :(",
            "invZero": "Nulu převrátit neumím :(",
            "noInternet": "Nemůžu se připojit k serveru! :(",
            "clearMsg": "Stiskni C/Delete"
        }

        super(MainWindow, self).__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)


# ---------------------------------------------
# Nastavení okna      
# ---------------------------------------------
        self.setWindowFlags(QtCore.Qt.FramelessWindowHint) # Odstranění okraje okna - zapnout až po implementaci přesunu okna za vršek
        self.ui.currFrame.setVisible(False) # Skrytí rámce s převodníkem měn
        self.oldPos = self.pos() # proměnná pro pozici okna
        QtGui.QFontDatabase.addApplicationFont(fontFile) # načtení fontu pro displej
        self.setWindowTitle("AlphaCalc 1.0")      

# ---------------------------------------------
# Nastavení prvků      
# ---------------------------------------------
        self.ui.currFrame.keyPressEvent = self.keyPressEvent # Propojení odchytávání tlačítek v otevřeném framu s celým oknem
        self.ui.Frame_header.mousePressEvent = self.framePressEvent
        self.ui.Frame_header.mouseMoveEvent = self.frameMoveEvent
        self.ui.combo_currFrom.keyPressEvent = self.keyPressEvent
        self.ui.combo_currTo.keyPressEvent = self.keyPressEvent # Odchytávání tlačítek i v comboboxu
        self.ui.combo_currFrom.addItems(convertLib.currencies) # Načtení měn do comboboxů
        self.ui.combo_currTo.addItems(convertLib.currencies)
        self.ui.combo_currTo.setCurrentIndex(1) # Posunutí pravého comboboxu, aby nebyl výchozí převod z první měny do první měny
        self.ui.notify_M.setVisible(False) # Skrytí indikátoru uloženého čísla v paměti
        self.ui.notify_E.setVisible(False) # Skrytí indikátoru chyby
# ---------------------------------------------
# Připojení tlačítek
# ---------------------------------------------
        self.ui.Button_help.clicked.connect(self.helpOnOff) # Připojení ? tlačítka k What's This nápovědě
        self.ui.Button_minimize.clicked.connect(self.minimize) # Připojení _ tlačítka k minimalizaci
        self.ui.Button_converter.clicked.connect(self.showHideCurrencies) # Připojení funkce na zobrazení a skrytí rámce s měnami
        self.ui.Button_equals.clicked.connect(self.buttonCalculate)
        self.ui.Button_ac.clicked.connect(self.clearActions)
        self.ui.Button_del.clicked.connect(self.backspaceDisplay)
        self.ui.Button_add.clicked.connect(self.buttonAdd)
        self.ui.Button_sub.clicked.connect(self.buttonSub)
        self.ui.Button_div.clicked.connect(self.buttonDiv)
        self.ui.Button_mul.clicked.connect(self.buttonMul)
        self.ui.Button_sqrt.clicked.connect(self.buttonSqrt)
        self.ui.Button_2sqrt.clicked.connect(self.buttonSqrt2)
        self.ui.Button_pow.clicked.connect(self.buttonPow)
        self.ui.Button_2pow.clicked.connect(self.buttonPow2)
        self.ui.Button_fac.clicked.connect(self.buttonFac)
        self.ui.Button_sin.clicked.connect(self.buttonSin)
        self.ui.Button_cos.clicked.connect(self.buttonCos)
        self.ui.Button_tan.clicked.connect(self.buttonTan)
        self.ui.Button_log.clicked.connect(self.buttonLog)
        self.ui.Button_ln.clicked.connect(self.buttonLn)
        self.ui.Button_abs.clicked.connect(self.buttonAbs)
        self.ui.Button_inverse.clicked.connect(self.buttonInv)
        self.ui.Button_percent.clicked.connect(self.buttonPer)
        self.ui.Button_ans.clicked.connect(self.buttonAns)
        self.ui.Button_m.clicked.connect(self.buttonMem)
        self.ui.Button_mdel.clicked.connect(self.buttonMDel)
        self.ui.Button_madd.clicked.connect(self.buttonMadd)
        self.ui.Button_msub.clicked.connect(self.buttonMsub)

        
        [getattr(self.ui,"Button_{0}".format(x)).pressed.connect(self.numClick) for x in "0123456789"] # Připojení číslic
        self.ui.Button_pi.pressed.connect(self.piClick) # Připojení pi
        self.ui.Button_e.pressed.connect(self.eClick) # Připojení e
        self.ui.Button_00.pressed.connect(self.doubleNulClick) # Připojení 00
        self.ui.Button_dot.pressed.connect(self.dotClick) # Připojení desetinné čárky
# ---------------------------------------------
# Klikání na čísla
# ---------------------------------------------
    ## Číslice
    def numClick(self):
        [self.appendDisplay(x) for x in "0123456789" if getattr(self.ui,"Button_{0}".format(x)).isDown()] # proskenuje všechyn čísla a zapíše číslo stisknutého

    ## Pi
    def piClick(self):
        if self.ui.Button_pi.isDown():
            self.clearDisplays()
            self.appendDisplay(str(amath.math.pi))

    ## e
    def eClick(self):
        if self.ui.Button_e.isDown():
            self.clearDisplays()
            self.appendDisplay(str(amath.math.e)) 

    ## 00
    def doubleNulClick(self):
        self.appendDisplay(str(0))
        self.appendDisplay(str(0)) 

    ## desetinní čárka
    def dotClick(self):
        global displayDot
        if displayDot:
            pass
        else:
            self.appendDisplay(".")
            displayDot = True
# ---------------------------------------------
# Ovládání klávesnicí
# ---------------------------------------------
    ## Override výchozího keyPressEvent
    #
    # Zjišťuje zmáčkunté tlačítka a vyvolává podle nich akce
    def keyPressEvent(self, e):
        if e.key() == QtCore.Qt.Key_Return: # enter = výpočet
            self.ui.Button_equals.animateClick()
        elif e.key() == QtCore.Qt.Key_Backspace: # backspace umaže poslední číslo
            self.ui.Button_del.animateClick()
        elif e.key() == QtCore.Qt.Key_Delete: # delete vyčistí všechny displeje, operaci, ...
            self.ui.Button_ac.animateClick()
        elif e.key() == QtCore.Qt.Key_Plus: 
            self.ui.Button_add.animateClick()
        elif e.key() == QtCore.Qt.Key_Minus: 
            self.ui.Button_sub.animateClick()
        elif e.key() == QtCore.Qt.Key_Asterisk: 
            self.ui.Button_mul.animateClick()
        elif e.key() == QtCore.Qt.Key_Slash:
            self.ui.Button_div.animateClick()
        elif e.key() == QtCore.Qt.Key_Enter:
            self.ui.Button_equals.animateClick()
        elif e.key() == QtCore.Qt.Key_Comma:
            self.ui.Button_dot.animateClick()
        elif e.key() == QtCore.Qt.Key_0:
            self.ui.Button_0.animateClick()
        elif e.key() == QtCore.Qt.Key_1:
            self.ui.Button_1.animateClick()
        elif e.key() == QtCore.Qt.Key_2:
            self.ui.Button_2.animateClick()
        elif e.key() == QtCore.Qt.Key_3:
            self.ui.Button_3.animateClick()
        elif e.key() == QtCore.Qt.Key_4:
            self.ui.Button_4.animateClick()
        elif e.key() == QtCore.Qt.Key_5:
            self.ui.Button_5.animateClick()
        elif e.key() == QtCore.Qt.Key_6:
            self.ui.Button_6.animateClick()
        elif e.key() == QtCore.Qt.Key_7:
            self.ui.Button_7.animateClick()
        elif e.key() == QtCore.Qt.Key_8:
            self.ui.Button_8.animateClick()
        elif e.key() == QtCore.Qt.Key_9:
            self.ui.Button_9.animateClick()
            
# ---------------------------------------------
# Posun okna
# ---------------------------------------------

    ## Override defaultní mousePressEvent funkce
    def framePressEvent(self, event):
        self.oldPos = event.globalPos()

    ## Override defaultní mouseMoveEvent funkce, aby bylo okno posuvné
    def frameMoveEvent(self, event):
        difference = QtCore.QPoint(event.globalPos() - self.oldPos)
        self.move(self.x() + difference.x(),self.y() + difference.y())
        self.oldPos = event.globalPos()

# ---------------------------------------------
# Pomocné funkce pro provoz kalkulačky
# ---------------------------------------------

    ## Funkce pro minimalizaci okna
    def minimize(self):
        self.showMinimized()

    ## Funkce pro zobrazování a skrývání převodníku měn
    def showHideCurrencies(self):
        global operationID
        if self.ui.Button_converter.isChecked():
            self.ui.currFrame.setVisible(True) 
            operationID = "conversion"
        else:
            self.ui.currFrame.setVisible(False)
            operationID = "empty"

    def setError(self, errid):
        global error
        error = True
        self.setDisplay(self.errorMessages[errid], 1)
        self.setDisplay(self.errorMessages["clearMsg"],2)
        self.ui.notify_E.setVisible(True)

    def clearError(self):
        global error
        error = False
        self.ui.notify_E.setVisible(False)      

    ## Funkce pro aktivaci nápovědy
    def helpOnOff(self):
        if QWhatsThis.inWhatsThisMode():
            QWhatsThis.leaveWhatsThisMode()
        else:
            QWhatsThis.enterWhatsThisMode()   

    ## Převod měn
    # 
    # Funkce načte číslo z displeje, pomocí knihovny převede měny, načte texty podle hodnot
    # a výsledek vypíše na všechyn části displeje
    def convertCurrency(self):
        global ansNum
        amount = float(self.getDisplay())
        self.clearDisplays()
        currFrom = self.ui.combo_currFrom.currentText()
        currTo = self.ui.combo_currTo.currentText()
        result = round(convertLib.convert(currFrom,currTo,amount),4)
        ansNum = result
        nameFrom = convertLib.getFullName(currFrom,amount)
        nameTo = convertLib.getFullName(currTo,result)
        self.setDisplay(result,0)
        self.setDisplay("{0} {1}".format(amount,nameFrom),1)
        self.setDisplay(nameTo,2)

    
    ## Obslužná funkce tlačítka Button_equals
    # 
    # @brief Funkce zavolá výpočet a vyresetuje kalkulačku pro další příklad
    def buttonCalculate(self):
        global operationID
        global operationPicked
        self.calculate()
        operationPicked = False
        operationID = "empty" 
        self.ui.Button_converter.setChecked(False)
        self.showHideCurrencies()  

    ## Výpočet Součtu dvou čísel
    # 
    # @brief Funkce načte druhou hodnotu, následně vypočítá součet hodnoty "amount" a "lastNum" (z funkce buttonAdd) a výsledek vypíše na display
    def calculateAdd(self):
        global operationPicked # načtení globálních proměnných
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay()) # načtení hodnoty z displaye
        result = amath.add(lastNum, amount) # zavolání funkce pro výpočet součtu
        lastNum = result
        ansNum = lastNum    
        self.clearDisplays()
        self.setDisplay(result,0) # zavolání funkce pro výpis výsledku na hlavní display

    ## obslužná funkce pro tlačítko Button_add
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonAdd(self):
        global operationPicked # načtení globálních proměnných
        global operationID
        global lastNum
        if operationPicked: # pokud už máme operaci vybranou, tak přes = zavoláme výpočet
            self.calculate()
        else: # jinak uložíme klinutí na tlačítko a nachystáme se na vlozeni dalsiho cisla
            lastNum = Decimal(self.getDisplay()) 
            operationPicked = True
            self.clearDisplays()
        operationID = "add"

    ## Výpočet rozdílu dvou čísel
    # 
    # @brief Funkce načte druhou hodnotu, následně vypočítá rozdíl hodnot "amount" a "lastNum" (z funkce buttonSub) a výsledek vypíše na display
    def calculateSub(self):
        global operationPicked 
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay())
        result = amath.sub(lastNum, amount)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_sub
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonSub(self):
        global operationPicked 
        global operationID
        global lastNum
        if operationPicked: 
            self.calculate()
        else: 
            lastNum = Decimal(self.getDisplay()) 
            operationPicked = True
            self.clearDisplays()
        operationID = "sub"

    ## Výpočet součinu dvou čísel
    # 
    # @brief Funkce načte druhou hodnotu, následně vypočítá součin hodnot "amount" a "lastNum" (z funkce buttonMul) a výsledek vypíše na display
    def calculateMul(self):
        global operationPicked 
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay())
        result = amath.mul(lastNum, amount)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_mul
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonMul(self):
        global operationPicked 
        global operationID
        global lastNum
        if operationPicked: 
            self.calculate()
        else: 
            lastNum = Decimal(self.getDisplay()) 
            operationPicked = True
            self.clearDisplays()
        operationID = "mul"

    ## Výpočet podílu dvou čísel
    # 
    # @brief Funkce načte druhou hodnotu, následně vypočítá podíl hodnot "amount" a "lastNum" (z funkce buttonDiv) a výsledek vypíše na display
    def calculateDiv(self):
        global operationPicked 
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay())
        result = amath.div(lastNum, amount)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_div
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonDiv(self):
        global operationPicked 
        global operationID
        global lastNum
        if operationPicked: 
            self.calculate()
        else: 
            lastNum = Decimal(self.getDisplay()) 
            operationPicked = True
            self.clearDisplays()
        operationID = "div"

    ## Výpočet odmocniny čísla
    # 
    # @brief Funkce načte druhou hodnotu, následně odmocní číslo "amount" číslem "lastNum" (z funkce buttonSqrt) a výsledek vypíše na display
    def calculateSqrt(self):
        global operationPicked 
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay())
        result = amath.sqrt(amount, lastNum)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_sqrt
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonSqrt(self):
        global operationPicked 
        global operationID
        global lastNum
        if operationPicked: 
            self.calculate()
        else: 
            lastNum = Decimal(self.getDisplay()) 
            operationPicked = True
            self.clearDisplays()
        operationID = "sqrt"

    ## Výpočet mocniny čísla
    # 
    # @brief Funkce načte druhou hodnotu, následně umocní číslo "amount" číslem "lastNum" (z funkce buttonSqrt) a výsledek vypíše na display
    def calculatePow(self):
        global operationPicked 
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay())
        result = amath.pow(lastNum, amount)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_pow
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonPow(self):
        global operationPicked 
        global operationID
        global lastNum
        if operationPicked: 
            self.calculate()
        else: 
            lastNum = Decimal(self.getDisplay()) 
            operationPicked = True
            self.clearDisplays()
        operationID = "pow"

    ## Výpočet faktoriálu čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá faktoriál čísla "amount" a výsledek vypíše na display
    def calculateFac(self):
        global operationPicked
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay())
        if int(amount) == amount:
            result = amath.fac(int(amount))
            lastNum = result
            ansNum = lastNum
            self.clearDisplays()
            self.setDisplay(result,0)
        else:
            raise TypeError

    ## obslužná funkce pro tlačítko Button_fac
    # 
    # @brief Funkce zavolá funkci "calculate"
    def buttonFac(self):
        global operationPicked
        global operationID
        operationPicked = True
        operationID = "fac"
        self.calculate()

    ## Výpočet sinus čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá sinus čísla "lastNum" a výsledek vypíše na display    
    def calculateSin(self):
        global operationPicked 
        global lastNum
        global ansNum
        lastNum = Decimal(self.getDisplay())
        result = round(amath.sin(lastNum),14)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)
    
    ## obslužná funkce pro tlačítko Button_sin
    # 
    # @brief Funkce zavolá funkci "calculate" nebo nastaví hodnotu "operarionPicked" na True a vymaže display
    def buttonSin(self):
        global operationPicked 
        global operationID
        if operationPicked:
            self.calculate()
        else:
            operationPicked = True
            self.clearDisplays()
        operationID = "sin"

    ## Výpočet cosinus čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá cosinus čísla "lastNum" a výsledek vypíše na display   
    def calculateCos(self):
        global operationPicked 
        global lastNum
        global ansNum
        lastNum = Decimal(self.getDisplay())
        result = round(amath.cos(lastNum),14)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_cos
    # 
    # @brief Funkce zavolá funkci "calculate" nebo nastaví hodnotu "operarionPicked" na True a vymaže display
    def buttonCos(self):
        global operationPicked 
        global operationID
        if operationPicked:
            self.calculate()
        else:
            operationPicked = True
            self.clearDisplays()
        operationID = "cos"

    ## Výpočet tangens čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá tangens čísla "lastNum" a výsledek vypíše na display
    def calculateTan(self):
        global operationPicked 
        global lastNum
        global ansNum
        lastNum = Decimal(self.getDisplay())
        result = round(amath.tan(lastNum),14)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_tan
    # 
    # @brief Funkce zavolá funkci "calculate" nebo nastaví hodnotu "operarionPicked" na True a vymaže display
    def buttonTan(self):
        global operationPicked 
        global operationID
        if operationPicked:
            self.calculate()
        else:
            operationPicked = True
            self.clearDisplays()
        operationID = "tan"

    ## Výpočet přirozeného logaritmu čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá přirozený logaritmus čísla "lastNum" a výsledek vypíše na display
    def calculateLn(self):
        global operationPicked 
        global lastNum
        global ansNum
        lastNum = Decimal(self.getDisplay())
        result = amath.ln(lastNum)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_ln
    # 
    # @brief Funkce zavolá funkci "calculate" nebo nastaví hodnotu "operarionPicked" na True a vymaže display
    def buttonLn(self):
        global operationPicked 
        global operationID
        if operationPicked:
            self.calculate()
        else:
            operationPicked = True
            self.clearDisplays()
        operationID = "ln"

    ## Výpočet logaritmu čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá logaritmus čísla "lastNum" o základu hodnoty "amount" a výsledek vypíše na display
    def calculateLog(self):
        global operationPicked 
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay())
        result = amath.log(lastNum, amount)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_log
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonLog(self):
        global operationPicked 
        global operationID
        global lastNum
        if operationPicked: 
            self.calculate()
        else: 
            lastNum = Decimal(self.getDisplay())
            operationPicked = True
            self.clearDisplays()
        operationID = "log"

    ## Výpočet druhé mocniny čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá druhou mocninu čísla "lastNum" a výsledek vypíše na display
    def calculatePow2(self):
        global operationPicked
        global lastNum
        global ansNum
        lastNum = Decimal(self.getDisplay())
        result = amath.pow(lastNum, 2)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_2pow
    # 
    # @brief Funkce nastaví hodnotu "operarionPicked" na True, nastaví hodnotu "operationID" na "pow2" a zavolá funkci "calculate"
    def buttonPow2(self):
        global operationPicked
        global operationID
        operationPicked = True
        operationID = "pow2"
        self.calculate()

    ## Výpočet druhé odmocniny čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá druhou odmocninu čísla "lastNum" a výsledek vypíše na display
    def calculateSqrt2(self):
        global operationPicked 
        global lastNum
        global ansNum
        lastNum = Decimal(self.getDisplay())
        result = amath.sqrt(lastNum, Decimal(2))
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_2sqrt
    # 
    # @brief Funkce zavolá funkci "calculate"
    def buttonSqrt2(self):
        global operationPicked 
        global operationID
        operationID = "sqrt2"
        self.calculate()

    ## Výpočet absolutní hodnoty čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá absolutní hodnotu čísla "lastNum" a výsledek vypíše na display
    def calculateAbs(self):
        global operationPicked 
        global lastNum
        global ansNum
        lastNum = Decimal(self.getDisplay())
        result = amath.abs(lastNum)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_abs
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonAbs(self):
        global operationPicked 
        global operationID
        operationID = "abs"
        self.calculate()

    ## Výpočet inverzní hodnoty čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá inverzní hodnotu čísla "lastNum" a výsledek vypíše na display
    def calculateInv(self):
        global operationPicked 
        global lastNum
        global ansNum
        lastNum = Decimal(self.getDisplay())
        result = amath.div(1, lastNum)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_inverse
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonInv(self):
        global operationPicked 
        global operationID
        operationID = "inv"
        self.calculate()


    ## Výpočet procentní hodnoty z čísla
    # 
    # @brief Funkce načte hodnotu z displaye, následně vypočítá kolik procent "amount" je ze základu čísla "lastNum" a výsledek vypíše na display
    def calculatePer(self):
        global operationPicked 
        global lastNum
        global ansNum
        amount = Decimal(self.getDisplay())
        result = amath.mul(amath.div(lastNum, amount), 100)
        lastNum = result
        ansNum = lastNum
        self.clearDisplays()
        self.setDisplay(result,0)

    ## obslužná funkce pro tlačítko Button_percent
    # 
    # @brief Funkce při prvním zavolání jakékoliv operace uloží první hodnotu pro výpočet nebo zavolá funkci "calculate"
    def buttonPer(self):
        global operationPicked 
        global operationID
        global lastNum
        if operationPicked and operationID is "per": 
            self.calculate()
        else: 
            lastNum = Decimal(self.getDisplay()) 
            operationPicked = True
            self.clearDisplays()
        operationID = "per"

    ## Vypsání posledního výsledku na display
    # 
    # @brief Funkce vypíše poslední výsledek na display
    def buttonAns(self):
        global ansNum
        self.setDisplay(ansNum,0)

    ## Uložení čísla z displaye do paměti
    # 
    # @brief Funkce vypíše hodnotu z proměnné "memNum" na display nebo načte hodnotu z displaye, kterou uloží do proměnné "memNum" a nastaví proměnnou "memSaved" na True
    def buttonMem(self):
        global memNum
        global memSaved
        if memSaved:
            self.setDisplay(memNum,0)
        else:
            memNum = Decimal(self.getDisplay())
            memSaved = True
            self.ui.notify_M.setVisible(True)
            

    ## Vymazání čísla z paměti
    # 
    # @brief Funkce vymaže/nastaví hodnotu proměnné "memNum" na 0 a nastaví hodnotu "memSaved" na False
    def buttonMDel(self):
        global memNum
        global memSaved
        memNum = 0
        memSaved = False
        self.ui.notify_M.setVisible(False)

    ## Přičítání k číslu z paměti
    # 
    # @brief Funkce přičítá číslo z displaye k číslu v paměti (proměnná "memNum")
    def buttonMadd(self):
        global memNum
        global memSaved
        if memSaved:
            memNum += Decimal(self.getDisplay())

    ## Odčítání od čísla z paměti
    # 
    # @brief Funkce odčítá číslo z displaye od čísla v paměti (proměnná "memNum")
    def buttonMsub(self):
        global memNum
        global memSaved
        if memSaved:
            memNum -= Decimal(self.getDisplay())
        
    ## Funkce, která nic nedělá
    # 
    # @brief funkce, která způsobuje, aby kalkulačka nic nedělala
    def doNothing(self):
        pass

    ## Nastaví hodnotu displeje
    # 
    # @param    value   hodnota k nastavení
    # @param    ID      ID displeje (nepovinné), 0=hlavní, 1=horní, 2=spodní
    def setDisplay(self, value, ID=0):
        global displayedNumber
        if ID == 0:
            try:
                Decimal(value)
            except:
                print("Error: Display ID 0 doesn't support non-float values.")
                return
            displayedNumber = str(Decimal(value))
            self.updateDisplay()
        elif ID == 1:
            self.ui.display_currencyFrom.setText(str(value))
        elif ID == 2:
            self.ui.display_currencyTo.setText(str(value))
        else:
            raise IndexError("ID {0} is not valid display ID.".format(ID))
    
    ## Zobrazí na displeji číslo z proměnné displayedNumber
    def updateDisplay(self):
        global displayedNumber
        global displayDot
        splitNumber = displayedNumber.split(".")
        wholePart = splitNumber[0]
        decimalPart = ""
        if len(splitNumber) > 1:
            decimalPart = splitNumber[1]
        # formátování výstupu podle délky čísla
        if (len(decimalPart) + len(wholePart) + 1) > 15: # dlouhé číslo
            # Naformátuje číslo tak, aby se vešlo na 16místný displej bez ořezů (16-znaménko-exponent-další znaky)
            parsedNumber = format(Decimal(displayedNumber),"1.{}G".format(16-len(str(ceil(amath.log(abs(Decimal(displayedNumber)),10))))-3))
            if parsedNumber.split(".")[1].count("0") == len(parsedNumber.split(".")[1]):
                parsedNumber = str(round(float(displayedNumber),0)).split(".")[0]
        elif (decimalPart == "" or decimalPart == "0") and not displayDot: # krátké celé číslo
            parsedNumber = wholePart 
        else: # krátké desetinné číslo
            parsedNumber = "{}.{}".format(wholePart,decimalPart) 
        self.ui.display.setText(parsedNumber)

    ## Načte hodnotu displeje
    #
    # @param    ID      ID displeje (nepovinné), 0=hlavní, 1=horní, 2=spodní
    def getDisplay(self, ID=0):
        global displayedNumber
        if ID == 0:
            return Decimal(displayedNumber)
        elif ID == 1:
            return self.ui.display_currencyFrom.text()
        elif ID == 2:
            return self.ui.display_currencyTo.text()
        else:
            raise IndexError("ID {0} is not valid display ID.".format(ID))

    ## Umaže poslední znak displeje
    def backspaceDisplay(self):
        global displayDot
        numParts = str(self.getDisplay()).split(".")
        if displayDot: # odmazávání za desetinnou čárkou
            if len(numParts[1]) == 1: # odmazání desetinné čárky
                self.setDisplay(numParts[0])
                displayDot = False
            else:
                self.setDisplay("{}.{}".format(numParts[0],numParts[1][:-1]))
        else: # mazání z celého čísla
            if numParts[0][:-1] == "" or self.getDisplay() == 0:
                self.clearDisplays()
            else:
                self.setDisplay(numParts[0][:-1])

    ## Vyresetuje vše do výchozího stavu
    def clearActions(self):
        global operationID
        global lastNum
        global operationPicked
        global displayDot
        operationID = "empty"
        lastNum = 0
        operationPicked = False
        displayDot = False
        if error:
            self.clearError()
        self.clearDisplays()
        self.ui.Button_converter.setChecked(False)
        self.showHideCurrencies()
    
    ## Vymaže obsah všech displejů
    #
    # @param    mode    udává mód mazání
    #                   0=hlavní displej maže na 0,
    #                   >0=maže na prázdno
    def clearDisplays(self, mode=0):
        global displayDot
        global displayedNumber
        global calculated
        if mode is 0:
            displayedNumber = "0"
            displayDot = False
            self.updateDisplay()
        else:
            displayedNumber = "0"
            displayDot = False
            self.ui.display.setText("")
        self.ui.display_currencyFrom.setText("")
        self.ui.display_currencyTo.setText("")
        calculated = False

    ## Připíše znak na konec displeje
    #
    # @param    value   znak, pro přípsání na displej
    def appendDisplay(self, value, ID=0):
        global displayedNumber
        global displayDot
        global calculated
        if calculated:
            if error:
                self.clearActions()
            self.clearDisplays()
        if ID == 0:
            if value == ".":
                displayDot = True
                displayedNumber += value
            elif displayedNumber == "0":
                displayedNumber = value
            else:
                displayedNumber += value
            self.updateDisplay()
        else:
            self.setDisplay(self.getDisplay(ID)+value,ID)

    ## Provede výpočet
    #
    # Funkce, která vykonává výpočet záleží na aktuálním obsahu proměnné operationID,
    # ve které by měl být řetězec s identifikátorem operace, který odpovídá klíči v
    # dictu self.operations
    # calculate nemaže obsah displeje, pouze spouští operaci po kliknutí na = nebo 
    # zmáčknutí klávesy Enter
    # 
    # @see operationID
    # @see MainWindow.operations
    def calculate(self):
        global operationID
        global calculated
        if operationID not in self.operations.keys():
            raise IndexError("Operation {0} is not a valid operation!".format(operationID))
        else:
            try:
                self.operations[operationID]()
            except TypeError:
                if operationID == "fac":
                    self.setError("facFloat")
            except ValueError:
                if operationID == "div":
                    self.setError("divNull")
                elif operationID == "sqrt" or operationID == "sqrt2":
                    self.setError("sqrtNeg")
                elif operationID == "fac":
                    self.setError("facNeg")
                elif operationID == "log" or operationID == "ln":
                    global lastNum
                    if lastNum < 0:
                        self.setError("logNeg")
                    else:
                        self.setError("logBase")
                elif operationID == "tan":
                    self.setError("tanBad")
                elif operationID == "inv":
                    self.setError("invZero")
            except:
                if operationID == "conversion":
                    self.setError("noInternet")

            calculated = True
            
            


# -----------------------------------------------------------------------------------------------

# -----------------------------------------------------------------------------------------------
# Spuštění programu
# -----------------------------------------------------------------------------------------------

def main():
    app = QApplication(sys.argv)
    appIcon = QtGui.QIcon()
    appIcon.addFile(iconFile, QtCore.QSize(363,512))
    app.setWindowIcon(appIcon)
    window = MainWindow()
    
    window.show()

    sys.exit(app.exec_())

if __name__ == "__main__":
    main()
