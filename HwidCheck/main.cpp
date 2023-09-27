#include "HwidChecker.h"
#include <cstdio>

int main()
{
	auto CPUSign = HwidChecker::GetCPUSign();
	auto DiskSerial = HwidChecker::GetDiskSerial();
	auto SMBDataMd5 = HwidChecker::GetSMBDataHash();
	auto GPUDeviceID = HwidChecker::GetGPUDeviceIdentifier();
	auto EthernetMac = HwidChecker::GetNetworkAdapterInfo();
	auto HwidHash = HwidChecker::GenerateHwidHash();
	
	printf("CPUSign:%X\n", CPUSign);
	printf("DiskSerial:%X\n", DiskSerial);
	printf("SMBDataMd5:%s\n", SMBDataMd5.c_str());
	printf("GPUDeviceID:{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}\n", 
		GPUDeviceID.Data1, GPUDeviceID.Data2, GPUDeviceID.Data3, 
		GPUDeviceID.Data4[0], GPUDeviceID.Data4[1], GPUDeviceID.Data4[2], GPUDeviceID.Data4[3],
		GPUDeviceID.Data4[4], GPUDeviceID.Data4[5], GPUDeviceID.Data4[6], GPUDeviceID.Data4[7]);

	PBYTE MAC = (PBYTE)&EthernetMac;
	printf("EthernetMac:{%02X-%02X-%02X-%02X-%02X-%02X}\n", MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);

	printf("HwidHash:%s\n", HwidHash.c_str());

	return 0;
}
