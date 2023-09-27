#include "HwidChecker.h"
#include <intrin.h>
#include <cstdint>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#include <iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")
#include "md5/md5.h"


namespace HwidChecker
{
	typedef struct _DMI_HEADER
	{
		BYTE	Type;
		BYTE	Length;
		WORD	Handle;
	} DMI_HEADER;

	typedef struct _RawSMBIOSData
	{
		BYTE    Used20CallingMethod;
		BYTE    SMBIOSMajorVersion;
		BYTE    SMBIOSMinorVersion;
		BYTE    DmiRevision;
		DWORD   Length;
		BYTE    SMBIOSTableData[1];
	} RawSMBIOSData;

	uint32_t GetCPUSign()
	{
		/*
		* cpuid Function ID
		* 0				//vendor string
		* 1				//处理器签名和功能位
		* 3				//CPUSerialNumber 一般被关闭
		* 0x80000001	//最大扩展功能号
		* 0x80000002	//返回CPU支持的扩展功能
		* 0x80000008	//虚拟地址和物理地址大小
		*/
		uint32_t Regs[4];
		uint32_t FunctionID = 1;	//处理器签名和功能位
		__cpuid((int*)Regs, FunctionID);
		uint32_t CPUSign = Regs[0];	//eax
		return CPUSign;
	}

	uint32_t GetDiskSerial()
	{
		DWORD SerialNum;	//序列号
		CHAR Disk[4];
		DWORD Enc = 0xDEF1FA9D;
		*(DWORD*)Disk = Enc ^ 0xDEADC0DE;
		::GetVolumeInformationA("C:\\", NULL, 0, &SerialNum, NULL, NULL, NULL, 0);
		return SerialNum;
	}

	std::string GetSMBDataHash()
	{
		DWORD SMBBufferSize = GetSystemFirmwareTable('RSMB', 0, 0, 0);
		if (SMBBufferSize <= 0) {
			return {};
		}
		PVOID FirmwareTableBuffer = VirtualAlloc(0, SMBBufferSize, MEM_COMMIT, PAGE_READWRITE);
		if (!FirmwareTableBuffer) {
			return {};
		}
		DWORD SMBDataSize = GetSystemFirmwareTable('RSMB', 0, FirmwareTableBuffer, SMBBufferSize);
		if (SMBDataSize != SMBBufferSize) {
			return {};
		}
		RawSMBIOSData* SMBData = (RawSMBIOSData*)FirmwareTableBuffer;
		//DMI_HEADER* DMIHeader = (DMI_HEADER*)SMBData->SMBIOSTableData;
	
		std::string TmpBuf = "";
		TmpBuf.resize(SMBData->Length);
		memcpy((void*)TmpBuf.data(), SMBData->SMBIOSTableData, SMBData->Length);

		VirtualFree(FirmwareTableBuffer, 0, MEM_RELEASE);
		return MD5(TmpBuf).toStr();
	}

	GUID GetGPUDeviceIdentifier()
	{
		IDirect3D9* DXObject = Direct3DCreate9(D3D_SDK_VERSION);
		if (!DXObject) {
			return {};
		}
		DWORD AdapterCount = DXObject->GetAdapterCount();
		if (!AdapterCount) {
			return {};
		}
		D3DADAPTER_IDENTIFIER9* Adapters = new D3DADAPTER_IDENTIFIER9[AdapterCount];
		for (unsigned int i = 0; i < AdapterCount; i++) {
			DXObject->GetAdapterIdentifier(i, 0, &Adapters[i]);
		}
		DXObject->Release();
		GUID DeviceIdentifier = Adapters[AdapterCount - 1].DeviceIdentifier;
		delete[] Adapters;
		return DeviceIdentifier;
	}

	uint64_t GetNetworkAdapterInfo()
	{
		ULONG AdapterInfoSize = 0;
		if (GetAdaptersInfo(NULL, &AdapterInfoSize) != ERROR_BUFFER_OVERFLOW) {
			return {};
		}
		IP_ADAPTER_INFO* AdapterInfoBuffer = (IP_ADAPTER_INFO*)VirtualAlloc(0, AdapterInfoSize, MEM_COMMIT, PAGE_READWRITE);
		if (!AdapterInfoBuffer) {
			return {};
		}
		if (GetAdaptersInfo(AdapterInfoBuffer, &AdapterInfoSize) != NO_ERROR) {
			return {};
		}
		uint64_t EthernetAdapterMacAddress = 0;
		IP_ADAPTER_INFO* AdapterNode = AdapterInfoBuffer;
		do {
			if (AdapterNode->Type == MIB_IF_TYPE_ETHERNET)
			{
				EthernetAdapterMacAddress = *(uint64_t*)&AdapterInfoBuffer->Address;
				break;
			}
			AdapterNode = AdapterNode->Next;
		} while (AdapterNode);
		VirtualFree(AdapterInfoBuffer, 0, MEM_RELEASE);
		return EthernetAdapterMacAddress;
	}

	std::string GenerateHwidHash()
	{
		return {};
	}
};
