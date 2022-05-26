package map;

/**
 * Pomocná class pro načítání mapy
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class tmpMapTile {
    private String type;
    private String dir;
    private int posX;
    private int posY;

    /**
     * Zjistí typ políčka
     * @return typ políčka
     */
    public String getType(){
        return type;
    }

    /**
     * Funkce směr cesty políčka (pouze pro cestu)
     * @return směr cesty
     */
    public String getDir(){
        return dir;
    }

    /**
     * Funkce vrátí X pozici políčka
     * @return X pozice na mapě
     */
    public int getPosX(){
        return posX;
    }

    /**
     * Funkce vrátí Y pozici políčka
     * @return Y pozice na mapě
     */
    public int getPosY(){
        return posY;
    }
}
