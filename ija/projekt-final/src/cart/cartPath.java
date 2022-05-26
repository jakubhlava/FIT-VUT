package cart;

import map.map;

import java.awt.*;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * Třída pro hledání a správu cest vozíků
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class cartPath {
    private ArrayList<Point> route;
    private ArrayList<Point> stops;
    private ArrayList<Integer> shelves;
    private map m;

    /**
     * Inicializace cesty s počátkem a koncem
     * @param shelves   seznam regálů
     * @param m         mapa
     */
    public cartPath(ArrayList<Integer> shelves, map m) {
        this.m = m;
        this.shelves = shelves;
        this.stops = new ArrayList<>();
    }


    /**
     * Výpočet nebo přepočet trasy bez blokací bodů
     * @return  true pokud se povedla nalézt cesta
     *          false pokud je bod nedostupný
     */
    public boolean calculate() {
        findPath fp = new findPath(m, shelves);
        this.route = fp.getRoute();
        this.stops = fp.getLoadPoints();
        return true;
    }

    /**
     * Vrací trasu cesty
     * @return  trasa cesty
     */
    public ArrayList<Point> getRoute() {
        return route;
    }

    /**
     * Přidá zastávku
     * @param p     Bod, kde zastavit
     */
    public void setStop(Point p) {
        stops.add(p);
    }

    /**
     * Vrací zastávky
     * @return  zastávky
     */
    public ArrayList<Point> getStops() {
        return stops;
    }
}
