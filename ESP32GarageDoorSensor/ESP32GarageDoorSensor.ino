#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <strings_en.h>
#include <WiFiManager.h>
#include "TPLinkSmartDevice.h"
#include "TPLNetworkClient.h"
#include "Utilities.h"

#include <vector>

WiFiManager wifiManager;
WiFiMulti WiFiMulti;


//TPLNetworkLocalClient localClient;
//TPLLocalDiscovery localDiscovery;
TPLNetworkManager netManager;
//TPLClientHandler client1("8006E12261657034F224406C89EA50301CFB2A92");
TPLinkSmartDevice smartDevice("8006E12261657034F224406C89EA50301CFB2A92");

const int CONFIG_PANEL_PIN=35;

// Not sure if WiFiClientSecure checks the validity date of the certificate. 
// Setting clock just to be sure...
void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

void pollConfigPanelSwitch()
{
  static bool configPinState=0;
  configPinState=digitalRead(CONFIG_PANEL_PIN);

  bool configError=0;
  
  if(configError == 1)
  {
    Serial.println("configError");
  }
  if(configPinState == HIGH)
  {
    Serial.println("configPinState == LOW");
  }
  
  if(configPinState == HIGH || configError)
  {
    //pinMode(LED1_PIN, OUTPUT);
    //pinMode(LED2_PIN, OUTPUT);
    //pinMode(LED3_PIN, OUTPUT);
    
    Serial.println("Starting Config Portal...");
    wifiManager.setConfigPortalBlocking(false);
    wifiManager.startConfigPortal("RemoteSwitchConfig", "password");
    
    while(!wifiManager.process())
    {
      // Update EEPROM
      /*
      if (shouldSaveConfig) {
        SaveEEPROM();
      }
      */
    }

    Serial.println("Ended Config Portal");
  
    ESP.restart();
  }
  else
  {
    Serial.println("autoConnecting...");
    if (!wifiManager.autoConnect("RemoteSwitchConfig", "password")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(CONFIG_PANEL_PIN, INPUT);

  WiFi.mode(WIFI_STA);

  pollConfigPanelSwitch();
  smartDevice.SetRelay(true);

  setClock();


  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  netManager.addClient(smartDevice);

  netManager.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  netManager.update();
}
