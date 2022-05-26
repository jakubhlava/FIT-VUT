package storage;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Set;

/**
 * Třída reprezentující celý sklad, tvoří obal pro systém regálů a zboží a uchovává si inforamce o známých typech zboží.
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class storage {
    private HashMap<String, String> knownTypes;
    private HashMap<Integer, shelf> shelves;

    /**
     * Konstruktor skladu, vytvoří prázdné hasmapy pro známé typy, jejich zobrazované názby a pro regály
     */
    public storage() {
        shelves = new HashMap<>();
        knownTypes = new HashMap<>();
    }

    /**
     * Přidá typ zboží do knihovny známých typů
     * @param type      identifikátor typu zboží
     * @param display   zobrazovaný název zboží
     */
    public void addType(String type, String display) {
        if (!knownTypes.containsKey(type)) {
            knownTypes.put(type, display);
        }
    }

    /**
     * Získá množinu známých typů zboží
     * @return  množina známých typů zboží
     */
    public Set<String> getKnown() {
        return knownTypes.keySet();
    }

    /**
     * Ověří, zda se typ nachází v knihovně známých typů
     * @param type  identifikátor typu zboží
     * @return  true pokud je typ známý
     *          false jinak
     */
    public boolean isKnown(String type) {
        return knownTypes.containsKey(type);
    }

    /**
     * Vrací zobrazovaný název pro určitý typ zboží
     * @param id            identifikátor typu zboží
     * @return              zobrazované jméno pro zboží
     * @throws Exception    pokud není typ známý
     */
    public String getDisplayName(String id) throws Exception {
        if (!knownTypes.containsKey(id)) {
            throw new Exception("Neznámý druh zboží!");
        } else {
            return knownTypes.get(id);
        }
    }

    /**
     * Pokud neexistuje regál s id shelfId, pak jej vytvoří a přídá do skladu
     * @param shelfId   identifikátor regálu
     */
    public void createShelf(Integer shelfId) {
        if (!hasShelf(shelfId)) {
            shelf s = new shelf(shelfId);
            shelves.put(shelfId,s);
        }
    }

    /**
     * Pokusí se najít zboží v regálu a zjistit jeho množství
     * @param shelfId   identifikátor regálu
     * @param type      identifikátor zboží
     * @return  počet zboží, pokud regál i zboží existují
     *          -1 pokud zboží neexistuje
     *          -2 pokud neexistuje regál
     * @throws Exception pokud neexistuje regál nebo typ zboží
     */
    public int getQty(Integer shelfId, String type) throws Exception {
        if (!isKnown(type)) {
            throw new Exception("Neexistující typ!");
        }
        if (hasShelf(shelfId)) {
            shelf s = shelves.get(shelfId);
            if (s.hasItem(type)) {
                  return s.getQty(type);
            } else {
                return 0;
            }
        } else {
            throw new Exception("Neexistující regál!");
        }
    }

    /**
     * Vrací počet kusů zboží v celém skladě
     * @param type  identifikátor zboží
     * @return  počet kusů zboží, pokud jde o známé zboží
     *          -1 pokud je typ zboží neznámý
     * @throws Exception pokud neexistuje typ zboží
     */
    public int getGlobalQty(String type) throws Exception {
        if (!isKnown(type)) {
            throw new Exception("Neznámý typ zboží");
        }
        int qty = 0;
        for (shelf s : shelves.values()) {
            int sQty = s.getQty(type);
            if (sQty > 0) qty += sQty;
        }
        return qty;
    }

    /**
     * Vrací obsah regálu
     * @param shelfId   identifikátor regálu
     * @return  obsah regálu
     * @throws Exception    pokud neexistuje typ regálu
     */
    public HashMap<String, Integer> getShelfContents(Integer shelfId) throws Exception {
        if (!hasShelf(shelfId)) {
            throw new Exception("Neexistující regál");
        }
        return shelves.get(shelfId).getStock();
    }

    /**
     * "Vyskladní" určitý počet zboží z regálu
     * @param shelfId   identifikátor regálu
     * @param type      identifikátor zboží
     * @param qty       počet kusů k vyskladnění
     * @throws Exception    při neexistujícím typu zboží, regálu nebo nedostatku zboží v regálu
     */
    public void pickFrom(Integer shelfId, String type, Integer qty) throws Exception {
        if (hasShelf(shelfId)) {
            shelves.get(shelfId).pick(type, qty);
        } else {
            throw new Exception("Neznámý identifikátor regálu");
        }
    }

    /**
     * Přidá předmět do regálu, případně zvýší jeho skladové zásoby, pokud již existuje
     * @param shelfId   identifikátor regálu
     * @param type      identifikátor zboží
     * @param qty       počet kusů
     * @throws Exception    pokud neexistuje typ zboží nebo regál
     */
    public void addItemTo(Integer shelfId, String type, Integer qty) throws Exception {
        if (!isKnown(type)) {
            throw new Exception("Neznámý typ zboží");
        }
        if (!hasShelf(shelfId)) {
            throw new Exception("Neznámý identifikátor regálu");
        }
        shelves.get(shelfId).addItem(type, qty);
    }

    /**
     * Ověří, zda regál s daným identifikátorem ve sklad uexistuje
     * @param shelfId   identifikátor regálu
     * @return  true    pokud regál existuje
     *          false   jinak
     */
    public boolean hasShelf(Integer shelfId) {
        return shelves.containsKey(shelfId);
    }

    /**
     * Vyhledá a vrátí objekt regálu podle jeho identifikátoru
     * @param shelfId       identifikátor regálu
     * @return              objekt regálu s identifikátorem shelfId
     * @throws Exception    pokud regál s tímto id neexistuje
     */
    public shelf getShelfById(Integer shelfId) throws Exception {
        if (!hasShelf(shelfId)) {
            throw new Exception("Regál s tímto ID se v tomto skladu nenachází!");
        } else {
            return shelves.get(shelfId);
        }
    }

    /**
     * Vrací seznam regálů, ve kterých se nachází určitý druh zboží
     * @param type  identifikátor typu zboží
     * @return  seznam regálů, obsahující typ zboží
     */
    public HashMap<Integer, Integer> findItem(String type) {
        HashMap<Integer, Integer> output = new HashMap<>();
        for (shelf s : shelves.values()) {
            if (s.hasItem(type)) {
                output.put(s.getShelfId(), s.getQty(type));
            }
        }
        return output;
    }

    /**
     * Provádí kontrolu integrity ID položek skladu po inicializaci skladu ze souboru
     * Sklad je načítán pomocí GSON knihovny obcházenící kontrolní mechanismy přidávání položek do skladu.
     * @return  true pokud všechna ID typů zboží jsou pro skald známá
     *          false pokud je výše uvedené pravidlo porušeno
     */
    public boolean integrityCheck() {
        boolean integrity = true;
        for (shelf s : shelves.values()) {
            HashMap<String, Integer> stock = s.getStock();
            for (String type : stock.keySet()) {
                if (!isKnown(type)) {
                    System.out.println("Porušena integrita: položka regálu "+s.getShelfId()+" s ID "+type+" je pro sklad neznámá!");
                    integrity = false;
                }
            }
        }
        return integrity;
    }

    public void printInfo() {
        System.out.println("Známé typy a jejich názvy:");
        for (String type : knownTypes.keySet()) {
            System.out.println("\t"+type + ", Zobrazované jméno: " + knownTypes.get(type));
        }
        System.out.println("Obsahuje regály:");
        for (shelf s : shelves.values()) {
            s.printInfo();
        }
    }
}
