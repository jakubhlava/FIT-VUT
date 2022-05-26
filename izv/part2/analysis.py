#!/usr/bin/env python3.9
# coding=utf-8
from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns
import numpy as np
import os

# muzete pridat libovolnou zakladni knihovnu ci knihovnu predstavenou na prednaskach
# dalsi knihovny pak na dotaz

""" Ukol 1:
načíst soubor nehod, který byl vytvořen z vašich dat. Neznámé integerové hodnoty byly mapovány na -1.

Úkoly:
- vytvořte sloupec date, který bude ve formátu data (berte v potaz pouze datum, tj sloupec p2a)
- vhodné sloupce zmenšete pomocí kategorických datových typů. Měli byste se dostat po 0.5 GB. Neměňte však na kategorický typ region (špatně by se vám pracovalo s figure-level funkcemi)
- implementujte funkci, která vypíše kompletní (hlubkou) velikost všech sloupců v DataFrame v paměti:
orig_size=X MB
new_size=X MB

Poznámka: zobrazujte na 1 desetinné místo (.1f) a počítejte, že 1 MB = 1e6 B. 
"""

cat_blacklist = {"p1", "p2a", "p2b", "p13a", "p13b", "p13c", "p14", "p34", "p53", "a",
                  "b", "d", "e", "f", "g", "j", "l", "n", "o", "r", "s", "region", "date"}


def get_dataframe(filename: str, verbose: bool = False) -> pd.DataFrame:
    df = pd.read_pickle(filename)
    print(f"orig_size={df.memory_usage(deep=True).sum()/1e6:.1f} MB")
    df["date"] = pd.to_datetime(df["p2a"])
    categorizable = set(df.columns) - cat_blacklist
    for cat in categorizable:
        df[cat] = df[cat].astype("category")
    print(f"new_size={df.memory_usage(deep=True).sum() / 1e6:.1f} MB")
    return df

# Ukol 2: počty nehod v jednotlivých regionech podle druhu silnic

def plot_roadtype(df: pd.DataFrame, fig_location: str = None,
                  show_figure: bool = False):
    cats = {
        1: "Dvoupruhová komunikace",
        2: "Třípruhová komunikace",
        3: "Čtyřpruhová komunikace",
        4: "Čtyřpruhová komunikace",
        5: "Vícepruhová komunikace",
        6: "Rychlostní komunikace",
        0: "Jiná komunikace"
    }
    order = {
        "Dvoupruhová komunikace": 0,
        "Třípruhová komunikace": 1,
        "Čtyřpruhová komunikace": 2,
        "Vícepruhová komunikace": 3,
        "Rychlostní komunikace": 4,
        "Jiná komunikace": 5
    }
    df["p21"] = df["p21"].map(cats)

    aggregated_df = df[df["region"].isin(["OLK", "JHM", "ZLK", "MSK"])].sort_values(["p21"])\
        .groupby(["region", "p21"]).size().reset_index(name="count")
    sns.set(rc={'axes.facecolor': '#303030'})
    g = sns.catplot(data=aggregated_df, col="p21", y="count", x="region", hue="region", legend_out=True, kind="bar",
                    col_wrap=3, height=3, aspect=0.75, dodge=False, col_order=order)
    (g.set_axis_labels("Kraj", "Počet nehod").set_titles("{col_name}").add_legend())
    if fig_location:
        g.fig.savefig(fig_location)
    if show_figure:
        plt.show()

# Ukol3: zavinění zvěří
def plot_animals(df: pd.DataFrame, fig_location: str = None,
                 show_figure: bool = False):
    cats = {
        1: "řidičem",
        2: "řidičem",
        3: "jiné",
        4: "zvěří",
        5: "jiné",
        6: "jiné",
        7: "jiné",
        0: "jiné",
    }
    df["p10"] = df["p10"].map(cats)
    df.drop(df.index[df["date"] >= np.datetime64("2021-01-01")])
    df["month"] = df["date"].dt.month
    aggregated_df = df[df["region"].isin(["OLK", "JHM", "ZLK", "MSK"])].groupby(["region", "month", "p10"]).size().reset_index(name="count")
    aggregated_df = aggregated_df.rename(columns={"p10": "Zavinění"})
    print(aggregated_df.columns)
    g = sns.catplot(data=aggregated_df, col="region", x="month", y="count", hue="Zavinění", sharex=False, margin_titles=True,
                    hue_order=["řidičem", "zvěří", "jiné"], legend_out=True, kind="bar", col_wrap=2, height=3.5, aspect=2)
    (g.set_axis_labels("Měsíc", "Počet nehod").set_titles("{col_name}").set_facecolor("#808080"))
    if fig_location:
        g.fig.savefig(fig_location)
    if show_figure:
        plt.show()

# Ukol 4: Povětrnostní podmínky
def plot_conditions(df: pd.DataFrame, fig_location: str = None,
                    show_figure: bool = False):
    pass

if __name__ == "__main__":
    # zde je ukazka pouziti, tuto cast muzete modifikovat podle libosti
    # skript nebude pri testovani pousten primo, ale budou volany konkreni ¨
    # funkce.
    df = get_dataframe("accidents.pkl.gz") # tento soubor si stahnete sami, při testování pro hodnocení bude existovat
    plot_roadtype(df, fig_location="01_roadtype.png", show_figure=True)
    #plot_animals(df, "02_animals.png", True)
    plot_conditions(df, "03_conditions.png", True)
