#!/usr/bin/python3.8
# coding=utf-8
import pandas as pd
import geopandas
import matplotlib.pyplot as plt
import contextily as ctx
import sklearn.cluster
import numpy as np
from sklearn.cluster import MiniBatchKMeans

# muzete pridat vlastni knihovny


def make_geo(df: pd.DataFrame) -> geopandas.GeoDataFrame:
    """ Konvertovani dataframe do geopandas.GeoDataFrame se spravnym kodovani"""
    df = df[pd.notnull(df.d) & pd.notnull(df.e)]  # odstranění nehod, které nemají polohu
    df["year"] = pd.to_datetime(df["p2a"]).dt.year  # pomocný sloupeček s rokem
    return geopandas.GeoDataFrame(df, geometry=geopandas.points_from_xy(df["d"], df["e"]), crs="EPSG:5514")


def plot_geo(gdf: geopandas.GeoDataFrame, fig_location: str = None,
             show_figure: bool = False):
    """ Vykresleni grafu s sesti podgrafy podle lokality nehody 
     (dalnice vs prvni trida) pro roky 2018-2020 """
    fig, axs = plt.subplots(3, 2, figsize=(10, 9), constrained_layout=True,
                            sharex=True, sharey=True, subplot_kw=dict(aspect="equal"))
    g_years = [2018, 2019, 2020]        # roky k vykreslení
    g_p36 = [0, 1]                      # hodnoty p36 pro jednotlivé sloupce subplotu
    g_color = ["g", "r"]                # barvy bodů pro sloupce
    g_p36_user = ["dálnice", "silnice 1. třídy"]    # uživatelsky přívětivé popisky sloupců
    # filtrace geodataframu na pouze požadovaná data
    filtered = gdf[(gdf["region"] == "JHM") & gdf["year"].isin([2018, 2019, 2020]) & gdf["p36"].isin([0, 1])]
    filtered = filtered[["year", "p36", "geometry"]]
    # limity grafu +- 10km od nejzašších souřadnic jednotlivých nehod, aby body neležely až v okraji
    xlimits = np.min(filtered.bounds)["minx"]-10000, np.max(filtered.bounds)["maxx"]+10000
    ylimits = np.min(filtered.bounds)["miny"]-10000, np.max(filtered.bounds)["maxy"]+10000
    # vykreslení 6 podgrafů podle parametrů
    for i in range(3):
        for j in range(2):
            # parametry podgrafu
            axs[i, j].set_title(f"JHM kraj: {g_p36_user[j]}, rok {g_years[i]}")
            axs[i, j].set_xlim(xlimits)
            axs[i, j].set_ylim(ylimits)
            # vykreslení ještě jednou přefiltrovaného geodataframu
            filtered[(filtered["year"] == g_years[i]) & (filtered["p36"] == g_p36[j])].plot(ax=axs[i, j], markersize=3,
                                                                                            color=g_color[j])
            # přidání podkladové mapy
            ctx.add_basemap(axs[i, j], crs=filtered.crs.to_string(), source=ctx.providers.Stamen.TonerLite,
                            attribution_size=2)
            axs[i, j].axis("off")
    # uložení/zobrazení
    if fig_location:
        fig.savefig(fig_location)
    if show_figure:
        plt.show()


def plot_cluster(gdf: geopandas.GeoDataFrame, fig_location: str = None,
                 show_figure: bool = False):
    """ Vykresleni grafu s lokalitou vsech nehod v kraji shlukovanych do clusteru """
    fig, ax = plt.subplots(1, 1, figsize=(8, 6))
    plt.title("Nehody v Jihomoravském kraji na silnicích 1. třídy")
    ax.axis("off")
    # vyfiltrování JHM kraje a silnic 1. třídy
    filtered = gdf[(gdf["region"] == "JHM") & (gdf["p36"] == 1)]
    # extrakce souřadnic z geodataframu ve formátu vhodném pro clusterovací funkci
    coords_for_kmeans = np.dstack((filtered.geometry.x, filtered.geometry.y)).reshape(-1, 2)
    # hledání clusterů pomocí K-Means, 40 clusterů vizuálně vypadá rozumně, 20 clusterů tvořilo příliš velké úseky
    model = MiniBatchKMeans(n_clusters=40).fit(coords_for_kmeans)
    filtered["cluster"] = model.labels_     # přidání sloupce s příslušností ke clusteru
    # agregace podle počtu nehod v clusterech, přidání sloupců s počtem nehody v clusterech
    filtered = filtered.dissolve(by="cluster", aggfunc={"region": "count"}).rename(columns=dict(region="cnt"))
    filtered.plot(ax=ax, column="cnt", cmap="viridis", legend=False, markersize=3)
    # vykreslení zmenšeného colorbaru s popiskem
    cbar = plt.colorbar(ax.collections[0], ax=ax, location="bottom", shrink=0.6)
    cbar.set_label("Počet nehod v úseku")
    plt.tight_layout()
    # podkladová mapa
    ctx.add_basemap(ax, crs=filtered.crs.to_string(), source=ctx.providers.Stamen.TonerLite, attribution_size=2)
    # uložení/zobrazení
    if fig_location:
        fig.savefig(fig_location)
    if show_figure:
        plt.show()


if __name__ == "__main__":
    # zde muzete delat libovolne modifikace
    gdf = make_geo(pd.read_pickle("accidents.pkl.gz"))
    plot_geo(gdf, "geo1.png", True)
    plot_cluster(gdf, "geo2.png", True)
