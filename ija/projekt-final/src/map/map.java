package map;

import utils.io;
import com.google.gson.Gson;

/**
 * Class pro načtení mapy z json souboru do pole
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class map {
    private int x_size = 0;
    private int y_size = 0;
    private int map_size;
    private Object[] map;

    /**
     * Vrací výšku mapy
     * @return výška mapy
     */
    public int getX_size(){
        return x_size;
    }

    /**
     * Vrací šířku mapy
     * @return šířka mapy
     */
    public int getY_size(){
        return y_size;
    }

    /**
     * Vrací pole s políčky mapy
     * @return  pole s mapou
     */
    public Object[] getMap() {
        return map;
    }

    /**
     * Funkce změní styl určení směru cesty ze stringu na int
     * @param   dir         string podoba směru cesty
     * @return  hodnota     směr cesty v int podobě
     */
    private int checkDir(String dir){
        if (dir == null){
            return -1;
        }
        switch (dir) {
            case "up":
                return 0;
            case "down":
                return 3;
            case "left":
                return 1;
            case "right":
                return 2;
        }
        return -2;
    }

    /**
     * Funkce vyprintuje mapu do konzole
     * @param   map      pole s položkami mapy
     */
    private void printMap(Object[] map, int size){
        int counter1 = 0;
        for(Object idk : map) {
            if (idk instanceof wall){
                System.out.print("  ");
            } else if (idk instanceof path){
                switch (((path)idk).checkDirection()) {
                    case 0:
                        System.out.print("↑ ");
                        break;
                    case 3:
                        System.out.print("↓ ");
                        break;
                    case 1:
                        System.out.print("← ");
                        break;
                    case 2:
                        System.out.print("→ ");
                        break;
                    default:
                        System.out.print("↕ ");
                        break;
                }
            } else if (idk instanceof shelf){
                System.out.print("# ");
                //System.out.print((32 * ((shelf)idk).checkPosX() + ((shelf)idk).checkPosY())+" ");
            } else if (idk instanceof dropOff){
                System.out.print("D ");
            } else if (idk instanceof cartDepot){
                System.out.print("C ");
            }
            counter1++;
            if (counter1 % size == 0) {
                System.out.print("\n");
            }
        }
    }

    /**
     * Funkce najde poslední položku mapy
     * @param   tile        pole položek
     * @return  last        poslední položka
     */
    private tmpMapTile lastTile(tmpMapTile[] tile){
        tmpMapTile last = null;
        for(tmpMapTile user : tile) {
            last = user;
        }
        return last;
    }

    /**
     * Funkce vytvoří nový objekt daného typu
     * @param   fill        položka načtená z json souboru
     * @return  objekt      nový objekt daného typu
     */
    private Object fillMap(tmpMapTile fill, int y_size){
        switch (fill.getType()){
            case "path":
                return new path(checkDir(fill.getDir()), fill.getPosX(), fill.getPosY());
            case "wall":
                return new wall(fill.getPosX(), fill.getPosY());
            case "shelf":
                return new shelf(fill.getPosX(), fill.getPosY(), y_size);
            case "cartDepot":
                return new cartDepot(fill.getPosX(), fill.getPosY());
            case "dropOff":
                return new dropOff(fill.getPosX(), fill.getPosY());
        }
        return null;
    }

    /**
     * Konstruktor - Načte mapu z json souboru a vytvoří ji v poli
     * @param path cesta k souboru
     * @throws Exception pokud soubor neexistuje
     */
    public map(String path) throws Exception {
        String json = io.readFile(path);
        Gson gson = new Gson();
        tmpMapTile[] mapTile = gson.fromJson(json, tmpMapTile[].class);

        tmpMapTile lastTile = lastTile(mapTile);
        x_size = lastTile.getPosX()+1;
        y_size = lastTile.getPosY()+1;
        int number = x_size * y_size;
        map = new Object[number];

        int counter = 0;
        for(tmpMapTile tile : mapTile) {
            map[counter] = fillMap(tile, y_size);
            counter++;
        }
        System.out.print("x: "+x_size+"\n");
        System.out.print("y: "+y_size+"\n");
        printMap(map, y_size);
    }
}

