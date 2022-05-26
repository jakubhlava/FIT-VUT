import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

/**
 * Hlavní třída FX aplikace
 * @author Jakub Hlava (xhlava52)
 * @author Lukáš Kraus (xkraus13)
 */
public class mainApp extends Application {

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage s) throws Exception {
        Parent scene = FXMLLoader.load(getClass().getClassLoader().getResource("app.fxml"));
        s.setTitle("Simulátor skladu");
        s.setScene(new Scene(scene));
        s.show();
    }

}
