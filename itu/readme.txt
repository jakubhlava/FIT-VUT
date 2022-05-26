Vzhledem k tomu, že výstupem našeho projektu je webová aplikace, umístili jsme její referenční kopii na server https://itu.jakubh.cz/
Pro osobní testování a zhodnocení je ovšem možné ji podle následujícího návodu zprovoznit i lokálně.

Popis instalace:

    Prerekvizity:
        - Python 3 vč. správce modulů pip
            - testováno pod verzemi 3.8, 3.9 a 3.10 (referenční implementace běží pod verzí 3.10)
        - MongoDB server, pro nejlepší výkon lokálně instalovaný (např. podle oficiální dokumentace zde https://docs.mongodb.com/guides/server/install/)
            - referenční implementace využívá verzi MongoDB 4.4.10 Community, na jiných verzích program nebyl testován

    Instalace:
        - rozbalte zdrojové soubory z archivu
        - v kořenové složce spusťte instalaci závislostí příkazem "pip install -e ."
            - pozn. tento příkaz se může lišit podle vašeho OS a instalace Pythonu,
              některé distribuce pro python3 pip využívají příkaz pip3
              jistotou je spustit pip pomocí vaší vybrané instalace, např. "python3.10 -m pip install -e ."
        - v MongoDB vytvořte databázi pro data aplikace, viz https://www.mongodb.com/basics/create-database
        - upravte konfiguraci v souboru soubor itu_app/settings.json
            - v položce uri vyplňte MongoDB Connection String odpovídající vašemu uživateli a instalaci
            - v položce db vyplňte název databáze
            - v položce secret_key vyplňte náhodný řetězec pro hashování cookies
        - konečně by mělo být možné aplikaci spustit v kořenové složce příkazem "python itu_app"
            - pozn. opět se příkaz pro spuštění Pythonu může lišit podle OS a instalace,
              postupujte podle vaší konkrétní konfigurace