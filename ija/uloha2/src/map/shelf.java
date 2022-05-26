package map;

/**
 * Class regálu
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class shelf {
    final private int positionX;
    final private int positionY;
    final private int ID;

    /**
     * Konstruktor - nastavení proměnných
     * @param   posX         X souřadnice, na kterým se cesta nachází
     * @param   posY         Y souřadnice, na kterým se cesta nachází
     */
    public shelf(int posX, int posY){
        positionX = posX;
        positionY = posY;
        ID = 32 * posX + posY;
    }

    /**
     * Funkce vrátí jedinečné ID regálu
     * @return ID regálu
     */
    public int checkID(){
        return ID;
    }

    /**
     * Funkce vrátí X pozici regálu
     * @return X pozice na mapě
     */
    public int checkPosX(){
        return positionX;
    }

    /**
     * Funkce vrátí Y pozici regálu
     * @return Y pozice na mapě
     */
    public int checkPosY(){
        return positionY;
    }
}
