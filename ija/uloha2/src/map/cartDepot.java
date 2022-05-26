package map;

/**
 * Class skladu vozíků
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class cartDepot {
    private int cart;
    final private int positionX;
    final private int positionY;

    /**
     * Konstruktor - nastavení proměnných
     * @param   posX         X souřadnice, na kterým se cesta nachází
     * @param   posY         Y souřadnice, na kterým se cesta nachází
     */
    public cartDepot(int posX, int posY){
        cart = 10;
        positionX = posX;
        positionY = posY;
    }

    /**
     * Funkce přidá vozík do depa
     */
    public void addCart(){
        cart++;
    }

    /**
     * Funkce odebere vozík z depa
     */
    public void subCart(){
            cart--;
    }

    /**
     * Funkce vrátí X pozici depa vozíků
     * @return X pozice na mapě
     */
    public int checkPosX(){
       return positionX;
    }

    /**
     * Funkce vrátí Y pozici depa vozíků
     * @return Y pozice na mapě
     */
    public int checkPosY(){
        return positionY;
    }
}
