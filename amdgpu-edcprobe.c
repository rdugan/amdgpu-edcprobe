#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>

#include "libamdmmio/amdmmio.h"
#include "libamdmmio/amdmmioregs.h"

#define NUM_ARGS 1
#define HEX_CHARS "0123456789abcdefABCDEF"
#define DEC_CHARS "0123456789"

#define NUM_UMC_COUNTERS 8
#define COUNTER_OVERFLOW -1

// add 0x40000 (0x100000) for successive mem err counter registers 
#define CTR_ADDR_INCREMENT 0x40000	// 0x100000

typedef union _UMCCH_PMC_LO_REG
{
  uint32_t raw;
  PerfMonCtr_Lo asPMCLO;
} UMCCH_PMC_LO_REG;

typedef union _UMCCH_PMC_HI_REG
{
  uint32_t raw;
  PerfMonCtr_Hi asPMCHI;
} UMCCH_PMC_HI_REG;

typedef enum _MEM_ERRS_TYPE {
  ERR_TYPE_READ,
  ERR_TYPE_WRITE,
  ERR_TYPE_COUNT
} mem_errs_type_t;

typedef enum _MEM_ERRS_OPT { 
  ERR_OPT_READ='r', 
  ERR_OPT_WRITE='w', 
  ERR_OPT_BOTH='b', 
  ERR_OPT_ALL='a' 
} mem_errs_opt_t;

const char *argp_program_version =
  "amdgpu-edcprobe 0.1.0";

// short doc string for help
static char doc[] =
  "amdgpu-edcprobe -- retrieve values from AMD GPU EDC counters";

// arg name(s)
static char args_doc[] = "AMD_GPU_INDEX";

// options definitions
static struct argp_option options[] = {
  {"mem-errs", 'm', "MEM_ERRS_TYPE", OPTION_ARG_OPTIONAL,  
   "display memory errors of TYPE r(ead), w(rite), b(oth) [DEFAULT], or t(otal)" },
  {"verbose",  'v', 0, 0,  "show counters by individual register" },
  { 0 }
};

// used by main to communicate with parse_opt
struct arguments
{
  char *args[NUM_ARGS];
  int GPUIndex;
  mem_errs_type_t memErrsOpt;
  int verbose;
};

// option parser
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  // get input arguments from argp_parse
  struct arguments *arguments = state->input;

  switch (key) {
    case 'm' : 
      if (arg) {
        if (strlen(arg) == 1) {
          switch (arg[0]) {
            case ERR_OPT_READ :
            case ERR_OPT_WRITE :
            case ERR_OPT_BOTH :
            case ERR_OPT_ALL :
              arguments->memErrsOpt = (mem_errs_opt_t) arg[0];
              break;
            default :
              argp_usage(state);
          }
        }
        else
          argp_usage(state);
      }
      break;
    case 'v':
      arguments->verbose = 1;
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num >= NUM_ARGS)
        // too many arguments
        argp_usage (state);
      else if (strncmp(arg, "0x", 2) == 0) {
        char *argVal = arg + 2;
        if (argVal[strspn(argVal, HEX_CHARS)] == 0)
          arguments->GPUIndex = strtoul(argVal, NULL, 16);
        else
          argp_usage (state);
      }
      else if (arg[strspn(arg, DEC_CHARS)] == 0)
          arguments->GPUIndex = strtoul(arg, NULL, 10);
      else
          argp_usage (state);

      arguments->args[state->arg_num] = arg;

      break;

    case ARGP_KEY_END:
      if (state->arg_num < NUM_ARGS)
        // not enough arguments
        argp_usage (state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

void print_mem_errs(struct arguments *arguments, int64_t counters[ERR_TYPE_COUNT][NUM_UMC_COUNTERS]) {
    int64_t totalRead = 0;
    int64_t totalWrite = 0;

    switch (arguments->memErrsOpt) {
      case ERR_OPT_READ :
        puts("\nType   | Read");
        puts("-------------------------");
        break;
      case ERR_OPT_WRITE :
        puts("\nType   | Write");
        puts("-------------------------");
        break;
      case ERR_OPT_BOTH :
        puts("\nType   | Read            | Write");
        puts("-------------------------------------------");
        break;
      case ERR_OPT_ALL :
        puts("\nType   | All");
        puts("-------------------------");
    }

    for (int i = 0; i < NUM_UMC_COUNTERS; i++) {
      int64_t sum = 0;
      if (arguments->verbose == 1) {
        printf("UMC %d:   ", i);
        switch (arguments->memErrsOpt) {
          case ERR_OPT_ALL :
            if (counters[ERR_TYPE_READ][i] == COUNTER_OVERFLOW || counters[ERR_TYPE_WRITE][i] == COUNTER_OVERFLOW)
              sum = COUNTER_OVERFLOW;
            else
              sum = counters[ERR_TYPE_READ][i] + counters[ERR_TYPE_WRITE][i];
            printf("%*ld", 15, sum);
            break;
          case ERR_OPT_READ :
            printf("%*ld", 15, counters[ERR_TYPE_READ][i]);
            break;
          case ERR_OPT_WRITE :
            printf("%*ld", 15, counters[ERR_TYPE_WRITE][i]);
            break;
          case ERR_OPT_BOTH :
            printf("%*ld", 15, counters[ERR_TYPE_READ][i]);
            printf("%*ld", 18, counters[ERR_TYPE_WRITE][i]);
        }
        printf("\n");
      }

      if (arguments->memErrsOpt != ERR_OPT_WRITE) {
        if (totalRead == COUNTER_OVERFLOW || counters[ERR_TYPE_READ][i] == COUNTER_OVERFLOW) {
          totalRead = COUNTER_OVERFLOW;
          if (arguments->memErrsOpt == ERR_OPT_READ && arguments->verbose == 0)
            // no need to continue if only printing total
            break;
        }
        else
          totalRead += counters[ERR_TYPE_READ][i];
      }

      if (arguments->memErrsOpt != ERR_OPT_READ) {
        if (totalWrite == COUNTER_OVERFLOW || counters[ERR_TYPE_WRITE][i] == COUNTER_OVERFLOW) {
          totalWrite = COUNTER_OVERFLOW;
          if (arguments->memErrsOpt == ERR_OPT_WRITE && arguments->verbose == 0)
            // no need to continue if only printing total
            break;
        }
        else
          totalWrite += counters[ERR_TYPE_WRITE][i];
      }

    }

    printf("Total:   ");
    switch (arguments->memErrsOpt) {
      case ERR_OPT_ALL :
        printf("%*ld", 15, totalRead + totalWrite);
        break;
      case ERR_OPT_READ :
        printf("%*ld", 15, totalRead);
        break;
      case ERR_OPT_WRITE :
        printf("%*ld", 15, totalWrite);
        break;
      case ERR_OPT_BOTH :
        printf("%*ld", 15, totalRead);
        printf("%*ld", 18, totalWrite);
    }
    printf("\n");

  return;
}

int main(int argc, char **argv) {
  AMDGPU GPU;
  int32_t status;
  struct arguments arguments;

  // array for register contents - one row each for read/write, 
  // with 8 columns for individual UMCs
  int64_t counters[ERR_TYPE_COUNT][NUM_UMC_COUNTERS];

  // set up arg defaults, then parse args
  arguments.verbose = 0;
  arguments.memErrsOpt = ERR_OPT_BOTH;

  argp_parse (&argp, argc, argv, 0, 0, &arguments);

  status = InitGPUByIndex(&GPU, AMD_MMIO_ACCESS_TYPE_DEBUGFS, arguments.GPUIndex);

  if(status != AMD_MMIO_ERR_SUCCESS)
  {
    printf("InitGPUByIndex() returned %d!\n", status);
    FreeGPU(&GPU);
    return status;
  }

  // loop over registers collecting error counts
  for (int i = 0; i < NUM_UMC_COUNTERS; i++) {
    UMCCH_PMC_LO_REG counterLow;
    UMCCH_PMC_HI_REG counterHigh;

    for (mem_errs_type_t errType = 0; errType < ERR_TYPE_COUNT; errType++) {
      amd_regspace_t regSpace = (i > 0) ? AMD_REGSPACE_SMN : AMD_REGSPACE_MM;
      uint32_t regBase = (errType == ERR_TYPE_READ) ? mmPerfMonCtr2_Lo_0 : mmPerfMonCtr4_Lo_0;
      uint32_t regAddress = regBase + (CTR_ADDR_INCREMENT * i);

      status = ReadMMIOReg(&GPU, regSpace, regAddress, &counterLow.raw);
      if (status != AMD_MMIO_ERR_SUCCESS) break;

      regBase = (errType == ERR_TYPE_READ) ? mmPerfMonCtr2_Hi_0 : mmPerfMonCtr4_Hi_0;
      regAddress = regBase + (CTR_ADDR_INCREMENT * i);
      status = ReadMMIOReg(&GPU, regSpace, regAddress, &counterHigh.raw);
      if (status != AMD_MMIO_ERR_SUCCESS) break;

      if (counterHigh.asPMCHI.Overflow == 1)
        counters[errType][i] = COUNTER_OVERFLOW;
      else
        counters[errType][i] = (counterHigh.asPMCHI.Data << 16) + counterLow.asPMCLO.Data;
    }

    if (status != AMD_MMIO_ERR_SUCCESS) break;
  }

  if(status == AMD_MMIO_ERR_SUCCESS) {
    print_mem_errs(&arguments, counters);
    status = 0;
  }
  else
    printf("\nReadMMIOReg() returned %d!\n", status);

  FreeGPU(&GPU);

  return status;
}

