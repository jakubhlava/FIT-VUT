package request;

import java.lang.reflect.Type;
import java.util.ArrayList;
import java.util.List;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import utils.io;

/**
 * Fronta požadavků, která spravuje seznam požadavků a umožňuje jeho konstrukci, jak manuálně, tak ze souboru.
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class requestQueue {
    private List<request> requests;

    /**
     * Konstruktor, inicializující prázdnou frontu požadavků.
     */
    public requestQueue() {
        requests = new ArrayList<>();
    }

    /**
     * Konstruktor, inicializující frontu požadavků ze souboru.
     * @param filename      cesta k souboru, obsahující frontu požadavků
     * @throws Exception    v případě neexistujícího souboru
     */
    public requestQueue(String filename) throws Exception {
        requests = new ArrayList<>();
        String content = io.readFile(filename);
        Gson gson = new Gson();
        Type reqType = new TypeToken<ArrayList<request>>(){}.getType();
        requests = gson.fromJson(content, reqType);
    }

    /**
     * Přidá požadavek na konec fronty
     * @param r     požadavek na vyskladnění
     */
    public void addRequest(request r) {
        requests.add(r);
    }

    /**
     * Odstraní a vrátí nejstarší požadavek z fronty
     * @return r    požadavek z konce fronty
     */
    public request popRequest() {
        request r = requests.get(0);
        requests.remove(0);
        return r;
    }

    /**
     * Vrátí celou frontu požadavků
     * @return  fronta požadavků
     */
    public List<request> getRequests() {
        return requests;
    }

}
