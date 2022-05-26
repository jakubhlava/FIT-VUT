#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import numpy as np
import zipfile
import os
import requests
from bs4 import BeautifulSoup
import re
from zipfile import ZipFile
from datetime import datetime
import csv
from io import TextIOWrapper
import pickle
import gzip


# Kromě vestavěných knihoven (os, sys, re, requests …) byste si měli vystačit s: gzip, pickle, csv, zipfile, numpy, matplotlib, BeautifulSoup.
# Další knihovny je možné použít po schválení opravujícím (např ve fóru WIS).


class DataDownloader:
    """ TODO: dokumentacni retezce 

    Attributes:
        headers         Nazvy hlavicek jednotlivych CSV souboru, tyto nazvy nemente!
        header_types    Datovy typy prirazene k jednotlivym hlavickam dle obsahu
        header_with_types   Zipovane pary datovych typu a hlavicek
        regions         Dictionary s nazvy kraju : nazev csv souboru
        data_mem_cache  Cache v paměti
    """

    headers = ["p1", "p36", "p37", "p2a", "weekday(p2a)", "p2b", "p6", "p7", "p8", "p9", "p10", "p11", "p12", "p13a",
               "p13b", "p13c", "p14", "p15", "p16", "p17", "p18", "p19", "p20", "p21", "p22", "p23", "p24", "p27",
               "p28",
               "p34", "p35", "p39", "p44", "p45a", "p47", "p48a", "p49", "p50a", "p50b", "p51", "p52", "p53", "p55a",
               "p57", "p58", "a", "b", "d", "e", "f", "g", "h", "i", "j", "k", "l", "n", "o", "p", "q", "r", "s", "t",
               "p5a"]

    header_types = [
        np.uint64, np.uint8, np.uint32, np.datetime64, np.uint8, np.datetime64, np.uint8, np.uint8, np.uint8, np.uint8,
        np.uint8, np.uint8, np.uint16, np.uint32, np.uint32, np.uint32, np.float_, np.uint8, np.uint8, np.uint8,
        np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8,
        np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8, np.uint8,
        np.float_, np.uint8, np.uint8, np.uint8, np.str_, np.str_, np.float_, np.float_, np.str_, np.str_, np.str_,
        np.str_,
        np.str_, np.str_, np.str_, np.str_, np.str_, np.str_, np.str_, np.str_, np.str_, np.str_, np.uint8
    ]

    headers_with_types = [x for x in zip(headers, header_types)]

    regions = {
        "PHA": "00",
        "STC": "01",
        "JHC": "02",
        "PLK": "03",
        "ULK": "04",
        "HKK": "05",
        "JHM": "06",
        "MSK": "07",
        "OLK": "14",
        "ZLK": "15",
        "VYS": "16",
        "PAK": "17",
        "LBK": "18",
        "KVK": "19",
    }

    data_mem_cache = {}

    def __init__(self, url="https://ehw.fit.vutbr.cz/izv/", folder="data", cache_filename="data_{}.pkl.gz"):
        self.url = url
        self.folder = folder
        self.cache_filename = cache_filename
        if not os.path.exists(self.folder):
            os.mkdir(self.folder)

    def download_data(self):
        """Stáhne zipy s daty o nehodách a uloží je do složky dané atributem objektu folder"""
        page = requests.get(self.url).text
        soup = BeautifulSoup(page, "html.parser")
        # protože soubory jsou kumulativní, vždy postačí stáhnout soubor, na který odkazuje poslední dlačítko v řádku
        rows = soup.find_all("tr")
        zip_btns = []
        for row in rows:
            btns = row.find_all("button", string="ZIP")
            zip_btns.append(btns[-1])
        pathre = re.compile(r"'(.*\.zip)'")  # získám z javascriptu v onclick pouze cestu k .zip souboru
        for btn in zip_btns:
            path = pathre.search(btn.get("onclick")).group(1)
            fname = path.split("/")[-1]
            if not os.path.exists(os.path.join(self.folder, fname)):
                file = requests.get(self.url + path)
                with open(os.path.join(self.folder, fname), "wb") as f:
                    f.write(file.content)

    def parse_region_data(self, region):
        """Zpracuje data ze zip archivů, vytvoří z nich cache soubory a uloží je na disk, vrátí slovník dle zadání"""
        self.download_data()
        files = [f for f in os.listdir(self.folder) if f.endswith(".zip")]
        content = []
        for f in files:
            with ZipFile(os.path.join(self.folder, f), "r") as zf:  # otevření zip souboru
                with zf.open(f"{self.regions[region]}.csv") as csvfile:  # otevření jednoho csv bez rozbalení zbytku
                    # převod file-like objectu z zf.open na TextIOWrapper akceptovaný csv knihovnou
                    r = csv.reader(TextIOWrapper(csvfile, "cp1250"), delimiter=";")
                    content.extend([line for line in r])

        arrsize = len(content)  # velikost pro alokaci ndarray
        outdict = {}
        for header, type in self.headers_with_types:
            if header == "p2a":
                outdict[header] = np.ndarray((arrsize,), dtype="datetime64[D]")
            elif header == "p2b":
                outdict[header] = np.ndarray((arrsize,), dtype="datetime64[m]")
            else:
                outdict[header] = np.ndarray((arrsize,), dtype=type)
        outdict["region"] = np.array([region for _ in range(arrsize)], dtype=np.str_)

        ctr = 0
        for l in content:
            for (header, type_), val in zip(self.headers_with_types, l):
                if type_ is np.uint8 or type_ is np.uint16 or type_ is np.uint32 or type_ is np.uint64:
                    if val == "" or val == "XX":  # vyřazení neplatných hodnot
                        val = -1
                    else:
                        try:
                            val = int(val)
                        except Exception as e:
                            val = -1
                elif type_ is np.float64:
                    if val == "" or val == "XX":  # vyřazení neplatných hodnot
                        val = 0.0
                    else:
                        val = val.replace(",", ".")  # převod českých fp na anglické
                        try:
                            val = float(val)
                        except Exception as e:
                            val = float("NaN")
                if header == "p2b":  # vyřazení neplatných hodnot času
                    if val[:2] == "25":
                        val = np.datetime64("nat")
                    elif val[2:] == "60":
                        val = val[:2] + "00"
                        val = datetime.strptime(val, "%H%M")
                        val = np.datetime64(val)
                    else:
                        val = datetime.strptime(val, "%H%M")
                        val = np.datetime64(val)
                else:
                    val = type_(val)
                outdict[header][ctr] = val
            ctr += 1
        # deduplikace dat
        _, indices = np.unique(outdict["p1"], return_index=True)
        for column in outdict.keys():
            new = outdict[column][indices]
            outdict[column] = new

        return outdict

    def get_dict(self, regions=None):
        """Načte kraje v regions buď z cache nebo vyvolá zpracování dat ze souboru"""

        if not regions:
            regions = self.regions.keys()

        combined = None
        for r in regions:
            if r in self.data_mem_cache.keys():
                region_data = self.data_mem_cache[r]
            elif os.path.exists(self.get_cache_path(r)):
                with open(self.get_cache_path(r), "rb") as f:
                    zipped = f.read()
                unzipped = gzip.decompress(zipped)
                region_data = pickle.loads(unzipped)
                self.data_mem_cache[r] = region_data
            else:
                region_data = self.parse_region_data(r)
                pickled = pickle.dumps(region_data)
                zipped = gzip.compress(pickled)
                with open(self.get_cache_path(r), "wb") as f:
                    f.write(zipped)
                self.data_mem_cache[r] = region_data
            if combined is None:
                combined = region_data
            else:
                for k in combined.keys():
                    combined[k] = np.concatenate((combined[k], region_data[k]))
        return combined

    def get_cache_path(self, region):
        """Pomocná funkce pro získávání cest k cache souborům"""
        return os.path.join(self.folder, self.cache_filename.format(region))


# TODO vypsat zakladni informace pri spusteni python3 download.py (ne pri importu modulu)

if __name__ == "__main__":
    downloader = DataDownloader()
    data = downloader.get_dict(["JHM", "OLK", "ZLK"])
    print("-" * int(os.get_terminal_size().columns-1))
    print("Získaná data".center(int(os.get_terminal_size().columns-1)))
    print("-" * int(os.get_terminal_size().columns-1))
    print("Sloupce dat: ", end="")
    for k in data.keys():
        print(k, end=" ")
    print()
    print(f"Počet záznamů: {data['region'].shape[0]}")
    print("Kraje v datasetu: ")
    for k in np.unique(data["region"]):
        print(k, end=" ")
