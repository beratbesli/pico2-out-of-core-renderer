#include "sd_csd.h"

bool sd_csd_capacity_sectors(const uint8_t csd[16], uint64_t *sectors) {
    if (csd == 0 || sectors == 0) return false;

    uint8_t structure = csd[0] >> 6;
    uint64_t count;
    if (structure == 1) { // SDHC/SDXC CSD v2
        uint32_t c_size = ((uint32_t)(csd[7] & 0x3f) << 16) |
                          ((uint32_t)csd[8] << 8) | csd[9];
        count = ((uint64_t)c_size + 1) * 1024;
    } else if (structure == 0) { // SDSC CSD v1
        uint8_t read_bl_len = csd[5] & 0x0f;
        if (read_bl_len < 9 || read_bl_len > 11) return false;
        uint32_t c_size = ((uint32_t)(csd[6] & 0x03) << 10) |
                          ((uint32_t)csd[7] << 2) | (csd[8] >> 6);
        uint8_t c_size_mult = ((csd[9] & 0x03) << 1) | (csd[10] >> 7);
        count = ((uint64_t)c_size + 1) << (c_size_mult + 2 + read_bl_len - 9);
    } else {
        return false;
    }

    if (count == 0) return false;
    *sectors = count;
    return true;
}
