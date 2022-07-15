/*
 * File: TPLinkJsonManager.h
 * Description: Handle JSON conversions
 */

#ifndef _TPLINKJSONMANAGER_H_
#define _TPLINKJSONMANAGER_H_

#include <Arduino.h>
#include <ArduinoJson.h>


class TPLinkJsonManager
{
public:

static void SetRelayToJSON(String &output, bool state, String deviceID)
{
  StaticJsonDocument<192> doc;

  doc["method"] = "passthrough";
  
  JsonObject params = doc.createNestedObject("params");
  params["deviceId"] = deviceID;
  params["requestData"]["system"]["set_relay_state"]["state"] = (state == 1 ? true : false);

  serializeJson(doc, output);
}

static void GetSystemInfoToJSON(String &output, String deviceID)
{
  StaticJsonDocument<192> doc;

  doc["method"] = "passthrough";
  
  JsonObject params = doc.createNestedObject("params");
  params["deviceId"] = deviceID;
  params["requestData"]["system"]["get_sysinfo"] = nullptr;
  
  serializeJson(doc, output);
}

static bool DocumentGetRelayState(StaticJsonDocument<1024>& doc, bool& state)
{
  JsonObject system_sysinfo = doc["result"]["responseData"]["system"]["get_sysinfo"];
  int relay_state = system_sysinfo["relay_state"];
  if(relay_state == 1)
  {
    state=true;
  }
  else
  {
    state=false;
  }

  return true;
}

static bool JSONToDocument(String jsonStr, StaticJsonDocument<1024>& doc)
{
  //StaticJsonDocument<1024> doc;

  DeserializationError error = deserializeJson(doc, jsonStr);
  
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return false;
  }
  
  int error_code = doc["error_code"]; // 0

  if(error_code != 0)
  {
    Serial.print("JSONToDocument() JSON error code failed: ");
    Serial.println(error_code);
    return false;
  }

  return true;

  /*
  JsonObject result_responseData_system_get_sysinfo = doc["result"]["responseData"]["system"]["get_sysinfo"];
  const char* result_responseData_system_get_sysinfo_dev_name = result_responseData_system_get_sysinfo["dev_name"];
  const char* result_responseData_system_get_sysinfo_hw_ver = result_responseData_system_get_sysinfo["hw_ver"];
  int result_responseData_system_get_sysinfo_rssi = result_responseData_system_get_sysinfo["rssi"]; // -44
  long result_responseData_system_get_sysinfo_latitude_i = result_responseData_system_get_sysinfo["latitude_i"];
  int result_responseData_system_get_sysinfo_err_code = result_responseData_system_get_sysinfo["err_code"];
  const char* result_responseData_system_get_sysinfo_deviceId = result_responseData_system_get_sysinfo["deviceId"];
  long result_responseData_system_get_sysinfo_longitude_i = result_responseData_system_get_sysinfo["longitude_i"];
  const char* result_responseData_system_get_sysinfo_mac = result_responseData_system_get_sysinfo["mac"];
  const char* result_responseData_system_get_sysinfo_active_mode = result_responseData_system_get_sysinfo["active_mode"];
  const char* result_responseData_system_get_sysinfo_icon_hash = result_responseData_system_get_sysinfo["icon_hash"];
  int result_responseData_system_get_sysinfo_updating = result_responseData_system_get_sysinfo["updating"];
  int result_responseData_system_get_sysinfo_led_off = result_responseData_system_get_sysinfo["led_off"];
  const char* result_responseData_system_get_sysinfo_feature = result_responseData_system_get_sysinfo["feature"];
  long result_responseData_system_get_sysinfo_on_time = result_responseData_system_get_sysinfo["on_time"];
  int result_responseData_system_get_sysinfo_relay_state = result_responseData_system_get_sysinfo["relay_state"];
  
  int result_responseData_system_get_sysinfo_next_action_type = result_responseData_system_get_sysinfo["next_action"]["type"];
  
  const char* result_responseData_system_get_sysinfo_oemId = result_responseData_system_get_sysinfo["oemId"];
  const char* result_responseData_system_get_sysinfo_alias = result_responseData_system_get_sysinfo["alias"];
  const char* result_responseData_system_get_sysinfo_model = result_responseData_system_get_sysinfo["model"];
  const char* result_responseData_system_get_sysinfo_hwId = result_responseData_system_get_sysinfo["hwId"];
  const char* result_responseData_system_get_sysinfo_mic_type = result_responseData_system_get_sysinfo["mic_type"];
  const char* result_responseData_system_get_sysinfo_sw_ver = result_responseData_system_get_sysinfo["sw_ver"];
  const char* result_responseData_system_get_sysinfo_status = result_responseData_system_get_sysinfo["status"];
  */
}

};

#endif
