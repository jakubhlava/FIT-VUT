package utils;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;

/**
 * Třída pro pomocné funkce
 * @author  Jakub Hlava (xhlava52)
 * @author  Lukáš Kraus (xkraus13)
 */
public class io {

    /**
     * Funkce načte soubor a vrátí ho ve stringu
     * @param   file        cesta k souboru
     * @return  obsah souboru
     * @throws Exception    pokud se soubor nepodaří najít, otevřít nebo přečíst
     */
    public static String readFile(String file) throws Exception
    {
        return new String(Files.readAllBytes(Paths.get(file)));
    }

    /**
     * Zapíše do souboru, pokud neexistuje, pak ho vytvoří, pokud existuje, pak ho přepíše
     * @param file          cesta k souboru
     * @param content       obsah k zapsání
     * @throws Exception    pokud se nepodařízapsat soubor
     */
    public static void writeFile(String file, String content) throws Exception {
        Files.writeString(Paths.get(file),content, StandardOpenOption.CREATE, StandardOpenOption.WRITE, StandardOpenOption.TRUNCATE_EXISTING);
    }

    /**
     * Vrací lineární pozici v poli podle souřadnic
     * @param posX      svislá souřadnice
     * @param posY      vodorovná souřadnice
     * @param y_size    šířka mapy
     * @return  pozice v poli políček mapy
     */
    public static int getPosInArray(int posX, int posY, int y_size){
        return y_size * posX + posY;
    }
}
