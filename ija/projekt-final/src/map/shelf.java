package map;

/**
 * Class regálu
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class shelf {
    final private int ID;

    /**
     * Konstruktor - nastavení proměnných
     * @param   posX         X souřadnice, na kterým se cesta nachází
     * @param   posY         Y souřadnice, na kterým se cesta nachází
     * @param   y_size       šířka mapy
     */
    public shelf(int posX, int posY, int y_size){
        ID = y_size * posX + posY;
    }

    /**
     * Funkce vrátí jedinečné ID regálu
     * @return ID regálu
     */
    public int checkID(){
        return ID;
    }
}
