package cart;

import request.request;

import java.util.HashMap;

/**
 * Třída pro úkoly vozíků, které vznikají z požadavků uživatele
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class task {
    private HashMap<String, Integer> goods; // zboží a počty kusů k vyzvednutí
    private int shelfId;                    // ID regálu odkud zboží vyzvednout
    private request parent;                 // požadavek, ke kterému úkol náleží (pro odebírání předmětů, mazání, ...)

    /**
     * Konstruktor
     * @param shelfId   ID regálu, odkud se bude zboží brát
     * @param r         požadavek, ke kterému úkol patří
     */
    public task(Integer shelfId, request r) {
        goods = new HashMap<>();
        this.shelfId = shelfId;
        parent = r;
    }

    /**
     * Vrátí seznam zboží v úkolu
     * @return  seznam zboží
     */
    public HashMap<String, Integer> getGoods() {
        return goods;
    }

    /**
     * Vrátí ID regálu, kde se bude úkol plnit
     * @return  ID regálu
     */
    public int getShelfId() {
        return shelfId;
    }

    /**
     * Změní ID regálu pro splnění
     * @param shelfId   nové ID regálu
     */
    public void setShelfId(int shelfId) {
        this.shelfId = shelfId;
    }

    /**
     * Přidá zboží do úkolu
     * @param name      identifikátor zboží
     * @param count     počet kusů
     */
    public void addGoods(String name, Integer count) {
        if (goods.containsKey(name)) {
            goods.put(name, goods.get(name)+count);
        } else {
            goods.put(name, count);
        }
    }

    /**
     * Vrátí původní požadavek
     * @return  původní požadavek
     */
    public request getParent() {
        return parent;
    }

    /**
     * Odebere zboží z úkolu a zároveň i z požadavku (splnění úkolu)
     * @param name      identifikátor zboží
     * @param count     počet kusů
     * @throws Exception pokud je množství odebíraného zboží vyšší než počet v úkolu nebo v úkolu takové zboží není
     */
    public void removeGoods(String name, Integer count) throws Exception {
        if (!goods.containsKey(name)) {
            throw new Exception("Zboží "+ name +" v tomto seznamu není!");
        } else {
            if (goods.get(name) < count) {
                throw new Exception("Ze seznamu nelze odebrat tolik zboží, požadavek: " + count + ", v seznamu:" + goods.get(name));
            } else {
                goods.put(name, goods.get(name)-count);
            }
            if (goods.get(name) == 0) {
                goods.remove(name);
            }
            parent.removeItem(name, count);
        }
    }

    /**
     * Přiřadí předvytvořený HashMap se zbožím a počty kusů
     * @param goods hashmap se zbožím pro úkol
     */
    public void setGoods(HashMap<String, Integer> goods) {
        this.goods = goods;
    }

}
