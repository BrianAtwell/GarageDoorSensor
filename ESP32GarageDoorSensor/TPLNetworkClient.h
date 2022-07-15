/*
 * File: CTPLNetworkClient.h
 * Description: Handle network device interactions
 */

#ifndef _TPLNETWORKCLIENT_H_
#define _TPLNETWORKCLIENT_H_

#include<Arduino.h>
#include "Timer.h"
#include <WiFiUdp.h>
#include <vector>
#include <list>

class TPLNetworkLocalUtilities
{
private:
  static const unsigned int InitializationVector=171;
public:
  static int littleToBigEndian(int dataIn);
  static void xorEncrypt(std::vector<uint8_t>& unencrypted, std::vector<uint8_t>& output);
  static void encrypt(std::vector<uint8_t> &inputJSON, std::vector<uint8_t>& pack);
  static void xorDecrypted(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &output);
  static void decrypt(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &unencrypted);
};

struct TPLNetworkClientAbstract
{
  virtual String sendRequest(TPLClientHandler* handler, String jsonStr);
};

namespace TPLNetworkCloudClient
{
  
  String sendRequest(TPLClientHandler* handler, String jsonStr)
  {
    
  }
};

namespace TPLNetworkLocalClient
{
  String sendRequest(TPLClientHandler* handler, String jsonStr)
  {
    
  }
};

class TPLLocalDiscovery
{
private:
  std::list<TPLNetworkLocalClient> list;
  static const int udpDiscoveryPort = 9999;
  static const char* udpDiscoveryAddress;
  static const unsigned int BlockSize = 4;
  Timer discoveryTimer;
  WiFiUDP udp;
  
public:
  TPLLocalDiscovery():discoveryTimer(5000, nullptr){}
  void addClient(TPLNetworkLocalClient client);
  void update();
  void start();
  void stop();
};

class TPLClientHandler
{
public:
  String ip;
  String deviceID;
  //localClientTime;

public:
  String sendRequest(String jsonStr);
  void onPacketReceived(StaticJsonDocument<1024> doc;);
  
  
};

class TPLNetworkManager
{
private:
  TPLLocalDiscovery localDiscovery;
  //TPCloudDiscovery cloudDiscovery;

  TPLNetworkLocalClient localClient;
  //TPLNetworkCloudClient cloudClient;

  std::list<TPLClientHandler*> clientList;
  
};

#endif
