package map;

/**
 * Class vyprazdňování vozíků
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class dropOff {
    final private int positionX;
    final private int positionY;

    /**
     * Konstruktor - nastavení proměnných
     * @param   posX         X souřadnice, na kterým se cesta nachází
     * @param   posY         Y souřadnice, na kterým se cesta nachází
     */
    public dropOff(int posX, int posY){
        positionX = posX;
        positionY = posY;
    }

    /**
     * Funkce vrátí X pozici vykládacího místa
     * @return X pozice na mapě
     */
    public int checkPosX(){
        return positionX;
    }

    /**
     * Funkce vrátí Y pozici vykládacího místa
     * @return Y pozice na mapě
     */
    public int checkPosY(){
        return positionY;
    }
}
