package cart;

import utils.io;
import map.cartDepot;
import map.map;
import map.path;
import map.shelf;
import map.dropOff;

import java.awt.*;
import java.util.ArrayList;
import java.util.Collections;

/**
 * Třída pro hledání cesty vozíku
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class findPath {

    /** Point startovní pozice odkud se bude hledat cesta */
    private final Point startPos = new Point();

    /** další pozice jednosměrné cesty */
    private final Point nextPos = new Point();

    /** další pozice obousměrné cesty */
    private final Point nextPos2 = new Point();

    /** Aktuální pozice */
    private final Point actualPos = new Point(-1,-1);
    private final Point loadPos = new Point();

    /** List ID požadovaných regálů */
    private final ArrayList<Integer> reqList;

    /** List x hodnot požádfovaných listů */
    private final ArrayList<Integer> xShelf;

    /** List pointů, kudy pojede vozík */
    private final ArrayList<Point> route;

    /** List pointů, kde vozík nakládá zboží */
    private final ArrayList<Point> loadStopPoints;


    private boolean repeat = true;
    private boolean repeat2 = true;
    private boolean imIn = false;
    private boolean changePos = false;
    private boolean jesteHledam = true;
    private boolean dalNicNeni;
    private boolean found;
    private boolean projelCelouCestu;

    private int pocetShelfuVUlicce;
    private int prujezdDepotem;

    private final map map;
    private final Object[] tiles;

    /**
     * Funkce vrátí List pointů (x,y souřadnice) kudy vozík pojede
     * @return  seznam bodů trasy
     */
    public ArrayList<Point> getRoute(){
        return route;
    }

    /**
     * Funkce vrátí List pointů (x,y souřadnice) kde vozík nabírá zboží
     * @return seznam zastávek na trase
     */
    public ArrayList<Point> getLoadPoints(){
        return loadStopPoints;
    }

    /**
     * kontruktor - inicializuje proměnné, zavolá funkci na nalezení startovní pozice a vyhledání cesty
     * @param map mapa
     * @param stops List ID požadovaných regálů
     */
    public findPath(map map, ArrayList<Integer> stops){
        this.reqList = stops;
        this.map = map;
        this.tiles = map.getMap();

        xShelf = new ArrayList<>();
        route = new ArrayList<>();
        loadStopPoints = new ArrayList<>();

        findStartPos();
        findRoute();
    }

    /**
     * Najde začáteční pozici, kde bude vyhledávání začínat (vedle cartDepotu)
     */
    private void findStartPos(){
        for (Object tile : tiles){
            if(tile instanceof cartDepot){
                int depotPosX = ((cartDepot)tile).checkPosX();
                int depotPosY = ((cartDepot)tile).checkPosY();
                if (startPath(depotPosX - 1, depotPosY)){
                    startPos.setLocation(depotPosX - 1,depotPosY);
                } else if (startPath(depotPosX + 1, depotPosY)){
                    startPos.setLocation(depotPosX + 1, depotPosY);
                } else if (startPath(depotPosX, depotPosY - 1)){
                    startPos.setLocation(depotPosX, depotPosY - 1);
                } else if (startPath(depotPosX, depotPosY + 1)){
                    startPos.setLocation(depotPosX, depotPosY + 1);
                } else {
                    System.exit(7);
                }
            }
        }
        nextPos.setLocation(startPos);
    }

    /**
     * Hlavní funkce pro hledání cesty k požadovaným regálům a zpět
     */
    private void findRoute(){
        for (int item : reqList) xShelf.add(item % map.getY_size());
        while(repeat){
            if (changePos){
                nextPos.setLocation(nextPos2);
                changePos = false;
            }
            loadPos.setLocation(nextPos);
            actualPos.setLocation(nextPos);
            checkTilesAround(nextPos.x, nextPos.y, ((path)tiles[io.getPosInArray(nextPos.x, nextPos.y, map.getY_size())]).checkDirection());
        }
    }

    /**
     * Kontrola tilů okolo cesty podle pravidla levé ruky (použe jednosměrné cesty)
     * @param x   x souřadnice tilu, na kterém se vozík nachází
     * @param y   y souřadnice tilu, na kterém se vozík nachází
     * @param forSwitch  směr jakým vozík jede
     */
    private void checkTilesAround(int x, int y, int forSwitch){
        Point savePos = new Point(x, y);
        route.add(savePos);
        System.out.println(nextPos);
        switch(forSwitch){
            case 0:         //směr cesty nahoru
                checkLEFT(x, y);
                checkRIGHT(x, y);
                checkUP(x, y);
                break;
            case 1:         //směr cesty vlevo
                checkDOWN(x, y);
                checkUP(x, y);
                checkLEFT(x, y);
                break;
            case 2:         //směr cesty vpravo
                checkUP(x, y);
                checkDOWN(x, y);
                checkRIGHT(x, y);
                break;
            case 3:         //směr cesty dolů
                checkRIGHT(x, y);
                checkLEFT(x, y);
                checkDOWN(x, y);
                break;
        }
    }

    /**
     * Kontrola tilů okolo cesty podle pravidla levé ruky (použe obousměrné cesty)
     */
    private void midPathCheck(){
        imIn = true;
        found = false;
        repeat2 = reqList.size() != 0;
        ArrayList<Point> maybeRoute = new ArrayList<>();
        projelCelouCestu = false;
        Point idk = new Point(nextPos);
        while (repeat2) {
            Point forSave = new Point(nextPos2);
            loadPos.setLocation(nextPos2);
            maybeRoute.add(forSave);
            System.out.println("midpath: "+nextPos2);
            if (nextPos.x < 2) {    // hledá se směrem dolů
                checkRIGHT(nextPos2.x, nextPos2.y);
                checkLEFT(nextPos2.x, nextPos2.y);
                checkDOWN(nextPos2.x, nextPos2.y);
            } else {                // hledá se směrem nahoru
                checkLEFT(nextPos2.x, nextPos2.y);
                checkRIGHT(nextPos2.x, nextPos2.y);
                checkUP(nextPos2.x, nextPos2.y);
            }
        }
        if(reqList.size() == 0){
            if(projelCelouCestu) nextPos2.x = (idk.x == 0) ? map.getX_size() - 1 : 0;
            else nextPos2.x = (idk.x == 0) ? 0 : map.getX_size() - 1;
            changePos = true;
            jesteHledam = false;
        } else if (dalNicNeni){
            nextPos2.x = map.getX_size() - 1;
            nextPos2.y++;
            changePos = true;
        }
        if (found){
            route.addAll(maybeRoute);
            if (!projelCelouCestu){
                Collections.reverse(maybeRoute);
                route.addAll(maybeRoute);
            }
        } else {
            if (projelCelouCestu) {
                route.addAll(maybeRoute);
            }
        }
        imIn = false;
    }

    /**
     * Kontrola tilu nad cestou
     * @param x   x souřadnice tilu, který se má prohledat
     * @param y   y souřadnice tilu, který se má prohledat
     */
    private void checkUP(int x,int y){
        try{
            checkTilesAroundHelp(tiles[io.getPosInArray(x - 1, y, map.getY_size())]);
        } catch (Exception e){
            System.out.print("");
        }
    }

    /**
     * Kontrola tilu nalevo od cesty
     * @param x   x souřadnice tilu, který se má prohledat
     * @param y   y souřadnice tilu, který se má prohledat
     */
    private void checkLEFT(int x,int y){
        try{
            checkTilesAroundHelp(tiles[io.getPosInArray(x, y - 1, map.getY_size())]);
        } catch (Exception e){
            System.out.print("");
        }
    }

    /**
     * Kontrola tilu napravo od cesty
     * @param x   x souřadnice tilu, který se má prohledat
     * @param y   y souřadnice tilu, který se má prohledat
     */
    private void checkRIGHT(int x,int y){
        try{
            checkTilesAroundHelp(tiles[io.getPosInArray(x, y + 1, map.getY_size())]);
        } catch (Exception e){
            System.out.print("");
        }
    }

    /**
     * Kontrola tilu pod cestou
     * @param x   x souřadnice tilu, který se má prohledat
     * @param y   y souřadnice tilu, který se má prohledat
     */
    private void checkDOWN(int x,int y){
        try{
            checkTilesAroundHelp(tiles[io.getPosInArray(x + 1, y, map.getY_size())]);
        } catch (Exception e){
            System.out.print("");
        }
    }

    /**
     * Kontrola typu tilu a zavolání dané funkce ke kontrole
     */
    private void checkTilesAroundHelp(Object tile){
        if (tile instanceof path)  pathTile((path)tile);
        else if (tile instanceof shelf)  shelfTile((shelf)tile);
        else if (tile instanceof dropOff)  dropOff();
        else if (tile instanceof cartDepot)  cartDepot();
    }

    /**
     * Kontrola tilu typu cesta
     * @param path tile cesty
     */
    private void pathTile(path path){
        if (!path.checkIsOpen()){
            if(path.checkDirection() < 0){
                repeat2 = false;
                return;
            } else {
                System.exit(5);
            }
        }
        int counter = 0, counter2 = 0;
        pocetShelfuVUlicce = 0;
        boolean jeTuShelf = false;
        for (int item : xShelf){
            if(path.checkPosY() + 1 == item || path.checkPosY() - 1 == item){
                jeTuShelf = true;
                pocetShelfuVUlicce++;
            }
            if(path.checkPosY() < item - 1) counter++;
            counter2++;
        }
        if (path.checkDirection() < 0 && jesteHledam){
            nextPos2.setLocation(path.checkPosX(), path.checkPosY());
            if(!(imIn)){
                if (jeTuShelf || (counter == counter2 && nextPos2.x < 5 && checkPathOpen(nextPos2.y))){
                    if(counter == counter2)  dalNicNeni = true;
                    midPathCheck();
                }
            }
        } else {
            projelCelouCestu = true;
            repeat2 = false;
            nextPos.setLocation(path.checkPosX(), path.checkPosY());
        }
    }

    /**
     * Kontrola shelfu - zda se jedná o shelf, ke kterému má vozík dojet
     * @param shelf tile regálu
     */
    private void shelfTile(shelf shelf){
        int index = 0;
        for (int a : reqList){
            if (shelf.checkID() == a){
                reqList.remove(index);
                xShelf.remove(index);
                pocetShelfuVUlicce--;
                found = true;
                loadStopPoints.add(new Point(loadPos));
                if (pocetShelfuVUlicce == 0 && reqList.size() != 0)  repeat2 = false;
            }
            index++;
        }
    }


    /**
     * Kontrola tilu typu dropOff
     * Pokud už není žádný požadavek, tak se vozík vyloží a zmizí
     */
    private void dropOff(){
        if(reqList.size() == 0) repeat = false;
    }

    /**
     * Kontrola tilu typu cartDepot
     * Kontrola, zda se hledání cesty nezacyklilo
     */
    private void cartDepot(){
        if(prujezdDepotem < 2) prujezdDepotem++;
        else System.exit(10);
    }

    private boolean checkPathOpen(int posY){
        for (Object tile : tiles){
            if (tile instanceof path){
                if(!((path)tile).checkIsOpen()){
                    if(((path)tile).checkPosY() == posY){
                        return false;
                    }
                }
            }
        }
        return true;
    }

    /**
     * Funkce kontroluje, zda je na dané pozici cesta (pro vyhledání startovací pozice)
     */
    private boolean startPath(int posX, int posY){
        if (posY >= map.getY_size() || posX >= map.getX_size() || posX < 0 || posY < 0) return false;
        return tiles[io.getPosInArray(posX, posY, map.getY_size())] instanceof path;
    }
}
