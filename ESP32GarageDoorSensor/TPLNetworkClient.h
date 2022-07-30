/*
 * File: CTPLNetworkClient.h
 * Description: Handle network device interactions
 */

#ifndef _TPLNETWORKCLIENT_H_
#define _TPLNETWORKCLIENT_H_

#include<Arduino.h>
#include <ArduinoJson.h>
#include "Timer.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <vector>
#include <list>

#define DEVICE_ID_LENGTH    40

// Class Prototypes
class TPLClientHandler;
class TPLNetworkManager;

class TPLNetworkLocalUtilities
{
private:
  static const unsigned int InitializationVector=171;
public:
  static const unsigned int BlockSize=4;
  static const uint32_t TimeoutSize = 2000;
  static const int Port = 9999;
  static uint32_t littleToBigEndian(uint32_t dataIn);
  static void xorEncrypt(std::vector<uint8_t>& unencrypted, std::vector<uint8_t>& output);
  static void encrypt(std::vector<uint8_t> &inputJSON, std::vector<uint8_t>& pack);
  static void xorDecrypted(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &output);
  static void decrypt(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &unencrypted);

  enum SmartDeviceType {None, SmartPlug,  SmartBulb, SmartDimmer, SmartStrip, SmartLightStrip};
};

namespace TPLNetworkCloudClient
{
  
  String sendRequest(TPLClientHandler* handler, String jsonStr);
};

namespace TPLNetworkLocalClient
{
  bool sendRequest(TPLClientHandler* handler, String& jsonStr, StaticJsonDocument<1024>& doc);
};

class TPLLocalDiscovery
{
private:
  static const int udpDiscoveryPort = 9999;
  static const char* udpDiscoveryAddress;
  Timer discoveryTimer;
  WiFiUDP udp;
  
public:
  TPLLocalDiscovery():discoveryTimer(5000, nullptr){}
  bool update(StaticJsonDocument<1024>& doc, String& ip);
  void start();
  void stop();
};

class TPLClientHandler
{
private:
  String ip;
  const char* deviceID;
  TPLNetworkLocalUtilities::SmartDeviceType type;
  TPLNetworkManager *manager;

protected:
  
  void onPacketReceived(StaticJsonDocument<1024>& doc);

public:
  TPLClientHandler(const char* ldeviceID);
  bool sendRequest(String& jsonStr, StaticJsonDocument<1024>& doc);

  const char* getDeviceID();
  String& getIP();
  TPLNetworkLocalUtilities::SmartDeviceType getType();
  void update();
  
  friend class TPLNetworkManager;
};

class TPLNetworkManager
{
public:
  enum NetworkType {LocalTPLNetwork, CloudTPLNetwork};
 
private:
  //time_t switchTimeout;
  //time_t disconnetTimeout;

  //time_t localDeviceUpdate;
  //time_t cloudDeviceUpdate;

  bool devicesAreFound;

  NetworkType networkType;
  
  TPLLocalDiscovery localDiscovery;
  //TPCloudDiscovery cloudDiscovery;
  
  //TPLNetworkCloudClient cloudClient;

  std::list<TPLClientHandler*> clientList;
public:
  TPLNetworkManager();
  static void toType(StaticJsonDocument<1024> &doc, TPLClientHandler* handler);
  void addClient(TPLClientHandler& client);
  void start();
  void update();
  bool sendRequest(TPLClientHandler* handler, String& jsonStr, StaticJsonDocument<1024>& doc);

  bool allDevicesAvailable() { return devicesAreFound; }

  friend class TPLClientHandler;
  
};

#endif
