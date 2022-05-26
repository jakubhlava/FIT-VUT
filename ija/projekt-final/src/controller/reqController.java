package controller;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.event.ActionEvent;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import javafx.stage.Stage;
import javafx.util.Callback;
import request.request;
import request.requestQueue;
import storage.storage;

import java.net.URL;
import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashMap;
import java.util.ResourceBundle;

/**
 * Controller pro dialogové okno, které přidává zboží
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class reqController implements Initializable {

    public Button finish;
    public ListView request;
    public ComboBox goods_type;
    public TextField goods_amount;
    public Button add_to_request;
    public Button remove_selected;

    private HashMap<String, Integer> requested; // požadavky
    private HashMap<String, String> shown;      // zobrazený detail
    private String picked = "";                 // výběr v comboboxu goods_type

    private storage s;
    private requestQueue rq;
    private dispatcher d;

    /**
     * Inicializace dialogu
     * @param url   url
     * @param rb    resourceBundle
     */
    public void initialize(URL url, ResourceBundle rb) {
        goods_type.valueProperty().addListener(new ChangeListener<String>() { // Listener pro výběr typu zboží
            @Override
            public void changed(ObservableValue observableValue, String oldv, String newv) {
                picked = newv;
            }
        });
        // Přepis názvů v comboboxu z ID na popisky
        Callback<ListView, ListCell> pickerFactory = new Callback<ListView, ListCell>() {
                    @Override
                    public ListCell call(ListView listView) {
                        ListCell<String> cell = new ListCell<>() {
                            @Override
                            protected void updateItem(String item, boolean empty) {
                                super.updateItem(item, empty);
                                try {
                                    setText(s.getDisplayName(item));
                                } catch (Exception e) {
                                    setText("<neznámá položka>");
                                }
                            }
                    };
                    return cell;
                }
            };
        goods_type.setCellFactory(pickerFactory);
        goods_type.setButtonCell(pickerFactory.call(null));
        requested = new HashMap<>();
        shown = new HashMap<>();
    }

    /**
     * Uložení požadavku do fronty
     * @param actionEvent   akce převzatá z tlačítka
     */
    public void save(ActionEvent actionEvent) {
        if (!requested.isEmpty()) {
            rq.addRequest(new request(requested));
        }
        Stage stage = (Stage) finish.getScene().getWindow();
        stage.close();
    }

    /**
     * Přidání typu zboží a množství do požadavku
     * @param actionEvent   akce převzatá z tlačítka
     * @throws Exception    pokud typ zboží neexistuje
     */
    public void add(ActionEvent actionEvent) throws Exception {
        int amount;
        try {
            amount = Integer.parseInt(goods_amount.getText());
        } catch (NumberFormatException e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nesprávný formát čísla!");
            alert.show();
            return;
        }
        if (amount <= 0) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nesprávný počet kusů!\nZáporné ani nulové množství nelze vyskladnit");
            alert.show();
            return;
        }
        if (picked == "") {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Vyberte zboží!");
            alert.show();
            return;
        }
        int maxQty = d.getGlobalQty(picked);
        if (maxQty < amount) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Tohoto zboží není ve skladu dostatek (na skladě: "+maxQty+"ks) !");
            alert.show();
            return;
        }
        if (!requested.containsKey(picked)) {
            requested.put(picked, amount);
            shown.put(s.getDisplayName(picked)+" - "+amount+" ks", picked);
        } else {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Toto zboží již je v seznamu!");
            alert.show();
            return;
        }
        request.setItems(FXCollections.observableArrayList(shown.keySet()).sorted());
        goods_amount.clear();
        goods_type.getSelectionModel().select(0);
    }

    /**
     * Odstraní zboží z požadavku
     * @param actionEvent   akce převzatá z tlačítka
     */
    public void remove(ActionEvent actionEvent) {
        String text,type;
        try {
            text = (String) request.getSelectionModel().getSelectedItem();
            type = shown.get(text);
        } catch (Exception e) {
            return;
        }
        shown.remove(text);
        requested.remove(type);
        request.setItems(FXCollections.observableArrayList(shown.keySet()).sorted());
    }

    /**
     * Příjímá objekt dispatcheru z hlavního okna
     * @param d     dispatcher
     */
    public void setDispatcher(dispatcher d) {
        this.d = d;
        setStorage(d.getStorage());
        setRequestQueue(d.getQueue());
    }

    /**
     * Přijímá objekt skladu z hlavního okna
     * @param s     používaný sklad
     */
    private void setStorage(storage s) {
        this.s = s;
        goods_type.setItems(FXCollections.observableArrayList(s.getKnown()).sorted(
                new Comparator<String>() {
                @Override
                    public int compare(String a, String b) {
                    try {
                        return s.getDisplayName(a).compareTo(s.getDisplayName(b));
                    } catch (Exception e) { return 0; }
                    }
        }
        ));
        goods_type.getSelectionModel().select(0);
    }

    /**
     * Přijímá objekt fronty požadavků z hlavního okna
     * @param rq    použivaná fronta požadavků
     */
    private void setRequestQueue(requestQueue rq) {
        this.rq = rq;
    }
}
