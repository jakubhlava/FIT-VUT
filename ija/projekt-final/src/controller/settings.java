package controller;

/**
 * Serializovatelná třída pro uchovávání a ukládání nastavení
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class settings {
    private int cartSize = 10;
    private int cartLimit = 10;
    private String defaultMapPath = "data/Map.json";
    private double simulationSpeed = 1.0;

    /**
     * Vrací rychlost simulace
     * @return  rychlost simulace
     */
    public double getSimulationSpeed() {
        return simulationSpeed;
    }

    /**
     * Vrací limit počtu vozíků
     * @return  limit počtu vozíků
     */
    public int getCartLimit() {
        return cartLimit;
    }

    /**
     * Vrací velikost (maximální kapacitu) vozíku
     * @return  kapacita vozíku
     */
    public int getCartSize() {
        return cartSize;
    }

    /**
     * Vrací výchozí cestu k mapě pro načtení
     * @return  cesta k mapě
     */
    public String getDefaultMapPath() {
        return defaultMapPath;
    }

    /**
     * Nastaví limit počtu vozíků ve skladu
     * @param cartLimit nový limit počtu vozíků
     */
    public void setCartLimit(int cartLimit) {
        this.cartLimit = cartLimit;
    }

    /**
     * Nastaví kapacitu vozíků
     * @param cartSize  nová kapacita vozíků
     */
    public void setCartSize(int cartSize) {
        this.cartSize = cartSize;
    }

    /**
     * Nastaví výchozí cestu k mapě, která se načítá při spuštění programu
     * @param defaultMapPath    cesta k mapě
     */
    public void setDefaultMapPath(String defaultMapPath) {
        this.defaultMapPath = defaultMapPath;
    }

    /**
     * Nastaví rychlost simulace
     * @param simulationSpeed   nová rychlost simulace
     */
    public void setSimulationSpeed(double simulationSpeed) {
        this.simulationSpeed = simulationSpeed;
    }
}
