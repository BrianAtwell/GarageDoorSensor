/*
 * File: CTPLNetworkClient.h
 * Description: Handle network device interactions
 */

#ifndef _CTPLNETWORKCLIENT_H_
#define _CTPLNETWORKCLIENT_H_

#include<Arduino.h>
#include "Timer.h"
#include <WiFiUdp.h>
#include <vector>
#include <list>

class CTPLNetworkLocalUtilities
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

struct CTPLNetworkClientAbstract
{
  //virtual bool connect();
  //virtual bool disconnect();
  //virtual void startDiscovery();
  //virtual void stopDiscovery();
  //virtual void update();
  //virtual bool isConnected();
  //virtual String sendRequest(String jsonStr);
};

class CTPLNetworkCloudClient : CTPLNetworkClientAbstract
{
  bool connect();
  bool disconnect();
  bool isConnected();
  String sendRequest(String jsonStr);
};

class CTPLNetworkLocalClient : CTPLNetworkClientAbstract
{
private:
  

public:
  bool connect();
  bool disconnect();
  void start();
  void update();
  bool isConnected();
  String sendRequest(String jsonStr);
};

class CTPLLocalDiscovery
{
private:
  std::list<CTPLNetworkLocalClient> list;
  static const int udpDiscoveryPort = 9999;
  static const char* udpDiscoveryAddress;
  static const unsigned int BlockSize = 4;
  Timer discoveryTimer;
  WiFiUDP udp;
  
public:
  CTPLLocalDiscovery():discoveryTimer(5000, nullptr){}
  void addClient(CTPLNetworkLocalClient client);
  void update();
  void start();
  void stop();
};

#endif
