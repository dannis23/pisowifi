#include <SPI.h>
#include <Wire.h>
#include <Entropy.h>
#include <Ethernet.h>
#include <TelnetClient.h>
#include <LiquidCrystal_I2C.h>

// #include <TimeLib.h>

#define randomSeed(s) srandom(s)

unsigned int mins = 0;
unsigned int hours = 0;
unsigned int days = 0;
unsigned int newmins = 0;
unsigned int newhours = 0;

unsigned long int times = 0;
unsigned long int hotspotTimes = 0;
unsigned int promoTime = 0;

int buttonPress = 0;
int ledState = LOW;
int gate = 0;
int interval = 0;
int start = 0;
int admin = 0;
int router_con = 0;

int add1hour = 0;
int add2hours = 0;
int add4hours = 0;
int add18hours = 0;
int promoTimeAdd1hour = 0;
int promoTimeAdd2hours = 0;
int promoTimeAdd4hours = 0;
int promoTimeAdd18hours = 0;

//Counter Timer
int counter = 0;
int timerCounter = 0;
int firstInsertCoin = 0;

int counterTest = 0;

//Countdown
int coinsCountdown, codeGeneratedCountdown;

int ConnectTry = 0;

const int ledpin = 5;
const int coinpin = 2;
const int buttonpin = 3;
const int disablepin = 4;

volatile float coins = 0;

unsigned long startMillis = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

String script;

uint32_t seed_value;
char letters[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
//char letters[10] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J'};
LiquidCrystal_I2C lcd(0x27, 20, 4);

// variable to hold Ethernet shield MAC address
byte clientMAC[] = { 0xAA, 0xBB, 0xCC, 0x00, 0xFE, 0xED };

//put here your router's ip address
IPAddress ip(192, 168, 88, 2);
IPAddress mikrotikRouterIp (192, 168, 88, 1);

IPAddress checkIP(192, 168, 88, 1);

// fill in your Domain Name Server address here:
IPAddress myDns(1, 1, 1, 1);

EthernetClient clients;
EthernetClient client;
telnetClient tc(client);

void runSetupConnection() {
  pinMode(buttonpin, INPUT);
  pinMode(ledpin, OUTPUT);
  pinMode(disablepin, OUTPUT);

  // start serial port:
  Serial.begin(9600);
  // give the ethernet module time to boot up:
  delay(1000);
  
  lcd.begin();
  lcd.backlight();

  //Initializing
  lcd.clear();
  Serial.println("initializing...");
  print_txt("initializing    ", 0, 0);
  print_txt(".", 12, 0);
  delay(500);
  print_txt(".", 13, 0);
  delay(500);
  print_txt(".", 14, 0);
  delay(500);

  Here:

  //Connecting
  lcd.clear();
  Serial.println("Check client");
  print_txt("Check client", 0, 0);
  delay(300);
  print_txt(".", 12, 0);
  delay(300);
  print_txt(".", 13, 0);
  delay(300);
  print_txt(".", 14, 0);
  
  digitalWrite(disablepin, HIGH);

  ConnectTry = 0;

  while (Ethernet.begin(clientMAC) == 0) {    
    ConnectTry += 1;
    Serial.println(ConnectTry);
    
    lcd.clear();
    Serial.println("Failed!");
    print_txt("Failed!  ", 0, 0);
    print_txt_int(ConnectTry, 19, 0);
    
    if (ConnectTry == 3) {
      Serial.println("reconnecting");
      
      goto Here;
    }
  }

  //Client Connection
  lcd.clear();
  Serial.println("Success!");
  print_txt("Success!", 0, 0);
  delay(1000);

  Ethernet.begin(clientMAC, ip, myDns);

  //Connecting Router
  lcd.clear();
  Serial.println("Check router");
  print_txt("Check router", 0, 0);
  delay(300);
  print_txt(".", 12, 0);
  delay(300);
  print_txt(".", 13, 0);
  delay(300);
  print_txt(".", 14, 0);

  tc.setPromptChar('>');
  if (tc.login(mikrotikRouterIp, "admin", "Danila12")) {
    lcd.clear();
    print_txt("Success!", 0, 0);
    Serial.println("Success!");
    delay(1000);
    lcd.clear();
  } else {
    lcd.clear();
    print_txt("Failed!", 0, 0);
    Serial.println("Failed!");
    delay(1000);
    
    goto Here;
  }

  Entropy.initialize();

  pinMode(buttonpin, INPUT);
  pinMode(ledpin, OUTPUT);
  pinMode(disablepin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(coinpin), coinInterrupt, RISING);
}  

void setup() {
  runSetupConnection();
}

void coinInterrupt() {
  coins ++;
  startMillis = millis(); //initial start time
  coinsCountdown = 30;
}

void blinker() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    digitalWrite(ledpin, ledState);
  }
}

void loop() {
  currentMillis = millis();
  if (coins < 1 && gate == 0 && start == 0) {
    if (clients.connect(checkIP, 23)) {
      char c = clients.read();
      Serial.print(c);
      Serial.println("Router connected!!!.");

      insertCoin();
      insertCoinBlink();
    } else {
      lcd.clear();
      Serial.println("Router disconnected!");
      print_txt("Router disconnected!", 0, 0);
      clients.stop();
      
      runSetupConnection();
    }
  }
  
  if ((start == 0) && (coins > 0) && (currentMillis - startMillis >= 100)) {
    
    mins = coins * 12;
    
    Serial.print("coins = ");
    Serial.println(coins);
    if (coins < 20) {
      Serial.println("coins below 20");
    }

    if((add1hour == 0) && coins >= 10 && coins < 20){
      Serial.println("coins 10 to 19");
      promoTimeAdd1hour = 60;
      add1hour = 1;
    }
    
    if((add4hours == 0) && coins >= 20 && coins < 30){
      Serial.println("coins 20 to 29");
      promoTimeAdd1hour = 0;
      promoTimeAdd4hours = 240;
      add4hours = 1;
    }

    if((add18hours == 0) && coins >= 30){
      Serial.println("coins 30 above");
      promoTimeAdd1hour = 0;
      promoTimeAdd4hours = 0;
      promoTimeAdd18hours = 1080;
      add18hours = 1;
    }

    times = mins + promoTimeAdd1hour + promoTimeAdd4hours + promoTimeAdd18hours;
    hotspotTimes = times * 60;

    hours = floor(times / 60);
    days = floor(hours / 24);
    newhours = hours - (days * 24);
    newmins = times - (hours * 60);

    if (firstInsertCoin == 0) {
        firstInsertCoin ++;
        lcd.clear();
    }   
    
    print_txt(" Credit: ", 0, 0);
    lcd.print(coins, 0);
    lcd.print(".00     ");

    print_txt(" ", 0, 1);
    lcd.print(days);
    lcd.print(" day ");
    lcd.print(newhours);
    lcd.print(" hr ");
    lcd.print(newmins);
    lcd.print(" min  ");

    print_txt("Press Button", 4, 3);

    countdownFunc(1);
    cantWaitCounterFunc();

    if (((coins > 0) && buttonPushCounter == 1) || coinsCountdown == 0) {

      lcd.clear();
      print_txt("Sending data", 0, 0);c:\Users\danni\Documents\MyFiles\PISO WIFI\Pisowifi\PisoWifiV5\PisoWifiV5.ino

      Serial.println("Sending data");

      if (!clients.connect(checkIP, 23)) {
        Serial.println("Failed!");
        print_txt("failed!", 8, 0);
        delay(1500);
        
        lcd.clear();
        Serial.println("Router disconnected!");
        print_txt("Router disconnected!", 0, 0);
        clients.stop();
        
        runSetupConnection();
      } 
      
      seed_value = Entropy.random();
      randomSeed(seed_value);
      char C[2] = {letters[random(10)]};
      char O[2] = {letters[random(10)]};
      char D[2] = {letters[random(10)]};
      char E[2] = {letters[random(10)]};
      char S[2] = {letters[random(10)]};
      char T[2] = {letters[random(10)]};

      script = "/ip hotspot user add name=";
      script += C;
      script += O;
      script += D;
      script += E;
      script += S;
      script += T;
      script += " limit-uptime=";
      script += hotspotTimes;
      
      if (promoTimeAdd1hour != 0) {
        script += " profile=hotspot2";
      } else if (promoTimeAdd4hours != 0) {
        script += " profile=hotspot3";
      } else if (promoTimeAdd18hours != 0) {
        script += " profile=hotspot4";
      } else {
        script += " profile=hotspot";
      }

      int script_len = script.length() + 1;
      char mscript[script_len];
      script.toCharArray(mscript, script_len);
      
      tc.sendCommand(mscript);
      Serial.println("Success!  ");
      print_txt(".", 12, 0);
      delay(300);
      print_txt(".", 13, 0);
      delay(300);
      print_txt(".", 14, 0);
      delay(1000);

      lcd.clear();
      print_txt("CODE GENERATED", 3, 0);
      print_txt("< ---------------- >", 0, 1);
      print_txt(C, 7, 2);
      print_txt(O, 8, 2);
      print_txt(D, 9, 2);
      print_txt(E, 10, 2);
      print_txt(S, 11, 2);
      print_txt(T, 12, 2);
      print_txt("Press Button", 4, 3);

      Serial.println("");
      Serial.println("CODE GENERATED");
      Serial.println("PRESS BUTTON ");
      Serial.println("");
      
      codeGeneratedCountdown = 60;
      while (digitalRead(buttonpin) == LOW && (coins > 0)) {

        countdownFunc(2);
        
        if (codeGeneratedCountdown == 0) {
          Serial.println("time stop!");
          coins = 0;
        }
      }
      resetAllValue();
      digitalWrite(ledpin, LOW);
      digitalWrite(disablepin, HIGH);
    }
  }
}

void resetAllValue() {
  Serial.println("resetAllValue!");

  coins = 0;
  gate = 0;
  start = 0;
  currentMillis = 0;
  startMillis = 0;
  promoTimeAdd1hour = 0;
  promoTimeAdd2hours = 0;
  promoTimeAdd4hours = 0;
  promoTimeAdd18hours = 0;
  add1hour = 0;
  add2hours = 0;
  add4hours = 0;
  add18hours = 0;
  timerCounter = 0;
  buttonPushCounter = 0;
  counter = 0;
  firstInsertCoin = 0;
}

void countdownFunc(int y) {

  if(y == 1) {
     counter ++;
    //  Serial.println(counter);

     if(counter == 15) {
        counter = 0;
        coinsCountdown --;
        
        //Serial.println(countdown);
        print_txt("  ", 18, 3);
    } 
    print_txt_int(coinsCountdown, 18, 3); 
  } else {
    counter ++;
    // Serial.println(counter);
    
    if(counter == 300) {
        counter = 0;
        codeGeneratedCountdown --;
        
        //Serial.println(countdown);
        print_txt("  ", 18, 3);
    }
    print_txt_int(codeGeneratedCountdown, 18, 3);
  }
}

void cantWaitCounterFunc() {
  // read the pushbutton input pin:
  buttonState = digitalRead(buttonpin);

  // compare the buttonState to its previous state
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
}

void insertCoin() {
  print_txt("ElsaSilogan PisoWIFI", 0, 0);
  print_txt("    Insert Coin     ", 0, 1);
  print_txt("                    ", 0, 2);
  print_txt("1=12m 10=3hr 20=8hr", 0, 3);
}

void insertCoinBlink() {
  delay(500);
  print_txt("                    ", 0, 1);
  delay(500);
  print_txt("    Insert Coin     ", 0, 1);
  delay(1500);

  // Serial.println("Insert Coin");
}

void print_txt(const char temp[16], int x, int y) { // char to display, x,y location
  lcd.setCursor(x, y);
  lcd.print(temp);
}

void print_txt_int(int temps, int x, int y) { // char to display, x,y location
  lcd.setCursor(x, y);
  lcd.print(temps);
}
