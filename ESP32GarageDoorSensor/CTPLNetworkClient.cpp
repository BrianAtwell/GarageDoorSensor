/*
 * File: CTPLNetworkClient.cpp
 * Description: Handle network device interactions
 */
#include "CTPLNetworkClient.h"
#include <WiFi.h>
#include "Timer.h"

#include <vector>
#include "Utilities.h"

int CTPLNetworkLocalUtilities::littleToBigEndian(int dataIn)
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

void CTPLNetworkLocalUtilities::xorEncrypt(std::vector<uint8_t>& unencrypted, std::vector<uint8_t>& output)
{
  int len = unencrypted.size();
  uint8_t key = CTPLNetworkLocalUtilities::InitializationVector;
  for(int i=0; i<len; i++)
  {
      key = key ^ unencrypted[i];
      output.push_back(key);
  }
}

void CTPLNetworkLocalUtilities::encrypt(std::vector<uint8_t> &inputJSON, std::vector<uint8_t>& pack)
{
  unsigned int len = CTPLNetworkLocalUtilities::littleToBigEndian(inputJSON.size());
  CTPLNetworkLocalUtilities::xorEncrypt(inputJSON, pack);
  pack.insert(pack.begin()++, (uint8_t*)&len, ((uint8_t*)&len)+4);
}


void CTPLNetworkLocalUtilities::xorDecrypted(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &output)
{
  int len = ciphertext.size();
  uint8_t key = CTPLNetworkLocalUtilities::InitializationVector;
  uint8_t plainbyte=0;
  for(int i=0; i<len; i++)
  {
      plainbyte = key ^ ciphertext[i];
      key = ciphertext[i];
      output.push_back(plainbyte);
  }
}

void CTPLNetworkLocalUtilities::decrypt(std::vector<uint8_t> &ciphertext, std::vector<uint8_t> &unencrypted)
{
  CTPLNetworkLocalUtilities::xorDecrypted(ciphertext, unencrypted);
}

bool CTPLNetworkCloudClient::connect()
{
  return false;
}

bool CTPLNetworkCloudClient::disconnect()
{
  return false;
}

bool CTPLNetworkCloudClient::isConnected()
{
  return false;
}

String CTPLNetworkCloudClient::sendRequest(String jsonStr)
{
  return String("");
}

const char* CTPLLocalDiscovery::udpDiscoveryAddress = "255.255.255.255";

bool CTPLNetworkLocalClient::connect()
{
  return false;
}

bool CTPLNetworkLocalClient::disconnect()
{
  return false;
}

void CTPLNetworkLocalClient::start()
{
}



void CTPLNetworkLocalClient::update()
{
}

bool CTPLNetworkLocalClient::isConnected()
{
  return false;
}

String CTPLNetworkLocalClient::sendRequest(String jsonStr)
{
  return String("");
}

void CTPLLocalDiscovery::start()
{
  udp.begin(WiFi.localIP(),udpDiscoveryPort);
}

void CTPLLocalDiscovery::stop()
{
  
}

void CTPLLocalDiscovery::update()
{
  char dataStr[] = "{\"system\": {\"get_sysinfo\": null}}";
  std::vector<uint8_t> discoveryPacket;
  std::vector<uint8_t> packedPacket;
  const uint16_t BUFFER_SIZE = 50;
  char buffer[51];
  int readBytes=0;

  AddUint8ToVector((uint8_t*)dataStr, sizeof(dataStr)-1, discoveryPacket);
  
  //Send a packet every x seconds
  if(discoveryTimer.Update())
  {
    //Serial.println("Discovery Packet Raw:");
    //printHexString(discoveryPacket);
    //Serial.println("Sending Discovery Packet");
    
    //udp.printf("Seconds since boot: %lu", millis()/1000);
    
    // Encrypt message
    CTPLNetworkLocalUtilities::encrypt(discoveryPacket, packedPacket);
    
    //Serial.println("Discovery Packet Encrypted: ");
    //printHexString(packedPacket);
    //Serial.println();

    // Trim beginning length bytes
    packedPacket.erase (packedPacket.begin(), packedPacket.begin()+4);

    //Serial.println("Discovery Packet Encrypted and trimmed: ");
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
    Serial.print("Packet available from remote IP: ");
    Serial.println(udp.remoteIP());
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
    std::vector<uint8_t> decryptedPacket;
    CTPLNetworkLocalUtilities::decrypt(packedPacket, decryptedPacket);
    printByteString(decryptedPacket);
  }
}
