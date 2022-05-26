package map;

/**
 * Class zdi
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class wall {
    final private int positionX;
    final private int positionY;

    /**
     * Konstruktor - nastavení proměnných
     * @param   posX         X souřadnice, na kterým se cesta nachází
     * @param   posY         Y souřadnice, na kterým se cesta nachází
     */
    public wall(int posX, int posY){
        positionX = posX;
        positionY = posY;
    }

    /**
     * Funkce vrátí X pozici zdi
     * @return X pozice na mapě
     */
    public int checkPosX(){
        return positionX;
    }

    /**
     * Funkce vrátí Y pozici zdi
     * @return Y pozice na mapě
     */
    public int checkPosY(){
        return positionY;
    }
}

