/*
 * File: CTPLinkSmartDevice.cpp
 * Description: Interfaces with TP-Link smart devices over coud API
 */

#include "CTPLinkSmartDevice.h"


bool CTPLinkSmartDevice::SetRelay(bool state)
{
  return false;
}

void CTPLinkSmartDevice::GetRelay(bool &state)
{
}

int CTPLinkSmartDevice::GetError()
{
  return 0;
}

int CTPLinkSmartDevice::UpdateTime()
{
  return 0;
}

bool CTPLinkSmartDevice::Update()
{
  return false;
}
