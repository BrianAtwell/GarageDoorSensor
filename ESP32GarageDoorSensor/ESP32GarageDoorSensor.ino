#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <strings_en.h>
#include <WiFiManager.h>
#include "TPLinkSmartDevice.h"
#include "TPLNetworkClient.h"
#include "Utilities.h"


/*
 * Description: This example Program will toggle TP Link KASA smart plug
 *  when you touch Touch0 sensor. Provide your own device ID. The device 
 *  ID can be aquired through python-kasa.
 */

#include <vector>

WiFiManager wifiManager;
WiFiMulti WiFiMulti;

int touch0threshold = 40;
bool touch0Pressed = false;
uint32_t touchActiveTime=0;
uint32_t touchInactiveTime=0;
uint32_t touchPrevTime=0;
const uint32_t touchActiveMaxTime=700;
const uint32_t touchInactiveMaxTime=1000;


//TPLNetworkLocalClient localClient;
//TPLLocalDiscovery localDiscovery;
TPLNetworkManager netManager;
//TPLClientHandler client1("8006E12261657034F224406C89EA50301CFB2A92");
TPLinkSmartDevice smartDevice("80062AFF83D4BC31B96FEE14AC2FB98C1D90E808");

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
  smartDevice.setRelay(true);

  setClock();


  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  netManager.addClient(smartDevice);

  netManager.start();

  touchActiveTime=0;
  touchInactiveTime=0;
  touchPrevTime=millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  netManager.update();

  if(touchRead(T0) > touch0threshold){
    if(touch0Pressed == false)
    {
      touchPrevTime=millis();
      touchInactiveTime=0;
      
      touch0Pressed=true;
    }
    else
    {
      touchInactiveTime+=millis()-touchPrevTime;
      touchPrevTime=millis();
    }
  }
  else
  {
    if(touch0Pressed == true)
    {
      touchPrevTime=millis();
      touchActiveTime=0;
      
      touch0Pressed=false;
    }
    else
    {
      touchActiveTime+=millis()-touchPrevTime;
      touchPrevTime=millis();
      if(touchActiveTime > touchActiveMaxTime && touchInactiveTime > touchInactiveMaxTime)
      {
        Serial.println("Touch Activated");
        if(smartDevice.getRelay())
        {
          smartDevice.setRelay(false);
        }
        else
        {
          smartDevice.setRelay(true);
        }
        touchInactiveTime=0;
        touchActiveTime=0;
      }
    }
  }
}
