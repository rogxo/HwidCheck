#pragma once
#include <Windows.h>
#include <string>

namespace HwidChecker
{
	uint32_t GetCPUSign();
	uint32_t GetDiskSerial();
	std::string GetSMBDataHash();
	GUID GetGPUDeviceIdentifier();
	uint64_t GetNetworkAdapterInfo();
	std::string GenerateHwidHash();
};

