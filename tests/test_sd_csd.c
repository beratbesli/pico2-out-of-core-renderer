#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "sd_csd.h"

int main(void) {
    uint64_t sectors = 0;
    uint8_t v2[16] = {0};
    v2[0] = 0x40;
    v2[7] = 0x3f;
    v2[8] = 0xff;
    v2[9] = 0xff;
    assert(sd_csd_capacity_sectors(v2, &sectors));
    assert(sectors == 4294967296ULL); // 2 TiB: exceeds uint32_t sectors.

    uint8_t v1[16] = {0};
    v1[5] = 9;
    v1[6] = 3;
    v1[7] = 0xff;
    v1[8] = 0xc0;
    v1[9] = 3;
    v1[10] = 0x80;
    assert(sd_csd_capacity_sectors(v1, &sectors));
    assert(sectors == 2097152ULL);

    v1[5] = 8;
    assert(!sd_csd_capacity_sectors(v1, &sectors));
    v1[0] = 0x80;
    assert(!sd_csd_capacity_sectors(v1, &sectors));
    assert(!sd_csd_capacity_sectors(0, &sectors));
    assert(!sd_csd_capacity_sectors(v2, 0));
    puts("sd_csd host tests passed");
    return 0;
}
