#pragma once

// Common AMD MMIO registers
#define mmMM_INDEX	0x00
#define mmMM_DATA	0x01
#define mmSMN_INDEX	0x14
#define mmSMN_DATA	0x15

typedef enum 
{
    AMD_REGSPACE_MM, 
    AMD_REGSPACE_SMN
} amd_regspace_t;

// Navi SDMA EDC counter register addresses
// 2 channels, 8 counters per channel, 2 registers (lo/hi) per counter

// channel 1 read regs
#define mmPerfMonCtr2_Lo_0 0x1434c      // 0x50d30
#define mmPerfMonCtr2_Hi_0 0x1434d      // 0x50d34

// channel 1 write regs
#define mmPerfMonCtr4_Lo_0 0x14350      // 0x50d40
#define mmPerfMonCtr4_Hi_0 0x14351      // 0x50d44

// channel 2 - not using for now, as it appears counts are always
// exactly the same as ch1, so we could just use ch1 x 2
//
// atitool reads regs 0x[N]51d30/4 for channel 2, but may be 
// incorrect, as reg dump seems to indicate it should be 0x[N]52d30/4?

typedef struct _PerfMonCtr_Lo
{
  uint32_t  Data : 32;
} PerfMonCtr_Lo;

typedef struct _PerfMonCtr_Hi
{
  uint32_t Data : 16;
  uint32_t Overflow : 1;
} PerfMonCtr_Hi;

