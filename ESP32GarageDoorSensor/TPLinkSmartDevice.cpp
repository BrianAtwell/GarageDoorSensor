/*
 * File: TPLinkSmartDevice.cpp
 * Description: Interfaces with TP-Link smart devices over coud API
 */

#include "TPLinkSmartDevice.h"
#include "TPLNetworkClient.h"

TPLinkSmartDevice::TPLinkSmartDevice(const char* ldeviceID): TPLClientHandler(ldeviceID), setRelayState(0), curRelayState(0), setRelayStartTime(0), timeoutSetRelay(5000)
{
  
}

void TPLinkSmartDevice::updateRelayState()
{
  uint8_t setStateTemp=((setRelayState<<0x1)&0x1);
  if((setRelayState&0x1) == 0x1)
  {
    if(setStateTemp == 0x1 && curRelayState == 1)
    {
      setRelayState=0;
    }
    if(setStateTemp == 0 && curRelayState == 0)
    {
      setRelayState=0;
    }

    if(setRelayStartTime!=0)
    {
      time_t diffTime = time(nullptr)-setRelayStartTime;
      if(diffTime >= timeoutSetRelay)
      {
        setRelayState=0;
        setRelayStartTime=0;
      }
    }
  }
}

void TPLinkSmartDevice::onPacketReceived(StaticJsonDocument<1024>& doc)
{
  JsonObject system_get_sysinfo = doc["system"]["get_sysinfo"];
  curRelayState = system_get_sysinfo["relay_state"];

  updateRelayState();
}

bool TPLinkSmartDevice::SetRelay(bool state)
{
  setRelayState=0;
  setRelayState = setRelayState|0x1;
  if(state)
  {
    setRelayState=setRelayState|0x2;
  }

  setRelayStartTime=time(nullptr);

  String jsonStr = String("{\"system\":{\"set_relay_state\":{\"state\": ")+String(state)+String("}}}");
  StaticJsonDocument<1024> doc;

  if( sendRequest(jsonStr, doc))
  {
    JsonObject system_get_sysinfo = doc["system"]["get_sysinfo"];
    curRelayState = system_get_sysinfo["relay_state"];
    updateRelayState();
  }
  
  return false;
}

bool TPLinkSmartDevice::GetRelay()
{
  return curRelayState;
}

bool TPLinkSmartDevice::Update()
{

  if((setRelayState&0x1)==0x1)
  {
    updateRelayState();
  }
  return false;
}

void TPLinkSmartDevice::setRelayTimeout(time_t ltimeout)
{
  timeoutSetRelay=ltimeout;
}

time_t TPLinkSmartDevice::getRelayTimeout()
{
  return timeoutSetRelay;
}

void TPLinkSmartDevice::getAlias(String& oAlias)
{
  oAlias=alias;
}
