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
  static int littleToBigEndian(int dataIn);
  static void xorEncrypt(std::vector<uint8_t>& unencrypted, std::vector<uint8_t>& output);
  static void encrypt(std::vector<uint8_t> &inputJSON, std::vector<uint8_t>& pack);
  static void xorDecrypted(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &output);
  static void decrypt(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &unencrypted);

  enum SmartDeviceType { SmartPlug,  SmartBulb, SmartDimmer, SmartStrip, SmartLightStrip};
};

namespace TPLNetworkCloudClient
{
  
  String sendRequest(TPLClientHandler* handler, String jsonStr);
};

namespace TPLNetworkLocalClient
{
  std::vector<uint8_t> sendRequest(TPLClientHandler* handler, String jsonStr);
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
public:
  String ip;
  const char* deviceID;
  //localClientTime;
  bool relayState;
  TPLNetworkLocalUtilities::SmartDeviceType type;

public:
  TPLClientHandler(const char* ldeviceID);
  String sendRequest(String jsonStr);
  void onPacketReceived(StaticJsonDocument<1024>& doc);
  
  friend class TPLNetworkManager;
};

class TPLNetworkManager
{
private:
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
  
};

#endif
