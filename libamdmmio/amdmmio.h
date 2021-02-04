#pragma once

#include <stdint.h>
#include "amdmmioregs.h"

typedef enum _AMD_MMIO_ACCESS_TYPE
{
	AMD_MMIO_ACCESS_TYPE_INVALID = 0,
	AMD_MMIO_ACCESS_TYPE_DEBUGFS = 1,
	AMD_MMIO_ACCESS_TYPE_LIBPCIACCESS = 2,
	AMD_MMIO_ACCESS_TYPE_ATILLK64_SYS = 3
} AMD_MMIO_ACCESS_TYPE;

#define AMD_MMIO_ERR_SUCCESS						0
#define AMD_MMIO_ERR_UNSUPPORTED_MMIO_ACCESS_TYPE	-1
#define AMD_MMIO_ERR_GPU_INDEX_NOT_FOUND			-2
#define AMD_MMIO_ERR_BACKEND						-3

typedef struct _AMDGPU_PCI_INFO
{
	void *MMIOBasePtr;
	
	uint8_t Revision;
	uint32_t MMIOBaseAddr, MMIOSize;
	uint16_t VendorID, DeviceID, SubVendor, SubDevice, PCIBus, PCIDevice, PCISlot, PCIFunction;
} AMDGPU_PCI_INFO;

typedef struct _AMDGPU_DEBUGFS_INFO
{
	uint8_t idx;
	int fd;
} AMDGPU_DEBUGFS_INFO;

typedef struct _AMDGPU
{
	#ifdef _WIN32
	HANDLE MMIOHandle;
	#endif
	
	AMD_MMIO_ACCESS_TYPE AccessType;
	
	AMDGPU_PCI_INFO PCIInfo;
	AMDGPU_DEBUGFS_INFO DebugFSInfo;
	
	struct _AMDGPU *next, *prev;
} AMDGPU;

int32_t InitGPUByIndex(AMDGPU *GPU, AMD_MMIO_ACCESS_TYPE atype, uint8_t idx);
int32_t ReadMMIOReg(AMDGPU *GPU, amd_regspace_t regspace, uint32_t reg, uint32_t *outval);
int32_t WriteMMIOReg(AMDGPU *GPU, uint32_t reg, uint32_t val);
int32_t FreeGPU(AMDGPU *GPU);
