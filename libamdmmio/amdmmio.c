#include <stdio.h>			// For snprintf()
#include <fcntl.h>			// For O_RDWR and O_SYNC
#include <unistd.h>			// For pread()/pwrite()/close()
#include <string.h>			// For memset()

#include "amdmmio.h"
#include "amdmmioregs.h"

int32_t InitGPUByIndex(AMDGPU *GPU, AMD_MMIO_ACCESS_TYPE atype, uint8_t idx)
{
	char path[1024];
	
	if(atype != AMD_MMIO_ACCESS_TYPE_DEBUGFS) return(AMD_MMIO_ERR_UNSUPPORTED_MMIO_ACCESS_TYPE);
	
	memset(GPU, 0x00, sizeof(AMDGPU));
	
	GPU->AccessType = atype;
	
	// For DebugFS access type
	if(GPU->AccessType == AMD_MMIO_ACCESS_TYPE_DEBUGFS)
	{
		snprintf(path, 1023, "/sys/kernel/debug/dri/%d/amdgpu_regs", idx);
		GPU->DebugFSInfo.fd = open(path, O_RDWR | O_SYNC);
		
		if(GPU->DebugFSInfo.fd == -1) return(AMD_MMIO_ERR_GPU_INDEX_NOT_FOUND);
		GPU->DebugFSInfo.idx = idx;
		
		return(AMD_MMIO_ERR_SUCCESS);
	}
}

int32_t ReadMMIOReg(AMDGPU *GPU, amd_regspace_t regspace, uint32_t reg, uint32_t *outval)
{
	uint32_t pIndex;
	uint32_t pData;

	switch (regspace)
	{
		case AMD_REGSPACE_SMN :
			pIndex = mmSMN_INDEX;
			pData = mmSMN_DATA;
			break;

		default :
			pIndex = mmMM_INDEX;
			pData = mmMM_DATA;
	}

	reg <<= 2;
	
	if(GPU->AccessType == AMD_MMIO_ACCESS_TYPE_DEBUGFS)
	{
		if(pwrite(GPU->DebugFSInfo.fd, &reg, (size_t)4, (pIndex << 2)) != 4)
		{
			return(AMD_MMIO_ERR_BACKEND);
		}
		
		if(pread(GPU->DebugFSInfo.fd, outval, (size_t)4, (pData << 2)) != 4)
		{
			return(AMD_MMIO_ERR_BACKEND);
		}
	}
		
	return(AMD_MMIO_ERR_SUCCESS);
}

int32_t WriteMMIOReg(AMDGPU *GPU, amd_regspace_t regspace, uint32_t reg, uint32_t val)
{
        uint32_t pIndex;
        uint32_t pData;

        switch (regspace)
        {
                case AMD_REGSPACE_SMN :
                        pIndex = mmSMN_INDEX;
                        pData = mmSMN_DATA;
                        break;

                default :
                        pIndex = mmMM_INDEX;
                        pData = mmMM_DATA;
        }

	reg <<= 2;
	
	if(GPU->AccessType == AMD_MMIO_ACCESS_TYPE_DEBUGFS)
	{
		if(pwrite(GPU->DebugFSInfo.fd, &reg, (size_t)4, (pIndex << 2)) != 4)
		{
			return(AMD_MMIO_ERR_BACKEND);
		}
		
		if(pwrite(GPU->DebugFSInfo.fd, &val, (size_t)4, (pData << 2)) != 4)
		{
			return(AMD_MMIO_ERR_BACKEND);
		}
	}
	
	return(AMD_MMIO_ERR_SUCCESS);
}

int32_t FreeGPU(AMDGPU *GPU)
{
	close(GPU->DebugFSInfo.fd);
	GPU->DebugFSInfo.idx = 0;
	GPU->AccessType = AMD_MMIO_ACCESS_TYPE_INVALID;
	
	return(AMD_MMIO_ERR_SUCCESS);
}
