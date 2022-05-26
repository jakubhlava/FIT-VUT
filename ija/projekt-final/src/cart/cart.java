package cart;

import map.map;

import java.awt.*;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.CopyOnWriteArrayList;

;import static cart.cartState.*;

/**
 * Třída pro generování a práci s vozíky
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class cart {
    private Point position;
    private cartPath path;
    private int pathStep = -1;
    private HashMap<String, Integer> content;
    private CopyOnWriteArrayList<task> tasks;
    private int size;
    private int ID;
    private cartState state;

    public cart(int ID, int size) {
        this.size = size;
        this.ID = ID;
        this.content = new HashMap<String, Integer>();
        this.tasks = new CopyOnWriteArrayList<>();
        this.position = new Point(0,0);
        this.state = CART_WAITING;
    }

    /**
     * Nastaví pozici vozíku
     * @param newpos    nová pozice
     */
    public void setPosition(Point newpos) {
        this.position = newpos;
    }

    /**
     * Nastaví pozici vozíku
     * @param x    nová pozice x
     * @param y    nová pozice y
     */
    public void setPosition(int x, int y) {
        position.x = x;
        position.y = y;
    }

    /**
     * Vrací pozici vozíku
     * @return  pozice vozíku
     */
    public Point getPosition() {
        return position;
    }

    /**
     * Vrací cestu vozíku
     * @return  cesta vozíku
     */
    public cartPath getPath() {
        return path;
    }

    /**
     * Vrací pozici v cestě
     * @return pozice v cestě
     */
    public int getPathStep() {
        return pathStep;
    }

    /**
     * Vrací úkoly vozíku
     * @return  úkoly vozíku
     */
    public CopyOnWriteArrayList<task> getTasks() {
        return tasks;
    }

    /**
     * Vrací, zda má vozík přiřazenou cestu
     * @return  true, pokud má vozík přiřazenou cestu
     *          false, jinak
     */
    public boolean hasPath() {
        return pathStep > -1;
    }

    /**
     * Přiřadá cestu/úkol vozíku
     * @param m objekt mapy
     */
    public void makePath(map m) {
        ArrayList<Integer> shelves = new ArrayList<>();
        for (task t : tasks) {
            shelves.add(t.getShelfId());
        }
        path = new cartPath(shelves, m);
        path.calculate();
        pathStep = 0;
        position = path.getRoute().get(0);
        state = CART_ON_ROUTE;
    }

    /**
     * Vrací stav vozíku
     * @return  stav vozíku
     */
    public cartState getState() {
        return state;
    }

    /**
     * Provede posun vozíku v čase o jeden krok
     */
    public void updateOnce() {
        switch (state) {
            case CART_ON_ROUTE:
                //System.out.println("State of cart "+getID()+" is CART_ON_ROUTE");
                pathStep++;
                if (pathStep < path.getRoute().size()) {
                    position = path.getRoute().get(pathStep);
                } else {
                    state = CART_FINISHED;
                }
                if (path.getStops().contains(position)) {
                    state = CART_PICKUP;
                    path.getStops().remove(position);
                }
                break;
            case CART_PICKUP:
                //System.out.println("State of cart "+ID+" is CART_PICKUP");
                state = CART_ON_ROUTE;
                break;
            case CART_FINISHED:
                //System.out.println("State of cart "+ID+" is CART_FINISHED");
                break;
            case CART_WAITING:
                //System.out.println("State of cart "+ID+" is CART_WAITING");
                break;
        }
        //System.out.println("Position of cart "+ID+" is: "+position.x+","+position.y+", on path it's point "+pathStep);
    }

    /**
     * Vrací ID vozíku
     * @return ID vozíku
     */
    public int getID() {
        return ID;
    }

    /**
     * Resetuje vozík do původního stavu
     */
    public void destroy() {
        this.content = new HashMap<String, Integer>();
        this.tasks = new CopyOnWriteArrayList<>();
        this.position = new Point(0,0);
        this.state = CART_WAITING;
        path = null;
        pathStep = -1;
        setPosition(0,0);
    }

    /**
     * Vloží zboží do košíku
     * @param type  identifikátor zboží
     * @param qty   množství zboží
     */
    public void addContent(String type, Integer qty) {
        if (content.containsKey(type)) {
            content.put(type,content.get(type)+qty);
        } else {
            content.put(type, qty);
        }
    }

    /**
     * Vrací obsah vozíku
     * @return  obsah vozíku
     */
    public HashMap<String, Integer> getContent() {
        return content;
    }

    /**
     * Přiřadí vozíku úkol
     * @param t úkol
     */
    public void addTask(task t) {
        tasks.add(t);
    }

    /**
     * Nastaví vozíku seznam úkolů ke splnění
     * @param tl    seznam úkolů
     */
    public void setTasks(CopyOnWriteArrayList<task> tl) {
        tasks = tl;
    }

}
