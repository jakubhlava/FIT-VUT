#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(135, 240);  // inicializace displeje

#define STARTSTOP 35    // tlačítko pro start a stop čítání + reset
#define SPLITBUTTON 0   // tlačítko pro tvorbu a procházení mezičasů
#define LONG_PRESS 1000   // minimální délka dlouhého stisku tlačítka v ms
#define PRESS_PAUSE 100   // doba mezi stisknutími, kdy tlačítko nereaguje aby se předešlo nechtěným dvojklikům

#define SPLIT_CAPACITY 99  // maximální počet mezičasů v paměti

// struktura pro ukládání mezičasů
typedef struct split {
  int minutes;
  int sec;
  int ms;
} split;

hw_timer_t * timer = NULL;      // struktura časovače/čitače
split splits[SPLIT_CAPACITY];   // pole uložených mezičasů
split save;                     // mezičas k uložení
int saved_splits = 0;           // aktuální počet uložených mezičasů
int shown = 0;                  // aktuálně zobrazený mezičas
// stavové proměnné nastavované obsluhou přerušení pro zajíštění překreslení/přepnutí v hlavní smyčce
bool savesplit = false;         
bool showsplits = false;
bool nextsplit = false;
bool shouldrun = false;
bool canreset = false;
bool running = false;
int mode = 0;                   // režim stopek (0 = stopky, 1 = prohlížení mezičasů)  
char buf[9];                    // buffer pro sprinf k vykreslování času
// napočítaný čas
int ms = 0;   
int sec = 0; 
int minutes = 0;
// poslední zobrazený čas (kvůli překreslení)
int lastmin = 0; 
int lastsec = 0; 
int lastms = 0;
// čas posedlních stisků tlačítek kvůli ochraně proti dvojklikům
int lastpress_ss = 0;
int lastpress_split = 0;
// pomocné proměnné pro obsluhu dlouhého stisku tlačítka
int lastState = HIGH;
int lastTime = 0;

void setup() {
  // inicializace tlačítek
  pinMode(STARTSTOP, INPUT_PULLUP);
  pinMode(SPLITBUTTON, INPUT_PULLUP);
  // inciializace počátečního stavu displeje
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(4);
  tft.drawString("00:00.00", 15, tft.height()/2 - 16);
  tft.setTextSize(2);
  tft.drawString("Splits:", 15, tft.height()-32);
  tft.drawString("00/"+String(SPLIT_CAPACITY), tft.width()-15-60, tft.height()-32);
  tft.setTextSize(4);
  // nastavení a inicializacce časovače
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &updateTime, true);
  timerAlarmWrite(timer, 10000, true);
  // připojení obsluh přerušení pro ovládací tlačítka
  attachInterrupt(STARTSTOP, handleStartStop, CHANGE);
  attachInterrupt(SPLITBUTTON, handleSplit, FALLING);
}

void loop() {
  // obsluha spuštění čítání
  if (shouldrun && !running) {
    if (mode == 1) {
      tft.setTextColor(TFT_BLACK);      
      sprintf(buf, "%02d:%02d.%02d", splits[shown].minutes, splits[shown].sec, splits[shown].ms);
      tft.setTextSize(2);      
      tft.drawString("Split time "+String(shown+1), 15, 32);  
      tft.setTextSize(4);
      tft.drawString(buf, 15, tft.height()/2 - 16);
    }
      mode = 0;
      running = true;
      timerAlarmEnable(timer);
      canreset = true;
  } 
  // obsluha pozastavení čítání
  if (!shouldrun && running) {
    running = false;
    timerAlarmDisable(timer);
  }
  // obsluha čítání
  if (running) {
    // kopie načítaného času aby se hodnoty pomocí přerušení nezměnily během vykonávání hlavní smyčky
    int mslock = ms;
    int slock = sec;
    int minlock = minutes;
    tft.setTextColor(TFT_BLACK);    
    if (lastmin != minlock) {  // vyčištění minut, pokud se změnily
      sprintf(buf, "%02d", lastmin);
      tft.drawString(buf, 15, tft.height()/2 - 16);
    }
    if (lastsec != slock) {  // vyčištění vteřin
      sprintf(buf, "   %02d", lastsec);  
      tft.drawString(buf, 15, tft.height()/2 - 16);
    }
    sprintf(buf, "      %02d", lastms);  // milisekundy se změnily prakticky jistě - čistíme při každém průběhu
    tft.drawString(buf, 15, tft.height()/2 - 16);
    tft.setTextColor(TFT_WHITE);
     // vykreslení nového času
    sprintf(buf, "%02d:%02d.%02d", minlock, slock, mslock);
    tft.drawString(buf, 15, tft.height()/2 - 16);
    lastmin = minlock;
    lastsec = slock;
    lastms = mslock;
    delay(75);
  }
  
  // vymazání aktuálního mezičasu z displeje a posun na další
  if (nextsplit) {
    tft.setTextColor(TFT_BLACK);
    sprintf(buf, "%02d:%02d.%02d", splits[shown].minutes, splits[shown].sec, splits[shown].ms);
    tft.drawString(buf, 15, tft.height()/2 - 16); 
    shown = (shown + 1) % saved_splits;
    showsplits = true;
    nextsplit = false;   
    
  }
  // přepnutí do modu mezičasů a vykreslení vybraného mezičasu
  if (showsplits) {
    mode = 1;
    showsplits = false;
    shouldrun = false;    
    tft.setTextColor(TFT_BLACK);
    sprintf(buf, "%02d:%02d.%02d", lastmin, lastsec, lastms);
    tft.drawString(buf, 15, tft.height()/2 - 16);
    tft.setTextSize(2);
    tft.drawString("Split time "+String(shown > 0 ? shown : saved_splits), 15, 32);    
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(4);
    sprintf(buf, "%02d:%02d.%02d", splits[shown].minutes, splits[shown].sec, splits[shown].ms);
    tft.drawString(buf, 15, tft.height()/2 - 16);
    tft.setTextSize(2);
    tft.drawString("Split time "+String(shown+1), 15, 32);   
    tft.setTextSize(4);
  }
  // obsluha uložení mezičasu + překreslení počítadla mezičasů
  if (savesplit) {
    savesplit = false;
    char splitbuf[3];
      sprintf(splitbuf, "%02d", saved_splits);
      tft.setTextSize(2);
      tft.setTextColor(TFT_BLACK);
      tft.drawString(splitbuf, tft.width()-15-60, tft.height()-32);
      tft.setTextColor(TFT_WHITE);
    splits[saved_splits] = save;
    saved_splits++;
    sprintf(splitbuf, "%02d", saved_splits);
    tft.drawString(splitbuf, tft.width()-15-60, tft.height()-32);
    tft.setTextSize(4);
  }
  // obsluha resetu podržením
  int state = digitalRead(STARTSTOP);
  int time = millis();
  if (lastState == LOW && state == LOW && time-lastTime > LONG_PRESS && canreset) {
    reset();
  }
}

// vynulování všech relevantních proměnných
void reset() {
  saved_splits = 0;
  shown = 0;
  savesplit = false;
  showsplits = false;
  nextsplit = false;
  shouldrun = false;
  canreset = false;
  running = false;
  mode = 0;
  canreset = false;
  tft.fillScreen(TFT_BLACK);
  tft.drawString("00:00.00", 15, tft.height()/2 - 16);
  shouldrun = false;
  running = false;
  timerAlarmDisable(timer);
  minutes = 0; sec = 0; ms = 0;
  saved_splits = 0;
  tft.setTextSize(2);
  tft.drawString("Splits:", 15, tft.height()-32);
  tft.drawString("00/"+String(SPLIT_CAPACITY), tft.width()-15-60, tft.height()-32);
  tft.setTextSize(4);
}

// obsluha časového přerušení, přepočet všech časových proměnných
void updateTime() {
  if (ms == 99) {
    ms = 0;
    if (sec == 59) {
      sec = 0;
      if (minutes == 99) {
        minutes = 0;
        sec = 0;
        ms = 0;
      } else {
        minutes++;
      }
    } else {
      sec++;
    }
  } else {
    ms++;
  }
}

// obsluha přerušení z tlačítka na start/stop/reset
void handleStartStop() {
  if (millis() - lastpress_ss > PRESS_PAUSE) {
    int state = digitalRead(STARTSTOP);
    int time = millis();
    if (lastState == HIGH && state == LOW) {
        shouldrun = !shouldrun;
    } 
    lastState = state;
    lastTime = time;
    lastpress_ss = millis();
  }
}

// obsluha přerušení z mezičasového tlačítka
void handleSplit() {
  if (millis() - lastpress_split > PRESS_PAUSE && digitalRead(SPLITBUTTON) == LOW) {
    if (running) {
      if (saved_splits < SPLIT_CAPACITY) {
        savesplit = true;
        save.ms = ms;
        save.sec = sec;
        save.minutes = minutes;
      }
    } else if (saved_splits > 0) {
      if (mode == 1) {
        nextsplit = true;
      } else {
        shown = 0;                        
        showsplits = true;
      }
    }
    lastpress_split = millis();
  }
}
