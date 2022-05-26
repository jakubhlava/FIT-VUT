package controller;

import cart.*;
import com.google.gson.Gson;
import javafx.application.Platform;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.geometry.HPos;
import javafx.geometry.VPos;
import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.MenuItem;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.TextField;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Text;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.util.Callback;
import request.request;
import request.requestQueue;
import storage.storage;
import storage.storageBuilder;
import map.map;
import map.path;
import map.cartDepot;
import map.dropOff;
import map.shelf;
import map.wall;
import utils.io;

import java.awt.*;
import java.io.File;
import java.net.URL;
import java.util.*;
import java.util.Timer;
import java.util.concurrent.CopyOnWriteArrayList;

import static cart.cartState.*;
import static java.lang.Integer.parseInt;
import static java.lang.Math.round;
import static java.lang.System.exit;

/**
 * Controller pro hlavní okno aplikace
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class UIController implements Initializable {

    @FXML private MenuItem menu_ld_map;
    @FXML private MenuItem menu_ld_storage;
    @FXML private MenuItem menu_ld_requests;
    @FXML private Label request_state;
    @FXML private Label cart_picked;
    @FXML private Label cart_state;
    @FXML private ListView cart_list;
    @FXML private ListView cart_content;
    @FXML private ListView shelf_list;
    @FXML private ListView shelf_content;
    @FXML private ListView request_list;
    @FXML private ListView request_content;
    @FXML private BorderPane root;

    @FXML private TextField cart_size;
    @FXML private TextField cart_limit;
    @FXML private TextField default_map_path;
    @FXML private TextField jump_steps;
    @FXML private Slider slider_sim_speed;
    @FXML private Label label_sim_speed;
    @FXML private ScrollPane map_root;

    @FXML private Label lbl_step;
    @FXML private Label lbl_cart_count;
    @FXML private Label lbl_task_count;

    @FXML private TabPane tabs;
    @FXML private Tab tab_carts;
    @FXML private Tab tab_shelves;

    @FXML private Button btn_add_request;
    @FXML private Button btn_remove_request;
    @FXML private Button btn_reset;
    @FXML private Button btn_start;
    @FXML private Button btn_stop;
    @FXML private Group map_group;
    @FXML private VBox map_vbox;
    private GridPane grid;


    // ------- KONEC FXML KOMPONENT ---------

    findPath h;
    private dispatcher mainDispatcher;
    private String defaultStorage;
    private String defaultRequestQueue;

    private HashMap<String, Integer> storageMenu;
    private boolean running = false;
    private HashMap<Point, Rectangle> paths;
    private HashMap<Integer, Rectangle> shelves;

    private HashMap<cart, Circle> cartPoints;

    private boolean canBlock = true;

    private double zoomMultiplier = 0.05;
    private double curScale = 1.2;

    Timer timer;
    int tick = 0;

    /**
     * Handler tlačítka pro přidání požadavku - otevírá dialog pro vytvoření nového požadavku na sklad a přidá ho do fronty
     * @param actionEvent   akce převzatá z tlačítka
     * @throws Exception    pokud programu chybí lxml definice dialogu
     */
    @FXML
    public void add_request(ActionEvent actionEvent) throws Exception {
        if (mainDispatcher.getStorage().getKnown().isEmpty()) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nelze přidávat požadavky, pokud je sklad neinicializovaný!");
            alert.show();
            return;
        }
        Stage dialog = createReqDialog();
        dialog.showAndWait();
        request_list.setItems(FXCollections.observableArrayList(mainDispatcher.getQueue().getRequests()));
        //mainDispatcher.createTasks();
        lbl_task_count.setText(""+mainDispatcher.taskCount());
    }

    /**
     * Handler tlačítka pro odstranění požadavku z fronty
     * @param actionEvent   akce převzatá z tlačítka
     * @throws Exception pokud se nepodaří zničit úkoly pro tento požadavek
     */
    @FXML
    public void remove_request(ActionEvent actionEvent) throws Exception {
        request r;
        try {
            r = (request) request_list.getSelectionModel().getSelectedItem();
        } catch (Exception e) {
            return;
        }
        mainDispatcher.getQueue().removeRequest(r);
        request_list.setItems(FXCollections.observableArrayList(mainDispatcher.getQueue().getRequests()));
        request_content.setItems(FXCollections.observableArrayList());
        mainDispatcher.destroyTasksFor(r);
        lbl_task_count.setText(""+mainDispatcher.taskCount());
    }

    /**
     * Otvírá dialog a předává mu objekt skladu a fronty požadavků
     * @return Stage s dialogem
     * @throws Exception    pokud programu chybí lxml definice dialogu
     */
    public Stage createReqDialog() throws Exception {
        FXMLLoader loader = new FXMLLoader(getClass().getClassLoader().getResource("add_request.fxml"));
        Stage s = new Stage();
        s.setScene(new Scene(loader.load()));
        s.setTitle("Přidat požadavek");
        reqController c = loader.getController();
        c.setDispatcher(mainDispatcher);

        return s;
    }

    /**
     * Obsluha dialogu pro načtení mapy po startu
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void load_map(ActionEvent actionEvent) {
        FileChooser fc = new FileChooser();
        fc.setTitle("Vyberte soubor s mapovým podkladem...");
        fc.getExtensionFilters().add(new FileChooser.ExtensionFilter("Soubory mapových podkladů", "*.json"));
        fc.setInitialDirectory(new File("./"));
        File picked = fc.showOpenDialog(root.getScene().getWindow());

        if (picked == null) {
            return;
        }

        mainDispatcher.setMap(picked.getPath());
        map_group.getChildren().clear();
        map_init();
    }

    /**
     * Otevření dialogu a načtení skladu ze souboru
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void load_storage(ActionEvent actionEvent) {
        FileChooser fc = new FileChooser();
        fc.setTitle("Vyberte soubor se stavem skladu...");
        fc.getExtensionFilters().add(new FileChooser.ExtensionFilter("Soubory skladu", "*.json"));
        fc.setInitialDirectory(new File("./"));
        File picked = fc.showOpenDialog(root.getScene().getWindow());

        if (picked == null) {
            return;
        }

        storageBuilder sb = new storageBuilder(picked.getPath());
        try {
            mainDispatcher.setStorage(sb.buildFromFile());
        } catch (Exception e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nepodařilo se načíst sklad!");
            alert.show();
            e.printStackTrace();
        }
        storageMenu = new HashMap<>();
        for (Integer id : mainDispatcher.getStorage().getShelfIds()) {
            storageMenu.put("Regál " + id, id);
        }
        ObservableList<String> shelves = FXCollections.observableArrayList(storageMenu.keySet()).sorted();
        shelf_list.setItems(FXCollections.observableArrayList(mainDispatcher.getStorage().getShelfIds()).sorted());
        // reset zbytku UI
        request_list.setItems(FXCollections.observableArrayList());
        request_content.setItems(FXCollections.observableArrayList());
        shelf_content.setItems(FXCollections.observableArrayList());
    }

    /**
     * Otevření dialogu a načtení požadavků ze souboru
     * @param actionEvent   akce převzatá z tlačítka
     * @throws Exception    pokud soubor neexistuje
     */
    @FXML
    public void load_requests(ActionEvent actionEvent) throws Exception {
        if (mainDispatcher.getStorage().getKnown().isEmpty()) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nelze přidávat požadavky, pokud je sklad neinicializovaný!");
            alert.show();
            return;
        }
        FileChooser fc = new FileChooser();
        fc.setTitle("Vyberte soubor s požadavky k vyskladnění...");
        fc.getExtensionFilters().add(new FileChooser.ExtensionFilter("Soubory požadavků", "*.json"));
        fc.setInitialDirectory(new File("./"));
        File picked = fc.showOpenDialog(root.getScene().getWindow());

        if (picked == null) {
            return;
        }

        requestQueue rq;
        try {
            rq = new requestQueue(picked.getPath());
        } catch (Exception e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nepodařilo se načíst požadavky!");
            alert.show();
            e.printStackTrace();
            return;
        }
        try {
            mainDispatcher.setQueue(rq);
        } catch (Exception e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Chyba při přidávání fronty: " + e.getMessage());
            alert.show();
            e.printStackTrace();
            return;
        }
        request_list.setItems(FXCollections.observableArrayList(mainDispatcher.getQueue().getRequests()));
        //mainDispatcher.createTasks();
        lbl_task_count.setText(""+mainDispatcher.taskCount());
    }

    /**
     * Otevře dialogová okno pro výběr souboru a vyexportuje seznam požadavků
     * @param actionEvent   akce převzatá z tlačítka
     */
    public void save_requests(ActionEvent actionEvent) {
        FileChooser fc = new FileChooser();
        fc.setTitle("Uložit jako...");
        fc.getExtensionFilters().add(new FileChooser.ExtensionFilter("Soubory požadavků", "*.json"));
        fc.setInitialDirectory(new File("./"));
        File picked = fc.showSaveDialog(root.getScene().getWindow());
        Gson gson = new Gson();
        String queue = gson.toJson(mainDispatcher.getQueue().getRequests());
        try {
            utils.io.writeFile(picked.getPath(),queue);
        } catch (Exception e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nepodařilo se uložit požadavky!");
            alert.show();
            e.printStackTrace();
        }
    }

    /**
     * Nastaví maximální velikost vozíku
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void set_cart_size(ActionEvent actionEvent) {
        Integer cart_size_val;
        try {
            cart_size_val = parseInt(cart_size.getText());
        } catch (NumberFormatException e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nesprávný formát čísla!");
            alert.show();
            return;
        }
        if (cart_size_val < 1) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Do vozíku se musí vejít alespoň jeden předmět!");
            alert.show();
            return;
        }
        mainDispatcher.getSettings().setCartSize(cart_size_val);
    }

    /**
     * Nastaví výchozí mapový podklad
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void set_map_file(ActionEvent actionEvent) {
        mainDispatcher.getSettings().setDefaultMapPath(default_map_path.getText());
    }

    /**
     * Otevře dialog pro výběr souboru s mapovým podkladem
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void pick_map_file(ActionEvent actionEvent) {
        FileChooser fc = new FileChooser();
        fc.setTitle("Vyberte soubor s mapovým podkladem...");
        fc.getExtensionFilters().add(new FileChooser.ExtensionFilter("Mapové podklady", "*.json"));
        fc.setInitialDirectory(new File("./"));
        File picked = fc.showOpenDialog(root.getScene().getWindow());
        if (picked == null) {
            return;
        }
        default_map_path.setText(picked.getPath());
    }

    /**
     * Nataví rychlost simulace
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void set_sim_speed(ActionEvent actionEvent) {
        mainDispatcher.getSettings().setSimulationSpeed(round(slider_sim_speed.getValue()));
        if (isRunning()) {
            stopTimer();
            startTimer();
        }
    }

    /**
     * Poskočí o počet kroků simulace podle požadvku uživatele
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void jump_to_step(ActionEvent actionEvent) {
        Integer step;
        try {
            step = parseInt(jump_steps.getText());
        } catch (NumberFormatException e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nesprávný formát čísla!");
            alert.show();
            return;
        }
        if (step < 0) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Cestovat zpět časem nelze!");
            alert.show();
            return;
        }
        mainDispatcher.fastForward(step);
        tick += step;
        tickView();
    }

    /**
     * Uloží nastavení do souboru
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void save_settings(ActionEvent actionEvent) {
        try {
            mainDispatcher.saveSettings();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /**
     * Nastaví limit na počet vozíků
     * @param actionEvent   akce převzatá z tlačítka
     */
    @FXML
    public void set_cart_limit(ActionEvent actionEvent) {
        Integer limit;
        try {
            limit = parseInt(cart_limit.getText());
        } catch (NumberFormatException e) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nesprávný formát čísla!");
            alert.show();
            return;
        }
        if (limit < 1) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Musí být povolen alespoň jeden vozík!");
            alert.show();
            return;
        }
        mainDispatcher.getSettings().setCartLimit(limit);
    }

    /**
     * Inicializace okna
     * @param url   url
     * @param rb    resourceBundle
     */
    public void initialize(URL url, ResourceBundle rb) {

        timer = new Timer(true);
        cartPoints = new HashMap<>();

        btn_reset.setDisable(true);
        btn_stop.setDisable(true);

        try {
            mainDispatcher = new dispatcher();
        } catch (Exception e) {
            e.printStackTrace();
            exit(1);
        }
        // Zobrazení nastavení na kartě
        cart_size.setText(""+mainDispatcher.getSettings().getCartSize());
        default_map_path.setText(mainDispatcher.getSettings().getDefaultMapPath());
        slider_sim_speed.setValue(mainDispatcher.getSettings().getSimulationSpeed());
        cart_limit.setText(""+mainDispatcher.getSettings().getCartLimit());

        storageMenu = new HashMap<>();
        shelf_list.getSelectionModel().selectedItemProperty().addListener( // listener pro detaily regálů
                new ChangeListener<Integer>() {
                    @Override
                    public void changed(ObservableValue<? extends Integer> observableValue, Integer oldv, Integer newv) {
                        if (newv == null) return;
                        shelf_content.setItems(FXCollections.observableArrayList(mainDispatcher.makeShelfContentList(newv)));
                    }
                }
        );
        shelf_list.setCellFactory(
                new Callback<ListView, ListCell>() {
                    @Override
                    public ListCell call(ListView listView) {
                        ListCell<Integer> cell = new ListCell<>() {
                            @Override
                            protected void updateItem(Integer item, boolean empty) {
                                super.updateItem(item, empty);
                                try {
                                    setText("Regál "+item);
                                } catch (Exception e) {
                                    setText("");
                                }
                            }
                        };
                        cell.setOnMouseClicked(e -> {
                            if (!cell.isEmpty()) {
                                hideCartPaths();
                                unmarkShelves();
                                markShelf(cell.getItem());
                            }
                        });
                        return cell;
                    }
                }
        );
        request_list.getSelectionModel().selectedItemProperty().addListener( // detaily požadavků
                new ChangeListener<request>() {
                    @Override
                    public void changed(ObservableValue<? extends request> observableValue, request oldv, request newv) {
                        if (newv == null) return;
                        request_content.setItems(FXCollections.observableArrayList(mainDispatcher.makeRequestContentList(newv)));
                    }
                }
        );
        request_list.setCellFactory( // přepis request objektů na popisky v seznamu požadavků
                new Callback<ListView, ListCell>() {
                    @Override
                    public ListCell call(ListView listView) {
                        ListCell<request> cell = new ListCell<>() {
                            @Override
                            protected void updateItem(request item, boolean empty) {
                                super.updateItem(item, empty);
                                try {
                                    setText("Požadavek "+mainDispatcher.getQueue().getPersistentID(item));
                                } catch (Exception e) {
                                    setText("");
                                }
                            }
                        };
                        cell.setOnMouseClicked(e -> {
                            if (!cell.isEmpty()) {
                                hideCartPaths();
                                unmarkShelves();
                                for (CopyOnWriteArrayList<task> tl : mainDispatcher.getTaskList()) {
                                    for (task t: tl) {
                                        if (t.getParent() == cell.getItem()) {
                                            markShelf(t.getShelfId());
                                        }
                                    }
                                }
                            }
                        });
                        return cell;
                    }
                }
        );
        cart_list.setCellFactory( // přepis cart objektů na popisky v seznamu vozíků
                new Callback<ListView, ListCell>() {
                    @Override
                    public ListCell call(ListView listView) {
                        ListCell<cart> cell = new ListCell<>() {
                            @Override
                            protected void updateItem(cart item, boolean empty) {
                                super.updateItem(item, empty);
                                try {
                                    setText("Vozík "+item.getID());
                                } catch (Exception e) {
                                    setText("");
                                }
                            }
                        };
                        cell.setOnMouseClicked(e -> {
                            if (!cell.isEmpty()) {
                                cart_content.setItems(FXCollections.observableArrayList(mainDispatcher.makeCartContentList(cell.getItem())));
                                switch (cell.getItem().getState()) {
                                    case CART_ON_ROUTE:
                                        cart_state.setText("Na cestě");
                                        break;
                                    case CART_PICKUP:
                                        cart_state.setText("Nabírá zboží");
                                        break;
                                    case CART_FINISHED:
                                        cart_state.setText("Dokončil úkoly");
                                        break;
                                    case CART_WAITING:
                                        cart_state.setText("Čeká na práci");
                                        break;
                                }
                                unmarkShelves();
                                hideCartPaths();
                                showCartPath(cell.getItem());
                            }
                        });
                        return cell;
                    }
                }
        );
        slider_sim_speed.valueProperty().addListener( // vizualizace hodnoty ve slideru
                new ChangeListener<Number>() {
                    @Override
                    public void changed(ObservableValue<? extends Number> observableValue, Number number, Number newv) {
                        label_sim_speed.setText(""+round((Double) newv));
                    }
                }
        );
        map_init();
    }

    /**
     * Inicializuje zobrazení mapy podle aktuálně načtené mapy v dispatcheru
     */
    private void map_init() {

        grid = new GridPane();
        map_group.getChildren().add(grid);

        paths = new HashMap<Point, Rectangle>();
        shelves = new HashMap<Integer, Rectangle>();

        // Ochrana při zadání špatné cesty k mapě
        if (mainDispatcher.getMap() == null) {
            Alert alert = new Alert(Alert.AlertType.ERROR, "Nepodařilo se načíst mapový podklad, zkontrolujte a upravte cestu v záložce nastavení a restartujte aplikaci!\nAplikace nyní pokračuje v nouzovém režimu!");
            alert.showAndWait();
            return;
        }

        map map = mainDispatcher.getMap();
        Object[] tiles = map.getMap();


        for (int i = 0; i < map.getY_size(); i++) {
            ColumnConstraints col = new ColumnConstraints();
            col.setHalignment(HPos.CENTER);
            grid.getColumnConstraints().add(col);
            for (int j = 0; j < map.getX_size(); j++) {
                RowConstraints row = new RowConstraints ();
                row.setValignment(VPos.CENTER);
                grid.getRowConstraints().add(row);
                Text text = new Text();
                final int posx = i;
                final int posy = j;
                Rectangle r = new Rectangle(800.0/map.getY_size(),600.0/map.getX_size());
                r.setStroke(new Color(0 / 255.0, 0 / 255.0, 0 / 255.0, 1.0));
                r.setStrokeWidth(0.7);
                if (tiles[j * map.getY_size() + i] instanceof path) {
                    r.setFill(new Color(70 / 255.0, 70 / 255.0, 70 / 255.0, 1.0));
                    switch (((path)tiles[j * map.getY_size() + i]).checkDirection()) {
                            case 0: text.setText("↑"); break;
                            case 3: text.setText("↓"); break;
                            case 1: text.setText("←"); break;
                            case 2: text.setText("→"); break;
                            default: text.setText("↕"); break;
                        }
                    r.addEventHandler(MouseEvent.ANY, e -> {
                        if (e.getButton() == MouseButton.PRIMARY) {
                            hideCartPaths();
                            if(canBlock) {
                                if (((path) tiles[io.getPosInArray(posy, posx, map.getY_size())]).checkIsOpen()) {
                                    ((path) tiles[io.getPosInArray(posy, posx, map.getY_size())]).closePath();
                                    r.setFill(new Color(10 / 255.0, 10 / 255.0, 10 / 255.0, 1.0));
                                    //text.setFill(new Color(0 / 255.0, 0 / 255.0, 0 / 255.0, 0.0));
                                } else {
                                    ((path) tiles[io.getPosInArray(posy, posx, map.getY_size())]).openPath();
                                    r.setFill(new Color(70 / 255.0, 70 / 255.0, 70 / 255.0, 1.0));
                                    //text.setFill(new Color(40 / 255.0, 40 / 255.0, 40 / 255.0, 1.0));
                                }
                            }
                            e.consume();
                        }
                    });
                    paths.put(new Point(posy,posx),r);
                    text.setFill(new Color(40/255.0,40/255.0,40/255.0, 1.0));
                } else if (tiles[j * map.getY_size() + i] instanceof wall) {
                    r.addEventHandler(MouseEvent.ANY, e -> {
                        if (e.getButton() == MouseButton.PRIMARY) {
                            hideCartPaths();
                            e.consume();
                        }
                    });
                    r.setFill(new Color(0 / 255.0, 0 / 255.0, 0 / 255.0, 1.0));
                } else if (tiles[j * map.getY_size() + i] instanceof shelf) {
                    r.setFill(new Color(100 / 255.0, 40 / 255.0, 0 / 255.0, 1.0));
                    r.addEventHandler(MouseEvent.ANY, e -> {
                        if (e.getButton() == MouseButton.PRIMARY) {
                            hideCartPaths();
                            selectShelf(io.getPosInArray(posy, posx, map.getY_size()));
                            e.consume();
                        }
                    });
                    shelves.put(posy * map.getY_size() + posx,r);
                } else if (tiles[j * map.getY_size() + i] instanceof cartDepot) {
                    r.addEventHandler(MouseEvent.ANY, e -> {
                        if (e.getButton() == MouseButton.PRIMARY) {
                            hideCartPaths();
                            e.consume();
                        }
                    });
                    r.setFill(new Color(255 / 255.0, 255 / 255.0, 0 / 255.0, 1.0));
                } else if (tiles[j * map.getY_size() + i] instanceof dropOff) {
                    r.addEventHandler(MouseEvent.ANY, e -> {
                        if (e.getButton() == MouseButton.PRIMARY) {
                            hideCartPaths();
                            e.consume();
                        }
                    });
                    r.setFill(new Color(255 / 255.0, 0 / 255.0, 255 / 255.0, 1.0));
                }


                text.setDisable(true);

                grid.add(r, i, j);
                grid.add(text, i, j);
            }
        }

        map_vbox.getChildren().clear();
        map_vbox.getChildren().add(map_group);
        grid.setScaleX(curScale);
        grid.setScaleY(curScale);

        map_vbox.setOnScroll(e -> {
            e.consume();
            double zoom = Math.exp(e.getTextDeltaY() * zoomMultiplier);
            curScale *= zoom;
            grid.setScaleX(curScale);
            grid.setScaleY(curScale);
            map_root.layout();

        } );
    }

    /**
     * Zvýrazní regál např. po kliknutí na mapě
     * @param shelfId   ID regálu
     */
    private void selectShelf(int shelfId) {
        unmarkShelves();
        markShelf(shelfId);
        tabs.getSelectionModel().select(tab_shelves);
        shelf_list.getSelectionModel().select(shelfId);
        shelf_content.setItems(FXCollections.observableArrayList(mainDispatcher.makeShelfContentList(shelfId)));
    }

    /**
     * Zvýrazní na mapě cestu vozíku
     * @param c vozík
     */
    public void showCartPath(cart c) {
        int routelen = c.getPath().getRoute().size();
        int pos = c.getPathStep();

        for (int i = pos+1; i < routelen; i++) {
            paths.get(c.getPath().getRoute().get(i)).setFill(Color.DARKRED);
        }
        for (task t : c.getTasks()) {
            markShelf(t.getShelfId());
        }
        for (Point p: c.getPath().getStops()) {
            paths.get(p).setFill(Color.DARKGREEN);
        }
        paths.get(c.getPath().getRoute().get(pos)).setFill(Color.WHITESMOKE);
    }

    /**
     * Odstraní zvýraznění cesty z mapy
     */
    public void hideCartPaths() {
        unmarkShelves();
        for (Point p : paths.keySet()) {
             if (((path) mainDispatcher.getMap().getMap()[io.getPosInArray(p.x, p.y, mainDispatcher.getMap().getY_size())]).checkIsOpen()) {
                 paths.get(p).setFill(new Color(70 / 255.0, 70 / 255.0, 70 / 255.0, 1.0));
             } else {
                 paths.get(p).setFill(new Color(10 / 255.0, 10 / 255.0, 10 / 255.0, 1.0));
             }
        }
    }

    /**
     * Označí v mapě regál
     * @param shelfId   číslo regálu
     */
    public void markShelf(int shelfId) {
        shelves.get(shelfId).setFill(new Color(195 / 255.0, 90 / 255.0, 20 / 255.0, 1.0));
    }

    /**
     * Odznačí všechny regály
     */
    public void unmarkShelves() {
        for (Rectangle r : shelves.values()) {
            r.setFill(new Color(100 / 255.0, 40 / 255.0, 0 / 255.0, 1.0));
        }
    }

    /**
     * Zahájí timer pro simulaci
     * @param actionEvent   akce převzatá z tlačítka
     * @throws Exception pokud se nepodaří vytvořit úkoly pro vozíky
     */
    public void start_simulation(ActionEvent actionEvent) throws Exception {
        btn_add_request.setDisable(true);
        btn_remove_request.setDisable(true);
        btn_reset.setDisable(true);
        btn_stop.setDisable(false);
        btn_start.setDisable(true);
        menu_ld_map.setDisable(true);
        menu_ld_storage.setDisable(true);
        menu_ld_requests.setDisable(true);
        if (!isRunning()) {
            canBlock = false;
            Gson gson = new Gson();
            // serializace pro pozdější reset
            defaultStorage = gson.toJson(mainDispatcher.getStorage());
            defaultRequestQueue = gson.toJson(mainDispatcher.getQueue().getRequests());
            if (!mainDispatcher.createTasks()) {
                Alert alert = new Alert(Alert.AlertType.ERROR, "Nedostatek zboží v dostupných regálech!\nOdblokujte cesty nebo odstraňte některé své požadavky a zkuste to znovu.");
                alert.show();
                reset_simulation(null);
            } else {
                startTimer();
            }
        }

    }

    /**
     * Ukončí timer pro simulaci
     * @param actionEvent   akce převzatá z tlačítka
     */
    public void stop_simulation(ActionEvent actionEvent) {
        btn_add_request.setDisable(false);
        btn_remove_request.setDisable(false);
        menu_ld_requests.setDisable(false);
        btn_reset.setDisable(false);
        btn_stop.setDisable(true);
        btn_start.setDisable(false);
        if (isRunning()) {
            stopTimer();
        }
    }

    /**
     * Vrátí simulaci do původního stavu
     * @param actionEvent   akce převzatá z tlačítka
     * @throws Exception    pokud se nepodaří inicializovat frontu požadavků
     */
    public void reset_simulation(ActionEvent actionEvent) throws Exception {
        canBlock = true;
        tick = 0;
        stop_simulation(actionEvent);
        Gson gson = new Gson();
        for (cart c : cartPoints.keySet()) {
            grid.getChildren().remove(cartPoints.get(c));
        }
        cartPoints.clear();
        // Deserializace uložených
        mainDispatcher.setStorage(gson.fromJson(defaultStorage, storage.class));
        mainDispatcher.setQueue(new requestQueue());
        mainDispatcher.getQueue().deserializeQueue(defaultRequestQueue);
        mainDispatcher.reset(); // pomocný reset
        // reset zobrazení
        lbl_step.setText(""+tick);
        request_content.setItems(FXCollections.observableArrayList());
        shelf_content.setItems(FXCollections.observableArrayList());
        request_list.setItems(FXCollections.observableArrayList(mainDispatcher.getQueue().getRequests()));
        shelf_list.setItems(FXCollections.observableArrayList(mainDispatcher.getStorage().getShelfIds()).sorted());
        lbl_task_count.setText(""+mainDispatcher.taskCount());
        menu_ld_map.setDisable(false);
        menu_ld_storage.setDisable(false);
    }

    /**
     * Třída s časovanou úlohou
     */
    class ticker extends TimerTask {
        @Override
        public void run() {
            Platform.runLater( () -> {
                // Zastavení simulace po dokončení všech požadavků
                if (mainDispatcher.getQueue().getRequests().isEmpty()) {
                    stop_simulation(null);
                }
                // Tick pro dispatcher
                tick++;
                mainDispatcher.tick();
                tickView();
            });
        }
    }

    /**
     * Provede krok pohledu
     */
    public void tickView() {
        lbl_step.setText(""+tick);
        lbl_cart_count.setText(mainDispatcher.getCartsInUse().size()+"/"+(mainDispatcher.getFreeCarts().size()+mainDispatcher.getCartsInUse().size()));
        // Aktualizace seznamu vozíků a požadavků
        cart_list.setItems(FXCollections.observableArrayList(mainDispatcher.getCartsInUse()));
        request_list.setItems(FXCollections.observableArrayList(mainDispatcher.getQueue().getRequests()));
        // Pohyb vozíků
        for (cart c : mainDispatcher.getCartsInUse()) {
            if (cartPoints.containsKey(c)) {
                if (c.getState() == CART_ON_ROUTE || c.getState() == CART_PICKUP) {
                    grid.setConstraints(cartPoints.get(c), c.getPosition().y ,c.getPosition().x);
                } else {
                    grid.getChildren().remove(cartPoints.get(c));
                }
            } else {
                Circle circ = new Circle();
                circ.setRadius(8.0);
                circ.setFill(Color.color(Math.random(), Math.random(), Math.random()));
                // Zobrazení trasy vozíků
                circ.addEventHandler(MouseEvent.ANY, e -> {
                    if (e.getButton() == MouseButton.PRIMARY) {
                        hideCartPaths();
                        showCartPath(c);
                        e.consume();
                    }
                });
                grid.add(circ, c.getPosition().y, c.getPosition().x);
                cartPoints.put(c,circ);
            }
        }
        // Vyčištění od mrtvých vozíků
        for (cart c : mainDispatcher.getFreeCarts()) {
            if (cartPoints.containsKey(c)) {
                grid.getChildren().remove(cartPoints.get(c));
                cartPoints.remove(c);
            }
        }
    }

    /**
     * Pomocná funkce pro spuštění časovače
     */
    public void startTimer() {
        timer = new Timer(true);
        running = true;
        if (mainDispatcher.getSettings().getSimulationSpeed() != 0) {
            timer.schedule(new ticker(), 0, (int)(1000/mainDispatcher.getSettings().getSimulationSpeed()));
        }
        mainDispatcher.startSim();
    }

    /**
     * Pomocná funkce pro ukončení časovače
     */
    public void stopTimer() {
        running = false;
        timer.cancel();
    }

    /**
     * Vrací stav simulace běží/neběží
     * @return  true, pokud běží, jinak false
     */
    public boolean isRunning() {
        return running;
    }
}


