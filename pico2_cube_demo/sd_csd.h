#ifndef SD_CSD_H
#define SD_CSD_H

#include <stdbool.h>
#include <stdint.h>

// Parse the 16-byte CSD register into the number of 512-byte sectors.
bool sd_csd_capacity_sectors(const uint8_t csd[16], uint64_t *sectors);

#endif
