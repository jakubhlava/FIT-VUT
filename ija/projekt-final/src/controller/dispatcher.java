package controller;

import com.google.gson.Gson;
import storage.*;
import cart.*;
import request.*;
import map.*;
import utils.io;

import java.awt.*;
import java.util.*;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * Třída pro údržbu a správu skladu, požadavků a vozíků a zajištění jejich integrity
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class dispatcher {
    private static final String settingsPath = "data/settings.json";
    private storage s;
    private requestQueue rq;
    private ArrayList<cart> freeCarts;
    private CopyOnWriteArrayList<cart> cartsInUse;
    private LinkedHashSet<request> solvedRequests;
    private CopyOnWriteArrayList<CopyOnWriteArrayList<task>> taskList;
    private Integer cartID = 1;
    private settings cfg;
    private map m;

    public dispatcher() throws Exception {
        freeCarts = new ArrayList<cart>();
        cartsInUse = new CopyOnWriteArrayList<cart>();
        s = new storage();
        rq = new requestQueue();
        String settingsJson;
        Gson gson = new Gson();
        try {
            settingsJson = utils.io.readFile(settingsPath);
            cfg = gson.fromJson(settingsJson, settings.class);
        } catch (Exception e) {
            cfg = new settings();
            saveSettings();
        }
        setMap(cfg.getDefaultMapPath());
        solvedRequests = new LinkedHashSet<>();
        taskList = new CopyOnWriteArrayList<>();
    }

    /**
     * Uloží nastavení do souboru
     * @throws Exception    pokud nelze přisstoupit k souboru
     */
    public void saveSettings() throws Exception {
        Gson gson = new Gson();
        String settingsJson;
        settingsJson = gson.toJson(cfg);
        utils.io.writeFile(settingsPath, settingsJson);
    }

    /**
     * Vyresetuje dispatcher do původního stavu
     */
    public void reset() {
        cartID = 1;
        s.clearReservations();
        solvedRequests.clear();
        freeCarts.clear();
        cartsInUse.clear();
        taskList.clear();
    }

    /**
     * Získá množství zboží na skladě s ohledem na množství zboží ve frontě požadavků
     * @param type          identifikátor zboží
     * @return              počet zboží, které lze vyskladnit
     * @throws Exception    pokud typ zboží neexistuje
     */
    public int getGlobalQty(String type) throws Exception {
        int baseQty = s.getGlobalQty(type);
        for (request r : rq.getRequests()) {
            if (r.getItems().containsKey(type)) {
                baseQty -= r.getItems().get(type);
            }
        }
        return baseQty;
    }

    /**
     * Nastaví sklad k použití
     * @param s     inicializovaný sklad
     */
    public void setStorage(storage s) {
        this.s = s;
        this.rq = new requestQueue();
    }

    /**
     * Nastaví frontu požadavků k používání
     * @param rq    inicializovaná fronta požadavků
     * @throws Exception    pokud je nedostatek zboží ve skladu
     */
    public void setQueue(requestQueue rq) throws Exception {
        requestQueue oldrq = this.rq;
        this.rq = rq;
        for (request r : rq.getRequests()) {
            for (String type : r.getItems().keySet()) {
                if (!s.isKnown(type)) {
                    throw new Exception("Neznámý typ");
                }
            }
        }
        for (String type : s.getKnown()) {
            if (getGlobalQty(type) < 0) {
                this.rq = oldrq;
                throw new Exception("Nedostatek zboží ve skladu");
            }
        }
    }

    /**
     * Vrátí aktuálněp oužívaný sklad
     * @return  sklad
     */
    public storage getStorage() {
        return s;
    }

    /**
     * Vrátí aktuálně používanou frontu požadavků
     * @return  fronta požadavků
     */
    public requestQueue getQueue() {
        return rq;
    }

    /**
     * Vrátí objekt mapy
     * @return  mapa
     */
    public map getMap() {
        return m;
    }

    /**
     * Změní mapu
     * @param path  cesta k souboru s mapou
     */
    public void setMap(String path) {
        map oldMap = m;
        try {
            m = new map(path);
        } catch (Exception e) {
            m = oldMap;
        }
    }

    /**
     * Získá a zpracuje obsah regálu pro použití v UI
     * @param shelfId   identifikátor regálu
     * @return  seznam předmětů v regálu
     */
    public ArrayList<String> makeShelfContentList(Integer shelfId) {
        HashMap<String, Integer> content;
        ArrayList<String> contentTable = new ArrayList<>();
        try {
            content = s.getShelfContents(shelfId);
        } catch (Exception e) {
            return contentTable;
        }
        for (String type : content.keySet()) {
            try {
                contentTable.add(s.getDisplayName(type) + ": " + content.get(type) + " ks");
            } catch (Exception e) {}
        }
        return contentTable;
    }

    /**
     * Získá a zpracuje obsah požadavku pro použítí v UI
     * @param r     požadavek
     * @return  seznam předmětů v púožadavku
     */
    public ArrayList<String> makeRequestContentList(request r) {
        ArrayList<String> contentTable = new ArrayList<>();
        HashMap<String, Integer> requests = r.getItems();
        for (String type : requests.keySet()) {
            try {
                contentTable.add(s.getDisplayName(type) + ": " + requests.get(type) + " ks");
            } catch (Exception e) {}
        }
        return contentTable;
    }

    /**
     * Získá a zpracuje obsah regálu pro použití v UI
     * @param   c   vozík
     * @return  seznam předmětů ve vozíku
     */
    public ArrayList<String> makeCartContentList(cart c) {
        HashMap<String, Integer> content;
        ArrayList<String> contentTable = new ArrayList<>();
        for (String type : c.getContent().keySet()) {
            try {
                contentTable.add(s.getDisplayName(type) + ": " + c.getContent().get(type) + " ks");
            } catch (Exception e) {}
        }
        return contentTable;
    }

    /**
     * Vrací objekt s aktuálním nastavením
     * @return  aktuální nastavení
     */
    public settings getSettings() {
        return cfg;
    }

    /**
     * Ze všech požadavků z aktuální fronty vytvoří úkoly pro vozíky dané velikosti
     * @throws Exception    Pokud nejsou ve frontě požadavků validní požadavky
     * @return true, pokud se podařilo vytvořit úkoly
     *         false, pokud ne
     */
    public boolean createTasks() throws Exception {
        LinkedHashSet<request> todo = (LinkedHashSet<request>) rq.getRequests().clone();
        todo.removeAll(solvedRequests);

        LinkedHashSet<Integer> blocked = getBlockedStorage();
        if (!areRequestsPossible(todo, blocked)) {
            return false;
        }

        for (request r: todo) {
            try {
                CopyOnWriteArrayList<CopyOnWriteArrayList<task>> items = requestToTasks(r, blocked);
                taskList.addAll(items);
                solvedRequests.add(r);
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }
        }
        System.out.println("Jobs done! Now pending: "+taskList.size()+" task(s)");
        return true;
    }

    /**
     * Pro požadavek (typicky mazaný) zničí existující úkoly a vrátí rezervované zboží
     * @param r požadavek, pro který budou zničeny úkoly
     * @throws Exception    Pokud nejsou ve frontě požadavků validní požadavky
     */
    public void destroyTasksFor(request r) throws Exception {

        for (CopyOnWriteArrayList<task> tl: taskList) {
            for (task t : tl) {
                if (t.getParent() == r) {
                    tl.remove(t);
                    for (String type : t.getGoods().keySet()) {
                        s.unreserveFrom(t.getShelfId(), type, t.getGoods().get(type));
                    }
                }

            }
            if (tl.size() == 0) {
                taskList.remove(tl);
            }
        }
    }

    /**
     * Převede požadavek na seznam seznamů úkolů pro vozíky, kde každému vozíku náleží právě jeden seznam úkolů tak,
     * aby byl vozík vždy efektivně využit
     * @param r         požadavek k převedení
     * @param blocked   seznam nedostupných regálů
     * @return          seznam seznamů úkolů vzniklý z požadavku.
     * @throws Exception    Pokud nejsou ve frontě požadavků validní požadavky
     */
    public CopyOnWriteArrayList<CopyOnWriteArrayList<task>> requestToTasks(request r, LinkedHashSet<Integer> blocked) throws Exception {
        CopyOnWriteArrayList<CopyOnWriteArrayList<task>> tasks = new CopyOnWriteArrayList<>();
        HashMap<String, Integer> items = (HashMap) r.getItems().clone();
        while (!items.isEmpty()) {                              // tasky na vozik
            CopyOnWriteArrayList<task> cartTasks = new CopyOnWriteArrayList<task>();
            int capacity = cfg.getCartSize();
            while (true) {                                      // itemy na task
                String type = items.keySet().iterator().next();
                HashMap<Integer, Integer> shelves = s.findItem(type);
                Iterator<Integer> iter = shelves.keySet().iterator();
                int shelf = iter.next();
                while (blocked.contains(shelf)) {
                    try {
                        System.out.println("[DISPATCHER] Shelf "+ shelf +" blocked, trying new one");
                        shelf = iter.next();
                        System.out.println("[DISPATCHER] Picked "+ shelf);
                    } catch (Exception e) {
                        System.out.println("[DISPATCHER] Can not access any shelf with "+type);
                        return tasks;
                    }
                }
                task t = new task(shelf, r);
                int reserve = capacity;
                if (items.get(type) < reserve) {
                    reserve = items.get(type);
                }
                if (s.getQty(shelf,type) < reserve) {
                    reserve = s.getQty(shelf,type);
                }
                t.addGoods(type, reserve);
                s.reserveFrom(t.getShelfId(), type, reserve);
                items.put(type, items.get(type)-reserve);
                capacity -= reserve;
                System.out.println("Created " + type + " task for shelf "+t.getShelfId()+" ("+items.get(type)+" remaining)");
                cartTasks.add(t);
                if (items.get(type) == 0) {
                    items.remove(type);
                }
                if (items.isEmpty()) {
                    break;
                }
                if (capacity == 0) {
                    break;
                }

            }
            tasks.add(cartTasks);
            System.out.println("This cart will have "+cartTasks.size()+" task(s)!");
        }
        return tasks;
    }

    /**
     * Provede jeden krok simulace
     */
    public void tick(){
        if (taskList.size() > 0) {
            if (freeCarts.size() > 0) {
                cart c = freeCarts.get(0);
                freeCarts.remove(c);
                CopyOnWriteArrayList<task> tl = taskList.get(0);
                c.setTasks(tl);
                System.out.println("[CART "+c.getID()+"] Assigned "+tl.size()+" tasks");
                for (task t: tl) {
                    System.out.println("[CART "+c.getID()+"] Will get goods from "+t.getShelfId());
                }
                taskList.remove(tl);
                c.makePath(m);
                cartsInUse.add(c);
            }
        }
        for (cart c: cartsInUse) {
            if (c.getState() == cartState.CART_ON_ROUTE) {
                c.updateOnce();
            } else if (c.getState() == cartState.CART_PICKUP) {
                Point cpos = c.getPosition();
                int[] possible = {
                        (cpos.x+1) * m.getY_size() + cpos.y,
                        (cpos.x-1) * m.getY_size() + cpos.y,
                        cpos.x * m.getY_size() + cpos.y+1,
                        cpos.x * m.getY_size() + cpos.y-1
                };
                for (int id : possible) { // sekonvání okolních regálů, zda můžu dokončit úkol některými z nich (a kterými)
                    System.out.println("[CART "+c.getID()+"] Scanning shelf "+id);
                    for (task t: c.getTasks()) {
                        System.out.println("[CART "+c.getID()+"] Got tasks for "+t.getShelfId());
                        if (id == t.getShelfId()) {
                            System.out.println("[CART "+c.getID()+"] Matched task for "+t.getShelfId() + " and neighbor shelf "+id);
                            try {
                                doPickup(c, t);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }
                }
                c.updateOnce();
            } else if (c.getState() == cartState.CART_FINISHED) {
                for (task t: c.getTasks()) {
                    boolean removal = true;
                    for (cart others: cartsInUse) { // proskenování ostatních vozíků, jestli některý ještě nedokončuje požadavek
                        if (others != c) {
                            for (task to : others.getTasks()) {
                                if (to.getParent() == t.getParent()) {
                                    removal = false;
                                }
                            }
                        }
                    }
                    if (t.getParent().getItems().isEmpty() && removal) {
                        rq.removeRequest(t.getParent());
                    }
                }
                freeCarts.add(c);
                cartsInUse.remove(c);
                c.destroy();
            }
        }
    }

    /**
     * Pomocná funkce pro dotvoření vozíků po začátku simulace
     */
    public void startSim() {
        while ((getCartsInUse().size()+getFreeCarts().size()) < cfg.getCartLimit()) {
            spawnCart();
        }
    }

    /**
     * Provede naložení vozíku podle přiděleného úkolu
     * @param c     vozík pro naložení
     * @param t     úkol vozíku
     * @throws Exception    Pokud zboží neexistuje nebo nastane nekonzistence mezi požadavky a skladem
     */
    public void doPickup(cart c, task t) throws Exception {
        for (String type : t.getGoods().keySet()) {
            System.out.println("[CART "+c.getID()+"] Picking "+t.getGoods().get(type)+" of "+type+" from shelf "+t.getShelfId());
            s.pickFrom(t.getShelfId(),type,t.getGoods().get(type));
            c.addContent(type, t.getGoods().get(type));
            t.removeGoods(type, t.getGoods().get(type));
        }
    }

    /**
     * Vytvoří nový vozík a přidá ho do seznamu vozíků
     */
    public void spawnCart() {
        cart c = new cart(cartID, cfg.getCartSize());
        cartID++;
        freeCarts.add(c);
    }

    /**
     * Odstraní existující vozík
     * @param c     vozík k odstranění
     */
    public void deleteCart(cart c) {
        try {
            freeCarts.remove(c);
        } catch (Exception e) {}
    }

    /**
     * Vybere poslední úkol ze seznamu úkolů pro vozíky
     * @return  seznam úkolů
     */
    public CopyOnWriteArrayList<task> popTaskList() {
        CopyOnWriteArrayList<task> r;
        if (taskList.size() > 0) {
            r = taskList.get(0);
        } else {
            return null;
        }
        taskList.remove(r);
        return r;
    }

    /**
     * Vrací počet nepřiřazených úkolů
     * @return  počet nepřiřazených úkolů
     */
    public int taskCount() {
        return taskList.size();
    }

    /**
     * Posun vpřed časem v simulaci
     * @param ticks počet kroků pro posun
     */
    public void fastForward(int ticks) {
        for (int i = 0; i < ticks; i++) {
            tick();
        }
    }

    /**
     * Vrátí seznam vozíků, které jsou na cestě
     * @return  seznam vozíků, které jsou na cestě
     */
    public CopyOnWriteArrayList<cart> getCartsInUse() {
        return cartsInUse;
    }

    /**
     * Vrátí seznam volných vozíků k použití
     * @return  seznam volných vozíků
     */
    public ArrayList<cart> getFreeCarts() {
        return freeCarts;
    }

    /**
     * Vrátí seznam požadavků
     * @return  seznam požadavků
     */
    public CopyOnWriteArrayList<CopyOnWriteArrayList<task>> getTaskList() {
        return taskList;
    }

    /**
     * Zjistí z mapy zablokované cesty
     * @return  souřadnice zablokovaných cest
     */
    private LinkedHashSet<Point> getBlockedPaths() {
        LinkedHashSet<Point> blocked = new LinkedHashSet<>();
        for (Object tile : m.getMap()) {
            if (tile instanceof path) {
                if (!((path) tile).checkIsOpen()) {
                    blocked.add(new Point(((path) tile).checkPosX(), ((path) tile).checkPosY()));
                }
            }
        }
        return blocked;
    }

    /**
     * Vytvoří seznam nedostupných regálů
     * @return  seznam nedostupných regálů
     */
    private LinkedHashSet<Integer> getBlockedStorage() {
        LinkedHashSet<Point> blocked = getBlockedPaths();
        LinkedHashSet<Integer> bshelves = new LinkedHashSet<>();

        LinkedHashSet<Point> allBlocked = new LinkedHashSet<>();

        int maxy = -1;

        for (Point p: blocked) {
            // detekce blokace jednosměrné hlavní cesty == odříznutí části skladu
            if (((path) m.getMap()[io.getPosInArray(p.x, p.y, m.getY_size())]).checkDirection() >= 0) {
                if (p.y > maxy) {
                    maxy = p.y;
                }
            }
            // spojení vícenásobných blokací v jednom sloupci
            for (Point p2: blocked) {
                if (p != p2) {
                    if (p.y == p2.y) {
                        int start,stop;
                        if (p.x < p2.x) {
                            start = p.x;
                            stop = p2.x;
                        } else {
                            start = p2.x;
                            stop = p.x;
                        }
                        for (int i = start; i <= stop; i++) {
                            allBlocked.add(new Point(i,p.y));
                        }
                    }
                }
            }
            allBlocked.add(p);
        }

        if (maxy != -1) {
            for (Object tile: m.getMap()) {
                if (tile instanceof path && ((path) tile).checkPosY() <= maxy) {
                    allBlocked.add(new Point(((path) tile).checkPosX(),((path) tile).checkPosY()));
                }
            }
        }

        for (Point p: allBlocked) { // detekce zablokovaných regálů těsně kolem zablokovaných cest
            int[] possible = { // čtyřokolí regálu
                    //(p.x+1) * m.getY_size() + p.y,
                    //(p.x-1) * m.getY_size() + p.y,
                    p.x * m.getY_size() + p.y+1,
                    p.x * m.getY_size() + p.y-1
            };
            for (int candidate: possible) { // do seznamu přidávám jen platné regály
                if (candidate < m.getMap().length && candidate >= 0) {
                    if (m.getMap()[candidate] instanceof shelf) {
                        bshelves.add(candidate);
                    }
                }
            }
        }

        return bshelves;
    }

    /**
     * Zjišťuje, zda je možné splnit aktuální požadavky
     * @return  true,   pokud ano
     *          false,  pokud ne
     */
    private boolean areRequestsPossible(LinkedHashSet<request> requests, LinkedHashSet<Integer> blocked) throws Exception {
        HashMap<String, Integer> requested = new HashMap<>();
        for (request r : requests) {
            for (String type: r.getItems().keySet()) {
                if (requested.containsKey(type)) {
                    requested.put(type,requested.get(type)+r.getItems().get(type));
                } else {
                    requested.put(type,r.getItems().get(type));
                }
            }
        }
        for (String type : requested.keySet()) {
            if (requested.get(type) > s.getGlobalQty(type, blocked)) {
                System.out.println("[DISPATCHER] Not enough "+type+", unblock some paths to continue!");
                return false;
            }
        }
        return true;
    }
}
