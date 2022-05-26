import numpy as np
from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns

# podíl viníků prokazatelně pod vlivem alkoholu
df = pd.read_pickle("accidents.pkl.gz")
df["year"] = pd.to_datetime(df["p2a"]).dt.year
mapping = {  # zjednodušení sloupce p9
    # hodnota pod 0.24 promile je pod fyziologickou hladinou a toleranci pristroje, neprokazuje tedy vliv alkoholu
    1: "ne",
    2: "ne",
    3: "ano",
    6: "ano",
    7: "ano",
    8: "ano",
    9: "ano",
    0: "nezjištěno",
    4: "drogy",
    5: "drogy"
}
df = df[df["year"] < 2021] # odfiltrování neúplného roku 2021
df["p11"] = df["p11"].map(mapping)
grouped = df.groupby(["year", "p11"]).size().reset_index(name="count")
grouped["relative"] = grouped["count"] / grouped.groupby("year")["count"].transform("sum") * 100 # poměry v rámci let

# tabulka s údaji o počtech nehod s porkázaným alkoholem nebo drogami
print("Rok\tVliv alkoholu\tVliv drog\tStřízliví\tNezjištěno")
for year in grouped.year.unique():
    params = grouped[grouped.year == year]
    print(year, end="\t")
    for i in range(4):
        print(f"{params.iloc[i, 2]} ({round(params.iloc[i, 3], 2)}%)", end="\t")
    print()

# výpočet poměru nehod se zraněním vůči nehodám pouze s materiální škodou
avg_injuries = df.groupby(["p11", "p9"]).size().reset_index(name="count")
# poměry v rámci kategorií
avg_injuries["relative"] = avg_injuries["count"] / avg_injuries.groupby("p11")["count"].transform("sum") * 100
# přejmenování kvůli legendě
avg_injuries["p9"] = avg_injuries["p9"].map({1: "Následky na životě", 2: "Pouze hmotné škody"})
g_chn = sns.catplot(x="p11", y="relative", hue="p9", data=avg_injuries, kind="bar", legend=False, palette="viridis")
(g_chn.set_axis_labels("Stav viníka", "Podíl z nehod [%]")
 .set_xticklabels(["Pod vlivem\nalkoholu", "Pod vlivem\ndrog", "Střízlivý", "Nezjištěno"])
 .set(yticks=np.arange(0, 100, 5)))
plt.legend(loc="upper left", title="Následek nehody")
plt.savefig("fig.png")

# klasifikace viníků
p10_map = {
    1: "řidič",
    2: "řidič nemotorového",
    3: "chodec",
    5: "jiný"
}
# filtrace nehod, kde byl viník pod vlivem alkoholu a patří do kategorií výše
filtered = df[(df["p11"].isin(["ano"])) & (df["p10"].isin([1, 2, 3, 5]))]
filtered["p10"] = filtered["p10"].map(p10_map)
drunk_categories = filtered.groupby(["p10"]).size().reset_index(name="count")
drunk_categories["relative"] = drunk_categories["count"] / sum(drunk_categories["count"]) * 100
# Výpis hodnot pro použití v textu
print("Statistiky 2016 - 2020")
print(f"Nehod pod vlivem alkoholu: {sum(drunk_categories['count'])}")
print(f"Viníci v motorových vozidlech: {drunk_categories.iloc[2, 1]} ({round(drunk_categories.iloc[2, 2], 2)} %)")
print(f"Viníci v nemotorových vozidlech (cyklisti, ...):"
      f" {drunk_categories.iloc[3, 1]} ({round(drunk_categories.iloc[3, 2], 2)} %)")
print(f"Viníkem opilý chodec: {drunk_categories.iloc[0, 1]} ({round(drunk_categories.iloc[0, 2], 2)} %)")
print(f"Viníkem jiný účastník provozu: {drunk_categories.iloc[1, 1]} ({round(drunk_categories.iloc[1, 2], 2)} %)")
