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
  uint8_t setStateTemp=((setRelayState>>0x1)&0x1);
  if((setRelayState&0x1) == 0x1)
  {
    Serial.print("SmartDevice Relay State: ");
    Serial.println(curRelayState);
    if(setStateTemp == 0x1 && curRelayState == 1)
    {
      setRelayState=0;
      setRelayStartTime=0;
    }
    if(setStateTemp == 0 && curRelayState == 0)
    {
      setRelayState=0;
      setRelayStartTime=0;
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

bool TPLinkSmartDevice::setRelay(bool state)
{
  Serial.print("setRelay function(");
  Serial.print(state);
  Serial.println(")");
  
  setRelayState=0;
  setRelayState = setRelayState|0x1;
  if(state)
  {
    setRelayState=setRelayState|0x2;
  }

  setRelayStartTime=time(nullptr);

  String ip=getIP();

  if(ip.length()>0)
  {
    SendUpdateRequest();
  }
  else
  {
    Serial.println("SmartDevice: IP not valid.");
  }
  
  return false;
}

bool TPLinkSmartDevice::getRelay()
{
  return curRelayState;
}

bool TPLinkSmartDevice::SendUpdateRequest()
{
  bool state = (setRelayState>>0x1)&0x1;
  int error_code=1;
  String jsonStr = String("{\"system\": {\"set_relay_state\": {\"state\": ")+String(state)+String("}}}");

  { // Narrow Scope
    StaticJsonDocument<1024> doc;
  
    if( sendRequest(jsonStr, doc))
    {
      Serial.println("Update SmartDevice Relay State");
      JsonObject system_get_sysinfo = doc["system"]["set_relay_state"];
      error_code = system_get_sysinfo["err_code"];
    }
  }
  if(error_code == 0)
  {
    StaticJsonDocument<1024> doc;
    
    jsonStr = "{\"system\": {\"get_sysinfo\": null}}";
    if(sendRequest(jsonStr, doc))
    {
      JsonObject system_get_sysinfo = doc["system"]["get_sysinfo"];
      curRelayState = system_get_sysinfo["relay_state"];
      updateRelayState();
      return true;
      
    }
    else
    {
      error_code=1;
    }
  }
  if(error_code != 0)
  {
    Serial.println("Error Setting Relay State.");
  }

  return false;
}

bool TPLinkSmartDevice::update()
{

  String ip=getIP();
  
  if(ip.length()>0)
  {
    if((setRelayState&0x1)==0x1)
    {
      SendUpdateRequest();
    }
  }
  else
  {
    Serial.println("SmartDevice: No valid IP");
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
