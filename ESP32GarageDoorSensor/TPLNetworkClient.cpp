/*
 * File: CTPLNetworkClient.cpp
 * Description: Handle network device interactions
 */
#include "TPLNetworkClient.h"
#include <WiFi.h>
#include "Timer.h"

#include <vector>
#include <list>
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
  unsigned int len = TPLNetworkLocalUtilities::littleToBigEndian(inputJSON.size());
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

bool TPLNetworkCloudClient::connect()
{
  return false;
}

bool TPLNetworkCloudClient::disconnect()
{
  return false;
}

bool TPLNetworkCloudClient::isConnected()
{
  return false;
}

String TPLNetworkCloudClient::sendRequest(String jsonStr)
{
  return String("");
}

const char* TPLLocalDiscovery::udpDiscoveryAddress = "255.255.255.255";

bool TPLNetworkLocalClient::connect()
{
  return false;
}

bool TPLNetworkLocalClient::disconnect()
{
  return false;
}

void TPLNetworkLocalClient::start()
{
}



void TPLNetworkLocalClient::update()
{
}

bool TPLNetworkLocalClient::isConnected()
{
  return false;
}

String TPLNetworkLocalClient::sendRequest(String jsonStr)
{
  return String("");
}

void TPLLocalDiscovery::addClient(TPLNetworkLocalClient client)
{
  list.push_back(client);
}

void TPLLocalDiscovery::start()
{
  udp.begin(WiFi.localIP(),udpDiscoveryPort);
}

void TPLLocalDiscovery::stop()
{
  
}

void TPLLocalDiscovery::update()
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
    TPLNetworkLocalUtilities::encrypt(discoveryPacket, packedPacket);
    
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
    TPLNetworkLocalUtilities::decrypt(packedPacket, decryptedPacket);
    printByteString(decryptedPacket);
  }
}
