#include "table.h"

// Physical SRAMs with large continuous memory blocks
#define NUM_PHYSICAL_SRAM 4
#define PHYSICAL_SRAM_CAPACITY (1024 * 1024)
static uint8_t physical_srams[NUM_PHYSICAL_SRAM][PHYSICAL_SRAM_CAPACITY];
static int physical_sram_top[NUM_PHYSICAL_SRAM];

// Physical TCAMs
#define NUM_PHYSICAL_TCAM 4
#define PHYSICAL_TCAM_CAPACITY (1024 * 1024)
static uint8_t physical_tcams[NUM_PHYSICAL_TCAM][PHYSICAL_TCAM_CAPACITY];
static int physical_tcam_top[NUM_PHYSICAL_TCAM];

Buffer alloc_logical_sram(int phy_id, int num_bytes) {
    int &top = physical_sram_top[phy_id];
    Buffer buf = physical_srams[phy_id] + top;
    top += num_bytes;
    if (top > PHYSICAL_SRAM_CAPACITY) {
        throw std::overflow_error("Physical SRAM out of memory");
    }
    return buf;
}

Buffer alloc_logical_tcam(int phy_id, int num_bytes) {
    int &top = physical_tcam_top[phy_id];
    Buffer buf = physical_tcams[phy_id] + top;
    top += num_bytes;
    if (top > PHYSICAL_SRAM_CAPACITY) {
        throw std::overflow_error("Physical SRAM out of memory");
    }
    return buf;
}
