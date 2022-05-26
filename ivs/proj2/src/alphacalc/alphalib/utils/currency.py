## 
# @file currency.py
# @brief Modul pro převod měn pro kalkulačku Alphamath
# @author Jakub Hlava (xhlava52)
# @date 04/2020

import requests
import json

## Seznam podporovaných měn
currencies = ["EUR","CZK","USD","GBP","RUB","VND","PLN","CHF","HUF","CNY","JPY"]
## Slovník jmen podporovaných měn včetně množných čísel
# 
# dict, který obsahuje pro zkratky měn tuple o 3 prvcích - název pro 1 ks, pro 2-4 ks, pro 5 a více ks
currencyNames = {
    "EUR": ("euro", "eura", "eur"),
    "CZK": ("česká koruna", "české koruny", "českých korun"),
    "USD": ("americký dolar", "americké dolary", "amerických dolarů"),
    "GBP": ("britská libra", "britské libry", "britských liber"),
    "RUB": ("ruský rubl", "ruské rubly", "ruských rublů"),
    "VND": ("vietnamský dong", "vietnamské dongy", "vietnamských dongů"),
    "PLN": ("polský zlotý", "polské zloté", "polských zlotých"),
    "CHF": ("švýcarský frank", "švýcarské franky", "švýcarských franků"),
    "HUF": ("maďarský forint", "maďarské forinty", "maďarských forintů"),
    "CNY": ("čínský juan", "čínské juany", "čínských juanů"),
    "JPY": ("japonský jen", "japonské jeny", "japonských jenů")
}

ratesCache = dict()

## Funkce pro načtení kurzů všech podporovaných měn
# 
# Funkce načte pro každou měnu její kurzovní lístek ve formátu JSON, 
# tento zpracuje a vybere z něj převodní kurzy ke všem ostatním měnám,
# které jsou knihovnou podporované.
# @param    currencyFrom    výchozí měna
# @param    currencyTo      cílová měna
#
# @return   rateDict        dict, jehož prvky jsou typu dict, které obsahují převodní kurzy,
#                           klíče obou dictů jsou zkratky měn.
def getExchangeRate(currencyFrom,currencyTo):
    if (currencyFrom,currencyTo) in ratesCache:
        return ratesCache[(currencyFrom,currencyTo)]
    feed = requests.get("http://floatrates.com/daily/{0}.json".format(currencyFrom.lower())).text
    data = json.loads(feed)
    rate = data[currencyTo.lower()]["rate"]
    ratesCache[(currencyFrom,currencyTo)] = rate
    return rate

## Funkce pro převod mezi měnami
# 
# @param    currencyFrom    zkratka výchozí měny (např. "EUR" - podle currencies)
# @param    currencyTo      zkratka cílové měny
# @param    amount          množství výchozí měny
# @param    rateDict        dict s převodními kurzy měn
#
# @return   changed         množství cílové měny
# @see      getExchangeRates()
# @see      currencies
def convert(currencyFrom, currencyTo, amount):
    rate = getExchangeRate(currencyFrom,currencyTo)
    changed = amount*rate
    return changed

## Funkce pro zjištění celého jména měny
# 
# Vrací český název měny ve správném tvaru podle množství
# @param    currency    zkratka měny
# @param    amount      množství
#
# @return   plný název měny (str)
# @see      currencies
# @see      currencyNames
def getFullName(currency, amount):
    if currency not in currencyNames.keys():
        return "CHYBA: Nepodporovaná měna!"
    if amount == 1:
        return currencyNames[currency][0]
    elif amount > 1 and amount < 5:
        return currencyNames[currency][1]
    else:
        return currencyNames[currency][2]
