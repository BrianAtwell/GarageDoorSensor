
/*
 * File: CTPLNetworkClient.cpp
 * Description: Handle network device interactions
 */
#include "TPLNetworkClient.h"
#include <ArduinoJson.h>
#include <WiFi.h>
#include "Timer.h"

#include <vector>
#include <list>
#include <cstring>
#include "Utilities.h"

int TPLNetworkLocalUtilities::littleToBigEndian(int dataIn)
{
  int bigEndian;
  uint8_t* bigEndianPtr=((uint8_t*)&bigEndian);
  uint8_t* dataInPtr = ((uint8_t*)&dataIn);
  bigEndianPtr[0]=dataInPtr[3];
  bigEndianPtr[1]=dataInPtr[2];
  bigEndianPtr[2]=dataInPtr[1];
  bigEndianPtr[3]=dataInPtr[0];

  //Serial.print("length: ");
  //printHexString(bigEndian);

  return bigEndian;
}

void TPLNetworkLocalUtilities::xorEncrypt(std::vector<uint8_t>& unencrypted, std::vector<uint8_t>& output)
{
  int len = unencrypted.size();
  uint8_t key = TPLNetworkLocalUtilities::InitializationVector;
  for(int i=0; i<len; i++)
  {
      key = key ^ unencrypted[i];
      output.push_back(key);
  }
}

void TPLNetworkLocalUtilities::encrypt(std::vector<uint8_t> &inputJSON, std::vector<uint8_t>& pack)
{
  uint32_t len = TPLNetworkLocalUtilities::littleToBigEndian(inputJSON.size());
  TPLNetworkLocalUtilities::xorEncrypt(inputJSON, pack);
  pack.insert(pack.begin()++, (uint8_t*)&len, ((uint8_t*)&len)+4);
}


void TPLNetworkLocalUtilities::xorDecrypted(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &output)
{
  int len = ciphertext.size();
  uint8_t key = TPLNetworkLocalUtilities::InitializationVector;
  uint8_t plainbyte=0;
  for(int i=0; i<len; i++)
  {
      plainbyte = key ^ ciphertext[i];
      key = ciphertext[i];
      output.push_back(plainbyte);
  }
}

void TPLNetworkLocalUtilities::decrypt(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &unencrypted)
{
  TPLNetworkLocalUtilities::xorDecrypted(ciphertext, unencrypted);
}

namespace TPLNetworkCloudClient
{
  
  String sendRequest(TPLClientHandler* handler, String jsonStr)
  {
    
  }
};

namespace TPLNetworkLocalClient
{
  std::vector<uint8_t> sendRequest(TPLClientHandler* handler, String jsonStr)
  {
    const char *dataStr = jsonStr.c_str();
    std::vector<uint8_t> discoveryPacket;
    std::vector<uint8_t> packedPacket;
    std::vector<uint8_t> decryptedPacket;
    uint32_t readPacketLength=0;
    uint32_t bytesRead=0;
    uint32_t bytesToRead=0;
    const uint16_t BUFFER_SIZE = 50;
    char buffer[BUFFER_SIZE];
  
    AddUint8ToVector((uint8_t*)dataStr, sizeof(dataStr)-1, discoveryPacket);
  
    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    if (!client.connect(handler->ip.c_str(), TPLNetworkLocalUtilities::Port)) {
        Serial.println("connection failed");
        return decryptedPacket;
    }
    
    // Encrypt message
    TPLNetworkLocalUtilities::encrypt(discoveryPacket, packedPacket);
  
    // Send TCP Packet
    client.write(packedPacket.data(), packedPacket.size());
  
    // Read Block Size which holds the length
    client.read(((uint8_t*)&readPacketLength), TPLNetworkLocalUtilities::BlockSize);
    
    //client.read(((uint8_t*)&readPacketLength), readPacketLength);
    
    while(client.available() > 0 && readPacketLength > bytesRead)
    {
      unsigned long timeout = millis();
      while (client.available() == 0) {
          if (millis() - timeout > TPLNetworkLocalUtilities::TimeoutSize) {
              Serial.println(">>> Client Timeout !");
              client.stop();
              return decryptedPacket;
          }
      }
      
      bytesToRead = readPacketLength-bytesRead;
      if(bytesToRead > BUFFER_SIZE)
      {
        bytesToRead = BUFFER_SIZE;
      }
      
      client.read((uint8_t *)buffer, BUFFER_SIZE);
  
      bytesRead+=bytesToRead;
      
      packedPacket.insert(packedPacket.end(),buffer,buffer+bytesToRead);
      //printHexString(packedPacket);
    }
  
    // Decrypt
    TPLNetworkLocalUtilities::decrypt(packedPacket, decryptedPacket);
    printByteString(decryptedPacket);

    return decryptedPacket;
  }
};

const char* TPLLocalDiscovery::udpDiscoveryAddress = "255.255.255.255";

void TPLLocalDiscovery::start()
{
  udp.begin(WiFi.localIP(),udpDiscoveryPort);
}

void TPLLocalDiscovery::stop()
{
  
}

bool TPLLocalDiscovery::update(StaticJsonDocument<1024>& doc, String& ip)
{
  std::vector<uint8_t> packedPacket;
  std::vector<uint8_t> decryptedPacket;
  const uint16_t BUFFER_SIZE = 50;
  char buffer[51];
  int readBytes=0;
  
  //Send a packet every x seconds
  if(discoveryTimer.Update())
  {
    std::vector<uint8_t> discoveryPacket;
    char dataStr[] = "{\"system\": {\"get_sysinfo\": null}}";
    AddUint8ToVector((uint8_t*)dataStr, sizeof(dataStr)-1, discoveryPacket);
    //Serial.println("Discovery Packet Raw:");
    //printHexString(discoveryPacket);
    //Serial.println("Sending Discovery Packet");
    
    // Encrypt message
    TPLNetworkLocalUtilities::encrypt(discoveryPacket, packedPacket);
    
    //Serial.println("Discovery Packet Encrypted: ");

    // Trim beginning length bytes
    packedPacket.erase (packedPacket.begin(), packedPacket.begin()+4);
    
    //printHexString(packedPacket);

    for(int i=0; i<1; i++)
    {
      udp.beginPacket(udpDiscoveryAddress, udpDiscoveryPort);
      udp.write(packedPacket.data(), packedPacket.size());
      udp.endPacket();
    }
  }

  udp.parsePacket();
  //receive response from server, it will be HELLO WORLD
  if((readBytes = udp.read(buffer, BUFFER_SIZE)) > 0){
    int count=1;
    //Serial.print("Packet available from remote IP: ");
    //Serial.println(udp.remoteIP());
    packedPacket.clear();
    packedPacket.insert(packedPacket.end(),buffer,buffer+readBytes);
    count++;
    
    while((readBytes = udp.read(buffer, BUFFER_SIZE)) > 0)
    {
      packedPacket.insert(packedPacket.end(),buffer,buffer+readBytes);
      //printHexString(packedPacket);
    }

    //printHexString(packedPacket);
    
    // Decrypt message
    TPLNetworkLocalUtilities::decrypt(packedPacket, decryptedPacket);
    //printByteString(decryptedPacket);

    DeserializationError error = deserializeJson(doc, decryptedPacket.data());

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return false;
    }

    JsonObject system_get_sysinfo = doc["system"]["get_sysinfo"];
    ip = udp.remoteIP().toString();

    return true;
  }
  return false;
}

TPLClientHandler::TPLClientHandler(const char* ldeviceID):deviceID(ldeviceID), ip(String("")), relayState(0)
{
}

String TPLClientHandler::sendRequest(String jsonStr)
{
  return String("");
}
  
void TPLClientHandler::onPacketReceived(StaticJsonDocument<1024>& doc)
{
  
}

TPLNetworkManager::TPLNetworkManager()
{
  
}

void TPLNetworkManager::start()
{
  localDiscovery.start();
}

void TPLNetworkManager::addClient(TPLClientHandler& client)
{
  clientList.push_back(&client);
}

void TPLNetworkManager::toType(StaticJsonDocument<1024> &doc, TPLClientHandler* handler)
{
  JsonObject system_get_sysinfo = doc["system"]["get_sysinfo"];
  String typeStr = system_get_sysinfo["mic_type"];
  typeStr.toLowerCase();

  // Device is a smartplug
  if(typeStr.indexOf("smartplug") != -1)
  {
    handler->type=TPLNetworkLocalUtilities::SmartPlug;
    return;
  }

  // Device is a smartplug
  if(typeStr.indexOf("smartbulb") != -1)
  {
    handler->type=TPLNetworkLocalUtilities::SmartBulb;
    return;
  }
}

void TPLNetworkManager::update()
{
  // Local Discovery Update Start
  StaticJsonDocument<1024> doc;
  String ip="";
  if(localDiscovery.update(doc, ip))
  {
    JsonObject system_get_sysinfo = doc["system"]["get_sysinfo"];
    const char* deviceId = system_get_sysinfo["deviceId"];

    for(std::list<TPLClientHandler*>::iterator iter= clientList.begin(); iter != clientList.end(); iter++)
    {
      TPLClientHandler *handler = *iter;

      if(deviceId && strncmp(handler->deviceID, deviceId, DEVICE_ID_LENGTH) == 0)
      {
        Serial.print("Handler Device ID: ");
        Serial.println(deviceId);
        // Found device
        if(ip.length() > 0)
        {
          handler->ip = ip;
        }
        
        handler->relayState=system_get_sysinfo["relay_state"];
        TPLNetworkManager::toType(doc, handler);

        Serial.print("ClientHandle Updated");
        Serial.print(" relayState: ");
        Serial.print(handler->relayState);
        Serial.print(" IP: ");
        Serial.println(handler->ip);
        
        handler->onPacketReceived(doc);
      }
    }
  }
  // Local Discovery Update End
  
  
}
