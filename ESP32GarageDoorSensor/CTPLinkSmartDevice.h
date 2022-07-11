/*
 * File: CTPLinkSmartDevice.h
 * Description: Interfaces with TP-Link smart devices over coud API
 */

#ifndef _CTPLINKSMARTDEVICE_H_
#define _CTPLINKSMARTDEVICE_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "CTPLinkJsonManager.h"


class CTPLinkSmartDevice
{
private:
  int errorCode;
  int updateTime;
  String alias;

public:
  bool SetRelay(bool state);
  void GetRelay(bool &state);
  int GetError();
  int UpdateTime();
  bool Update();
  
};

#endif
