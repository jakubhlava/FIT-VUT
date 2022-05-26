package map;

/**
 * Class cesty
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class path {
    final private int direction; //určení směru - UP=0, LEFT=1, RIGHT=2, DOWN=3
    final private int positionX;
    final private int positionY;
    private boolean isOpen;

    /**
     * Konstruktor - nastavení proměnných
     * @param   dir         směr jakým se dá po cestě jet
     * @param   posX         X souřadnice, na kterým se cesta nachází
     * @param   posY         Y souřadnice, na kterým se cesta nachází
     */
    public path(int dir, int posX, int posY){
        direction = dir;
        isOpen = true;
        positionX = posX;
        positionY = posY;
    }

    /**
     * Funkce vrátí směr cesty
     * @return  směr cesty
     */
    public int checkDirection(){
        return direction;
    }

    /**
     * Funkce otevře cestu (bude průjezdná)
     */
    public void openPath(){
        isOpen = true;
    }

    /**
     * Funkce uzavře cestu (bude neprůjezdná)
     */
    public void closePath(){
        isOpen = false;
    }

    /**
     * Funkce zjistí, zda je cesta průjezdná
     * @return průjezdnost cesty
     */
    public boolean checkIsOpen(){
        return isOpen;
    }

    /**
     * Funkce vrátí X pozici cesty
     * @return X pozice na mapě
     */
    public int checkPosX(){
        return positionX;
    }

    /**
     * Funkce vrátí Y pozici cesty
     * @return Y pozice na mapě
     */
    public int checkPosY(){
        return positionY;
    }
}
