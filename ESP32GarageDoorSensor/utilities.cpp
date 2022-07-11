/*
 * File: Utilities.h
 * Description: hold Utility functions
 */

#include <Arduino.h>
#include "Utilities.h"
#include<vector>

void AddUint8ToVector(uint8_t* bytes, int bytesLen, std::vector<uint8_t>& vect)
{
  for(int i=0; i<bytesLen; i++)
  {
    vect.push_back(bytes[i]);
  }
}

void printByteString(std::vector<uint8_t> dataBytes)
{
  size_t len=dataBytes.size();
  for(size_t i=0; i<len; i++)
  {
    Serial.print((char)dataBytes[i]);
  }

  Serial.println();
}

void printHexString(std::vector<uint8_t> dataBytes)
{
  size_t len=dataBytes.size();
  for(size_t i=0; i<len; i++)
  {
    Serial.print("0x");
    Serial.print((uint8_t)dataBytes[i], HEX);
    Serial.print(", ");
  }

  Serial.println();
}

void printHexString(int dataBytes)
{
  uint8_t* dataPtr=0;
  for(int i=0; i<4; i++)
  {
    Serial.print("0x");
    Serial.print((uint8_t)dataPtr[i], HEX);
    Serial.print(", ");
  }

  Serial.println();
}
