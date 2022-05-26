#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import argparse
import os

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors
# povolene jsou pouze zakladni knihovny (os, sys) a knihovny numpy, matplotlib a argparse
from download import DataDownloader


def plot_stat(data_source,
              fig_location=None,
              show_figure=False):
    """Načte data, vytvoří z nich matice pro graf a vykreslí graf"""
    causes = {}
    for reg in DataDownloader.regions:  # vygenerování prázdných sloupců grafu
        causes[reg] = np.zeros((6,), dtype=np.uint32)
    for i in range(6): # sečtení a rozřazení počtů nehod ke krajům a příčinám
        indices = np.where(data_source["p24"] == i)
        region_vals = data_source["region"][indices]
        regions, counts = np.unique(region_vals, return_counts=True)
        causes_matrix = np.asarray((regions, counts)).T
        for row in causes_matrix:
            j = (i-1) % 6
            causes[row[0]][j] = row[1]
    output = np.empty((14, 6), dtype=np.uint32)
    sorted_regions = sorted(causes.keys())
    for i in range(len(sorted_regions)):
        output[i] = causes[sorted_regions[i]]
    accidents_sum = np.sum(output, axis=0)

    relative = output * 100 / accidents_sum # výpočet relativních hodnot vůči příčinám
    relative = np.ma.masked_where(relative == 0, relative) # vytvoření bílých políček
    fig = plt.figure(figsize=(10,8))
    ax = fig.add_subplot(2, 1, 1)
    ax.set_title("Absolutně")
    im = ax.imshow(output.T, norm=colors.LogNorm())
    ylabels = [
        "Přerušovaná žlutá",
        "Semafor mimo provoz",
        "Dopravní značky",
        "Přenosné dopravní značky",
        "Nevyznačena",
        "Žádná úprava"
    ]
    ax.set_xticklabels(sorted(DataDownloader.regions.keys()))
    ax.set_xticks([i for i in range(14)])
    ax.set_yticklabels(ylabels)
    ax.set_yticks([i for i in range(6)])
    bar = fig.colorbar(im, ax=ax)
    bar.set_label("Počet nehod")
    ax = fig.add_subplot(2, 1, 2)
    ax.set_title("Relativně vůči příčině")
    im = ax.imshow(relative.T, cmap="plasma")
    ax.set_xticklabels(sorted(DataDownloader.regions.keys()))
    ax.set_xticks([i for i in range(14)])
    ax.set_yticklabels(ylabels)
    ax.set_yticks([i for i in range(6)])
    bar = fig.colorbar(im, ax=ax)
    bar.set_label("Podíl nehod pro danou příčinu [%]")
    plt.tight_layout()
    if show_figure:
        plt.show()
    if fig_location is not None:
        directory, _ = os.path.split(fig_location)
        os.makedirs(directory, exist_ok=True)
        fig.savefig(fig_location)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Vizualizuje data o nehodách pro jednotlivé kraje")
    parser.add_argument("--fig_location", default=None, help="Soubor pro uložení výstupního grafu")
    parser.add_argument("--show_figure", default=False, action="store_true", help="Zobrazit okno s grafem")
    args = parser.parse_args()
    plot_stat(DataDownloader().get_dict(), args.fig_location, args.show_figure)
