/*
 * File: TPLinkSmartDevice.cpp
 * Description: Interfaces with TP-Link smart devices over coud API
 */

#include "TPLinkSmartDevice.h"


bool TPLinkSmartDevice::SetRelay(bool state)
{
  return false;
}

void TPLinkSmartDevice::GetRelay(bool &state)
{
}

int TPLinkSmartDevice::GetError()
{
  return 0;
}

int TPLinkSmartDevice::UpdateTime()
{
  return 0;
}

bool TPLinkSmartDevice::Update()
{
  return false;
}
