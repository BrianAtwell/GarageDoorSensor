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
  static const int udpDiscoveryPort = 9999;
  static const char* udpDiscoveryAddress;
  static const unsigned int InitializationVector=171;
  static const unsigned int BlockSize = 4;
  Timer discoveryTimer;
  WiFiUDP udp;

public:
  CTPLNetworkLocalClient():discoveryTimer(5000, nullptr){}
  bool connect();
  bool disconnect();
  void start();
  int littleToBigEndian(int dataIn);
  void xorEncrypt(std::vector<uint8_t>& unencrypted, std::vector<uint8_t>& output);
  void encrypt(std::vector<uint8_t> &inputJSON, std::vector<uint8_t>& pack);
  void xorDecrypted(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &output);
  void decrypt(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &unencrypted);
  void startDiscovery();
  void stopDiscovery();
  void updateDiscovery();
  void update();
  bool isConnected();
  String sendRequest(String jsonStr);
};

#endif
