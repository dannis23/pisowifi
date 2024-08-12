#include <SPI.h>
#include <Wire.h>
#include <Entropy.h>
#include <Ethernet.h>
#include <TelnetClient.h>
#include <LiquidCrystal_I2C.h>

#include <TimeLib.h>

#define randomSeed(s) srandom(s)

unsigned int mins = 0;
unsigned int hours = 0;
unsigned int days = 0;
unsigned int newmins = 0;
unsigned int newhours = 0;

unsigned int times = 0;
unsigned int promoTime = 0;

int buttonPress = 0;
int ledState = LOW;
int gate = 0;
int interval = 0;
int start = 0;
int admin = 0;
int router_con = 0;

int add2hours = 0;
int add4hours = 0;
int promoTimeAdd2hours = 0;
int promoTimeAdd4hours = 0;

//Counter Timer
int counter = 0;
int timerCounter = 0;

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
LiquidCrystal_I2C lcd(0x27, 20, 4);
EthernetClient client;
telnetClient tc(client);

// variable to hold Ethernet shield MAC address
byte clientMAC[] = { 0xAA, 0xBB, 0xCC, 0x00, 0xFE, 0xED };

//put here your router's ip address
IPAddress ip(192, 168, 88, 2);
IPAddress mikrotikRouterIp (192, 168, 88, 1);

// fill in your Domain Name Server address here:
IPAddress myDns(1,1,1,1);

void runSetupConnection() {
  pinMode(buttonpin, INPUT);
  pinMode(ledpin, OUTPUT);
  pinMode(disablepin, OUTPUT);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  print_txt("initializing    ", 0, 0);
  Serial.println("initializing");
  lcd.clear();
  print_txt("initializing.   ", 0, 0);
  Serial.println("initializing.");
  delay(500);
  lcd.clear();
  print_txt("initializing..  ", 0, 0);
  Serial.println("initializing..");
  delay(500);
  lcd.clear();
  print_txt("initializing... ", 0, 0);
  Serial.println("initializing...");
  delay(500);
  lcd.clear();
  print_txt("initializing....", 0, 0);
  Serial.println("initializing....");
  delay(1000);
  digitalWrite(disablepin, HIGH);
  
  // start serial port:
  Serial.begin(9600);
  // give the ethernet module time to boot up:
  delay(1000); 

  // get ethernet shield IP via DHCP
  // [part of Ethernet Library]
  while (Ethernet.begin(clientMAC) == 0) {
   lcd.clear();
    print_txt("Connection Failed!", 0, 0);
    Serial.println("Failed to configure Ethernet using DHCP"); // display error
    delay(1000); // retry after 1 sec
  }
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("connected!");
  Serial.println("connected!");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("please wait....");
  Serial.println("please wait....");
  delay(1000);
  
  
  //start the Ethernet connection using a fixed IP address and DNS server:
 Ethernet.begin(clientMAC, ip, myDns);
  //print the Ethernet board/shield's IP address:
 Serial.print("My IP address: ");
 Serial.println(Ethernet.localIP());
 
 tc.setPromptChar('>');
 if (tc.login(mikrotikRouterIp, "admin", "Danila12")) {
  Serial.println();
  Serial.println("Admin Login");
  Serial.println();
  lcd.clear();
 } else {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("MR-IP Address error");
  delay(5000);
  runSetupConnection();
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

  cantWaitCounterFunc();
  
   currentMillis = millis();
   if (coins < 1 && gate == 0 && start == 0) {
    
    if (client.connected()) {

      char c = client.read();
      Serial.print(c);
      Serial.println("client connected!!!.");
      delay(1000);

      print_txt("ElsaSilogan PisoWIFI", 0, 0);
      print_txt("    Insert Coin     ", 0, 1);
      print_txt("                    ", 0, 2);
      print_txt("1=12m 20=6hr 30=10hr", 0, 3);

      Serial.println();
      Serial.println("ElsaSilogan PisoWIFI");
      Serial.println();

      insertCoinBlink();
    } else {
      Serial.println("client not connected!!!.");
      delay(1000);
      client.stop();
      runSetupConnection();
    }
  }
  
  if ((start == 0) && (coins > 0) && (currentMillis - startMillis >= 100)) {
    mins = coins * 12;   
  
      if((add2hours == 0) && coins >= 20 && coins < 30){
        Serial.println("coins 20 to 29");
        promoTimeAdd2hours = 120;
        add2hours = 1;
      }
  
      if((add4hours == 0) && coins >= 30){
        Serial.println("coins 30 above");
        promoTimeAdd2hours = 0;
        promoTimeAdd4hours = 240;
        add4hours = 1;
      }
  
      times = mins + promoTimeAdd2hours + promoTimeAdd4hours;
      
      hours = floor(times / 60);
      days = floor(hours / 24);
      newhours = hours - (days * 24);
      newmins = times - (hours * 60);
      lcd.setCursor(0, 0);
      lcd.print(" Credit: ");
      lcd.print(coins, 0);
      lcd.print(".00     ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.print(days);
      lcd.print(" day ");
      lcd.print(newhours);
      lcd.print(" hr ");
      lcd.print(newmins);
      lcd.print(" min  ");
      
      lcd.setCursor(0, 2);
      lcd.print("    Press Button    ");
      lcd.setCursor(0, 3);
      lcd.print("  To Generate Code  ");
      
      currentMillis = millis(); //get the current time
      
    if ((coins > 0) && buttonPushCounter == 1) {
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
        script += times * 60;
        script += " profile=hotspot";
        
        int script_len = script.length() + 1;
        char mscript[script_len];
        script.toCharArray(mscript, script_len);
        
        lcd.setCursor(0, 0);
        lcd.print("sending data       ");
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        lcd.print("                    ");
  
        Serial.println("");
        Serial.println("sending data.");
        Serial.println("");
  
        tc.sendCommand(mscript);
        lcd.setCursor(0, 0);
        lcd.print("sending data.");
        delay(300);
        lcd.clear();
        lcd.print("sending data..");
        lcd.setCursor(0, 0);
        lcd.print("   CODE GENERATED   ");
        lcd.setCursor(0, 1);
        lcd.print("< ---------------- >");
        lcd.setCursor(0, 2);
        lcd.print("       ");
        lcd.print(C);
        lcd.print(O);
        lcd.print(D);
        lcd.print(E);
        lcd.print(S);
        lcd.print(T);
        lcd.print("       ");
        lcd.setCursor(0, 3);
        delay(2000);
        lcd.print("    PRESS BUTTON    ");
  
        Serial.println("");
        Serial.println("CODE GENERATED");
        Serial.println("PRESS BUTTON ");
        Serial.println("");
  
        while (digitalRead(buttonpin) == LOW && (coins > 0)) {
          counter++;
          Serial.println(counter);
          
          if (counter == 5000) {
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
  promoTimeAdd2hours = 0;
  promoTimeAdd4hours = 0;
  add2hours = 0;
  add4hours = 0;
  timerCounter = 0;
  buttonPushCounter = 0;
  counter = 0;
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

void insertCoinBlink() { 
  delay(500);
  print_txt("                    ", 0, 1);
  delay(500);
  print_txt("    Insert Coin     ", 0, 1);
  delay(1500);
}

void print_txt(const char temp[16], int x, int y){ // char to display, x,y location
  lcd.setCursor(x,y);
  lcd.print(temp);
}
