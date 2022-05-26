package storage;

import com.google.gson.Gson;
import utils.io;


/**
 * Třída pro sestavení skladu ze souboru
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class storageBuilder {
    private String filename;

    /**
     * Konstruktor třídy pro sestavení skladu
     * @param filename  cesta k souboru, obsahující definici skladu
     */
    public storageBuilder(String filename) {
        this.filename = filename;
    }

    /**
     * Sestaví sklad ze souboru pomocí knihovny GSON
     * @return              sestavený sklad
     * @throws Exception    pokud soubor neexistuje
     *
     */
    public storage buildFromFile() throws Exception {
        String file;
        try {
            file = io.readFile(filename);
        } catch (Exception e) {
            throw new Exception("Nepodařilo se načíst konfiguraci skladu ze souboru "+filename);
        }
        Gson gson = new Gson();
        storage stor = gson.fromJson(file, storage.class);
        if (stor.integrityCheck()) {
            return stor;
        } else {
            throw new Exception("Soubor "+ filename + " neobsahuje korektní definici skladu!");
        }
    }

}
