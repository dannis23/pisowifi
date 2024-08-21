#include <SPI.h>
#include <Wire.h>
#include <Entropy.h>
#include <Ethernet.h>
#include <TelnetClient.h>
#include <LiquidCrystal_I2C.h>

// #include <TimeLib.h>

#define randomSeed(s) srandom(s)

String script;
String reboot_script;

uint32_t seed_value;
char letters[10] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
LiquidCrystal_I2C lcd(0x27, 20, 4);

// variable to hold Ethernet shield MAC address
byte clientMAC[] = { 0xAA, 0xBB, 0xCC, 0x00, 0xFE, 0xED };

//put here your router's ip address
IPAddress mikrotikRouterIp (192, 168, 88, 1);

EthernetClient clients;
EthernetClient client;
telnetClient tc(client);

int ConnectTry = 0;

//Counter Timer
int counter, coinsCountdown, codeGeneratedCountdown = 0;

void setup() {
   // start serial port:
  Serial.begin(9600);
  // give the ethernet module time to boot up:
  delay(1000);
  
  lcd.begin();
  lcd.backlight();

  //Initializing
  lcd.clear();
  Serial.println("initializing...");

  Here:

  //Connecting
  lcd.clear();
  Serial.println("Check client");

  ConnectTry = 0;
  checkEthernet:
  
  while (Ethernet.begin(clientMAC) == 0) {
    ConnectTry += 1;
    Serial.println(ConnectTry);
    Serial.println("Failed!");
    
    if (ConnectTry == 3) {
      lcd.clear();
      Serial.println("Rebooting Router!");
      delay(1000);
      goto checkEthernet;
    }
  }

  //Client Connection
  Serial.println("Success!");
  delay(1000);

  //Connecting Router
  Serial.println("Check router");

  tc.setPromptChar('>');
  if (tc.login(mikrotikRouterIp, "admin", "Danila12")) {
    Serial.println();
    Serial.println();
    Serial.println("Success!");
    delay(1000);
  } else {
    Serial.println("Failed!");
    goto Here;
  }

  Serial.println();
  Serial.println();

  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");

  sendCode:

  Serial.println();
  Serial.println();

  Entropy.initialize();
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
  script += 1000;

  // //add profile
  // script += " profile=hotspot2; ";

  // //run script
  // script += "/system script run check_user;";

  int script_len = script.length() + 1;
  char mscript[script_len];
  script.toCharArray(mscript, script_len);
  
  //send command to router
  tc.sendCommand(mscript); 

  Serial.println();
  Serial.println();
  Serial.println();

  Serial.println("15");
  delay(1000);
  Serial.println("14");
  delay(1000);
  Serial.println("13");
  delay(1000);
  Serial.println("12");
  delay(1000);
  Serial.println("11");
  delay(1000);
  Serial.println("10");
  delay(1000);
  Serial.println("9");
  delay(1000);
  Serial.println("8");
  delay(1000);
  Serial.println("7");
  delay(1000);
  Serial.println("6");
  delay(1000);
  Serial.println("5");
  delay(1000);
  Serial.println("4");
  delay(1000);
  Serial.println("3");
  delay(1000);
  Serial.println("2");
  delay(1000);
  Serial.println("1");
  delay(1000);

  goto sendCode;
}
void loop() {}