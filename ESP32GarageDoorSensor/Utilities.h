/*
 * File: Utilities.h
 * Description: hold Utility functions
 */

#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include <Arduino.h>
#include<vector>

void AddUint8ToVector(uint8_t* bytes, int bytesLen, std::vector<uint8_t>& vect);
void printByteString(std::vector<uint8_t> dataBytes);
void printHexString(std::vector<uint8_t> dataBytes);
void printHexString(int dataBytes);

#endif
