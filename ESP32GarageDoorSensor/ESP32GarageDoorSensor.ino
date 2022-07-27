#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <strings_en.h>
#include <WiFiManager.h>
#include "TPLinkSmartDevice.h"
#include "TPLNetworkClient.h"
#include "Utilities.h"
#include "AverageHandler.h"

# define ARRAY_SIZE(type) sizeof(type)/sizeof(type[0])


/*
 * Description: This example Program will toggle TP Link KASA smart plug
 *  when you touch Touch0 sensor. Provide your own device ID. The device 
 *  ID can be aquired through python-kasa.
 *  
 *  Changed to IR Photo diode and IR LED. This will be place around Garage Door at bottom of the garage door. 
 *  It will detect if the door is open or closed. When open the analog in will read towards 0 direction where 
 *  0 is the most saturated IR signal. When the garage door is close the path of the signal will be broken and
 *  it will read a value between 1500 and up.
 */

#include <vector>

WiFiManager wifiManager;
WiFiMulti WiFiMulti;

/*
int touch0threshold = 40;
bool touch0Pressed = false;
uint32_t touchActiveTime=0;
uint32_t touchInactiveTime=0;
uint32_t touchPrevTime=0;
const uint32_t touchActiveMaxTime=700;
const uint32_t touchInactiveMaxTime=1000;
int touch0Sample=50;
AverageHandler<100, int> touch0Avg;
int curTouch0Avg=0;
*/

// IR LED: 3.3V to 470 ohm resistor, 470 ohm resistor to Anode, Cathode to Ground.

// Photo diode: 3.3V to 10K ohm resistor, 10k ohm resistor to Cathode of Photo diode,
//              Cathode is also connected Arduino anlog Input GPIO36/AD0, Anode is connected to Ground.

// 0 Value when IR is signal is directly connected
// 1500-2000 Value when IR signal is blocked with thick paper/Door Closed.

int analogIRMinThreshold = 1000;
bool analogIRPressed = false;
AverageHandler<100, int> analogIRAvg;
int curAnalogIRAvg=0;
int analogIRSample=50;

const int analogIRInPin = 36;
const int errorLEDPin=17;
const int connectedLEDPin=16;
bool connectedLEDState=false;

Timer blinkLEDConnectedTimer(250, nullptr);
//Timer touchTimer(10, nullptr);
Timer analogIRTimer(10, nullptr);
//Timer firstLightOnTimer(8000, nullptr);
//bool firstLightTurnedOn=false;



//TPLNetworkLocalClient localClient;
//TPLLocalDiscovery localDiscovery;
TPLNetworkManager netManager;
//TPLClientHandler client1("8006E12261657034F224406C89EA50301CFB2A92");
TPLinkSmartDevice smartDevices[] = {TPLinkSmartDevice("80062AFF83D4BC31B96FEE14AC2FB98C1D90E808"),TPLinkSmartDevice("800686F229F4148C8D9F0DEBEBE656531D90744B")};

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
  pinMode(analogIRInPin, INPUT);
  pinMode(errorLEDPin, OUTPUT);
  pinMode(connectedLEDPin,OUTPUT);

  digitalWrite(errorLEDPin,LOW);
  digitalWrite(connectedLEDPin,LOW);

  WiFi.mode(WIFI_STA);

  pollConfigPanelSwitch();

  setClock();


  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  for(int i=0; i<ARRAY_SIZE(smartDevices); i++)
  {
    netManager.addClient(smartDevices[i]);
  }

  netManager.start();

/*
  touchActiveTime=0;
  touchInactiveTime=0;
  touchPrevTime=millis();
  */

  analogIRPressed=false;
  curAnalogIRAvg=1000;

  // Calculate initial Average
  for(int i=0; i<100; i++)
  {
    analogIRSample=analogRead(analogIRInPin);
    analogIRAvg.addSample(analogIRSample);
    curAnalogIRAvg=analogIRAvg.calculateAverage();
  }

  // Initialize so we don't change the relay on startup
  // only change when we detect a new change
  if(curAnalogIRAvg < analogIRMinThreshold){
    analogIRPressed=true;
  }
  else
  {
    analogIRPressed=false;
  }

  Serial.println("Entering Loop");
}

void loop() {
  // put your main code here, to run repeatedly:
  netManager.update();

  /*
  if(touchTimer.Update())
  {
    // Only update every 10 ms
    touch0Sample=touchRead(T0);
    touch0Avg.addSample(touch0Sample);
    curTouch0Avg=touch0Avg.calculateAverage();
  }
  */

  /*
  if(firstLightTurnedOn)
  {
    if(firstLightOnTimer.Update())
    {
      if(smartDevices[0].getRelay() == true && smartDevices[1].getRelay() == false)
      {
        smartDevices[1].setRelay(true);
        firstLightTurnedOn=false;
      }
    }
  }
  */

  if(blinkLEDConnectedTimer.Update())
  {
    if(netManager.allDevicesAvailable())
    {
      connectedLEDState=true;
      digitalWrite(connectedLEDPin, HIGH);
    }
    else
    {
      if(connectedLEDState == true)
      {
        digitalWrite(connectedLEDPin, LOW);
        connectedLEDState=false;
      }
      else
      {
        digitalWrite(connectedLEDPin, HIGH);
        connectedLEDState=true;
      }
    }
  }

  if(analogIRTimer.Update())
  {
    analogIRSample=analogRead(analogIRInPin);
    analogIRAvg.addSample(analogIRSample);
    curAnalogIRAvg=analogIRAvg.calculateAverage();
  }

  if(curAnalogIRAvg < analogIRMinThreshold){
    if(analogIRPressed == false)
    {
      // Execute once per peak and valley
      bool noErrors=false;

      Serial.println("Door Opened");
      for(int i=0; i<ARRAY_SIZE(smartDevices); i++)
      {
        noErrors=noErrors|!smartDevices[i].setRelay(true);
      }

      if(noErrors==true)
      {
        digitalWrite(errorLEDPin, HIGH);
      }
      else
      {
        digitalWrite(errorLEDPin, LOW);
      }

      /*
      if(smartDevices[1].getRelay() == false)
      {
        firstLightTurnedOn=true;
        firstLightOnTimer.Reset();
      }
      */
      
      analogIRPressed=true;
    }
  }
  else
  {
    if(analogIRPressed == true)
    {

      analogIRPressed=false;
    }
  }

  /*
  if(curTouch0Avg > touch0threshold){
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
  */
}
