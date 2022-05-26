import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import request.*;
import storage.*;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

public class kubaDemo {
    private storage s;

    public static void main(String[] args) throws Exception {
        /*Gson gson = new GsonBuilder().setPrettyPrinting().create();
        requestQueue rq = new requestQueue();
        request r1 = new request();
        r1.addItem("banan", 10);
        r1.addItem("jabko", 20);

        request r2 = new request();
        r2.addItem("stul", 1);
        r2.addItem("zidle", 4);
        rq.addRequest(r1);
        rq.addRequest(r2);
        String json = gson.toJson(rq.getRequests());
        System.out.println(json);
        // serializace se specialnim typem
        requestQueue rq2 = new requestQueue("requests.json");
        json = gson.toJson(rq.getRequests());
        System.out.println(json);*/

        System.out.println("Ukázka tvorby regálů napřímo...");
        // Vytvořím prázdný sklad
        storage mainStorage = new storage();
        System.out.println("================================\nPrázdný storage:");
        mainStorage.printInfo();

        // Doplním známé typy
        mainStorage.addType("jablka", "Jablka");
        mainStorage.addType("hrusky", "Hrušky");
        mainStorage.addType("jablka-gold", "Jablka Golden Delicious");
        // Průběžně vypíšeme, co se mění

        System.out.println("================================\nStorage po doplnění několika typů:");
        mainStorage.printInfo();

        mainStorage.addType("pomerance", "Pomeranče");
        mainStorage.addType("mandarinky", "Mandarinky");
        mainStorage.addType("rajce-cz", "Česká rajčata");
        mainStorage.addType("rajce-dov", "Rajčata z dovozu");

        System.out.println("================================\nStorage po doplnění dalších typů, vytvoření a naplnění regálu:");
        mainStorage.printInfo();
        mainStorage.createShelf(999);
        mainStorage.addItemTo(999,"rajce-cz", 50);
        mainStorage.printInfo();

        // Pro pozdější potřebu náhodného plnění
        List<String> types = Arrays.asList("jablka", "hrusky", "jablka-gold", "pomerance", "mandarinky", "rajce-cz", "rajce-dov");

        for (int i = 0; i < 10; i++) {
            mainStorage.createShelf(i);
        }

        Random random = new Random();
        // Pokud se náhodným výběrem vybere 2x stejný typ, pak se pomocí addItem navýší skladové zásoby
        for (int i = 0; i < 10; i++) {
            mainStorage.addItemTo(i, types.get(random.nextInt(7)), random.nextInt(500));
            mainStorage.addItemTo(i, types.get(random.nextInt(7)), random.nextInt(500));
        }

        System.out.println("================================\nFinální stav po naplnění:");
        // Finální stav
        mainStorage.printInfo();

        // Loading ze souboru

        System.out.println("================================\nNačítání skladu ze souboru:");
        storageBuilder sb = new storageBuilder("data/demoStorage.json");
        storage main = sb.buildFromFile();

        main.printInfo();




    }

}
