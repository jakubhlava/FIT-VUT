package request;

import java.util.HashMap;

/**
 * Třída pro požadavky na vyskladnění ze skladu
 * Obsahuje jeden požadavek, který obsahuje jeden nebo více typů zboží a počty kusů k vyskladnění
 *  * @author Jakub Hlava (xhlava52)
 *  * @author Lukáš Kraus (xkraus13)
 */
public class request {
    private HashMap<String, Integer> requests;

    /**
     * Konstruktor, tvořící prázdný požadavek - očekává se naplnění později
     */
    public request() {
        requests = new HashMap<>();
    }

    /**
     * Konstruktor, tvořící požadavek z již existující mapy typů a počtů ks k vyskladnění
     * @param requests  HashMap typ - počet ks k vyskladnění
     */
    public request(HashMap<String, Integer> requests) {
        this.requests = requests;
    }

    /**
     * Přidá část k požadavku na vyskladnění.
     * V případě, že typ předmětu ještě není v požadavku, pak je přidán se specifikovaným počtem ks.
     * Pokud již v požadavku je, pak je počet požadovaných kusů navýšen o specifikovaný počet ks.
     * @param type  identifikátor typu zboží
     * @param qty   počet ks k vyskladnění
     */
    public void addItem(String type, int qty) {
        if (requests.containsKey(type)) {
            requests.put(type, requests.get(type) + qty);
        } else {
            requests.put(type, qty);
        }
    }

    /**
     * Sníží množství požadovaných ks zboží v požadavku.
     * Pokud požadované množství klesne na 0, odstraní typ z požadavku.
     * @param type  identifikátor typu zboží
     * @param qty   počet ks, o které požadavek zmenšit
     */
    public void removeItem(String type, int qty) {
        if (requests.containsKey(type)) {
            int newQty = requests.get(type) - qty;
            requests.put(type, newQty);
            if (newQty <= 0) {
                requests.remove(type);
            }
        }
    }

    /**
     * Vrací mapu se všemi požadavky pro pozdější zpracování / pro serializaci
     * @return  HashMap typ zboží - počet ks k vyskladnění
     */
    public HashMap<String, Integer> getRequests() {
        return requests;
    }
}
