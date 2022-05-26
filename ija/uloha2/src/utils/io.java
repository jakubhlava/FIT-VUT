package utils;

import java.nio.file.Files;
import java.nio.file.Paths;

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
}
