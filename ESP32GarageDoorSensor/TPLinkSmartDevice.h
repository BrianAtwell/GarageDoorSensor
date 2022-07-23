/*
 * File: TPLinkSmartDevice.h
 * Description: Interfaces with TP-Link smart devices over coud API
 */

#ifndef _TPLinkSmartDevice_H_
#define _TPLinkSmartDevice_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "TPLinkJsonManager.h"
#include "TPLNetworkClient.h"


class TPLinkSmartDevice : public TPLClientHandler
{
private:
  String alias;
  uint8_t setRelayState;
  uint8_t curRelayState;
  time_t setRelayStartTime;
  time_t timeoutSetRelay;

protected:
  void onPacketReceived(StaticJsonDocument<1024>& doc);
  bool SendUpdateRequest();

public:
  TPLinkSmartDevice(const char* ldeviceID);
  void updateRelayState();
  bool setRelay(bool state);
  bool getRelay();
  bool update();
  void setRelayTimeout(time_t ltimeout);
  time_t getRelayTimeout();
  void getAlias(String& oAlias);
  
};

#endif
