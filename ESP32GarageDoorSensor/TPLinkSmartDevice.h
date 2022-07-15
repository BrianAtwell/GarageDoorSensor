/*
 * File: TPLinkSmartDevice.h
 * Description: Interfaces with TP-Link smart devices over coud API
 */

#ifndef _TPLinkSmartDevice_H_
#define _TPLinkSmartDevice_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "TPLinkJsonManager.h"


class TPLinkSmartDevice
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
