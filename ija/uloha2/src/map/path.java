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
    private boolean isCart;

    /**
     * Konstruktor - nastavení proměnných
     * @param   dir         směr jakým se dá po cestě jet
     * @param   posX         X souřadnice, na kterým se cesta nachází
     * @param   posY         Y souřadnice, na kterým se cesta nachází
     */
    public path(int dir, int posX, int posY){
        direction = dir;
        isOpen = false;
        isCart = false;
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
     * Funkce nastaví ceště, že se na ní nachází vozík
     */
    public void plusCart(){
        isCart = true;
    }

    /**
     * Funkce nastaví ceště, že se na ní nenachází vozík
     */
    public void minusCart(){
        isCart = false;
    }

    /**
     * Funkce vrací, jestli se na cestě nachází vozík
     * @return Zda na cestě stojí vozík
     */
    public boolean checkCart(){
        return isCart;
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
