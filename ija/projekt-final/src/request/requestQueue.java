package request;

import java.lang.reflect.Type;
import java.util.*;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import utils.io;

/**
 * Fronta požadavků, která spravuje seznam požadavků a umožňuje jeho konstrukci, jak manuálně, tak ze souboru.
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class requestQueue {
    private LinkedHashSet<request> requests;
    private HashMap<request, Integer> persistentIDs;
    private int maxID = 1;

    /**
     * Konstruktor, inicializující prázdnou frontu požadavků.
     */
    public requestQueue() {
        requests = new LinkedHashSet<>();
        persistentIDs = new HashMap<request, Integer>();
    }

    /**
     * Konstruktor, inicializující frontu požadavků ze souboru.
     * @param filename      cesta k souboru, obsahující frontu požadavků
     * @throws Exception    v případě neexistujícího souboru
     */
    public requestQueue(String filename) throws Exception {
        requests = new LinkedHashSet<>();
        String content = io.readFile(filename);
        Gson gson = new Gson();
        Type reqType = new TypeToken<LinkedHashSet<request>>(){}.getType();
        requests = gson.fromJson(content, reqType);
        persistentIDs = new HashMap<request, Integer>();
        for (request r : requests) {
            persistentIDs.put(r,maxID);
            maxID++;
        }
    }

    /**
     * Pomocná funkce pro reset, kdy dokchází k serializaci mimo soubor (tedy nelze využít kontruktor)
     * @param serialized    řetězec s GSON serializací seznamu požadavků
     */
    public void deserializeQueue(String serialized) {
        requests = new LinkedHashSet<>();
        Gson gson = new Gson();
        Type reqType = new TypeToken<LinkedHashSet<request>>(){}.getType();
        requests = gson.fromJson(serialized, reqType);
        persistentIDs = new HashMap<request, Integer>();
        for (request r : requests) {
            persistentIDs.put(r,maxID);
            maxID++;
        }
    }

    /**
     * Přidá požadavek na konec fronty
     * @param r     požadavek na vyskladnění
     */
    public void addRequest(request r) {
        requests.add(r);
        persistentIDs.put(r,maxID);
        maxID++;
    }

    /**
     * Odstraní požadavek z fronty
     * @param r     pozadavek k odstranění
     */
    public void removeRequest(request r) {
        requests.remove(r);
    }

    /**
     * Vrací stálé ID pro požadavek
     * @param r     požadavek
     * @return      stálé ID
     * @throws Exception    Pokud požadavek nikdy nebyl v této frontě
     */
    public int getPersistentID(request r) throws Exception {
        if (!persistentIDs.containsKey(r)) {
            throw new Exception("Tento požadavek nepatří do této fronty!");
        } else {
            return persistentIDs.get(r);
        }
    }

    /**
     * Odstraní a vrátí nejstarší požadavek z fronty
     * @return r    požadavek z konce fronty
     */
    public request popRequest() {
        request r = requests.iterator().next();
        requests.remove(r);
        return r;
    }

    /**
     * Vrátí celou frontu požadavků
     * @return  fronta požadavků
     */
    public LinkedHashSet<request> getRequests() {
        return requests;
    }

}
