package storage;

import java.util.HashMap;

/**
 * Třída reprezentující jeden regál skladu.
 * Regály mají své ID, podle kterého se svazují s políčky na mapě a naskladněné zboží.
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
class shelf {
    private HashMap<String, Integer> stock;   /// Zboží, které regál obsahuje
    private Integer shelfId;                     /// ID regálu

    /**
     * Konstruktor třídy shelf
     * @param id    identifikátor regálu, používaný pro spojení s mapou
     */
    shelf(Integer id) {
        stock = new HashMap<>();
        shelfId = id;
    }

    /**
     * Přidá zboží do regálu
     * V případě, že typ zboží již v regálu je, doplní do dané položky kusy zboží, které byly naskladněny,
     * jinak vytvoří novou položku s daným počtem kusů zboží a vloží ji do regálu.
     * @param type  identifikátor typu zboží
     * @param qty   počet kusů, vkládaných do regálu
     */
    void addItem(String type, int qty) {
        if (stock.containsKey(type)) {
            stock.put(type, stock.get(type) + qty);
        } else {
            stock.put(type, qty);
        }
    }

    /**
     * Zjišťuje, zda regál obsahuje daný typ předmětu
     * @param type  identifikátor typu zboží
     * @return  výsledek hledání
     */
    boolean hasItem(String type) {
        return stock.containsKey(type);
    }

    /**
     * Zjišťuje množství kusů na skladě
     * @param type  identifikátor typu zboží
     * @return  počet kusů na skladě nebo -1, pokud předmět v regálu vůbec není
     */
    int getQty(String type) {
        return stock.getOrDefault(type, -1);
    }

    /**
     * @return  identifikátor regálu
     */
    Integer getShelfId() {
        return shelfId;
    }

    /**
     * Vyskladní zboží z regálu
     * @param type          identifikátor typu zobží k vyskladnění
     * @param qty           počet kusů zboží k vyskladnění
     * @throws Exception    pokud je nedostatek zboží na skladě nebo v regálu tento typ vůbec není
     */
    void pick(String type, int qty) throws Exception {
        int inStock = getQty(type);
        if (inStock == -1) {
            throw new Exception("Toto zboží se v tomto regálu nenachází!");
        } else if (inStock < qty) {
            throw new Exception("Nedostatek zboží v tomto regálu");
        }
        stock.put(type, inStock - qty);
        if (inStock - qty == 0) {
            stock.remove(type);
        }

    }

    public HashMap<String, Integer> getStock() {
        return stock;
    }

    void printInfo() {
        System.out.println("\tRegál "+shelfId);
        for (String si : stock.keySet()) {
            System.out.println("\t\t"+si+" : "+stock.get(si));
        }
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof shelf)) {
            return false;
        } else {
            return ((shelf) o).getShelfId().equals(shelfId);
        }
    }

    @Override
    public int hashCode() {
        return shelfId.hashCode();
    }
}
