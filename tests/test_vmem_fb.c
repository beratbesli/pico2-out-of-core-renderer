#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "vmem_fb.h"

#define BASE_SECTOR 32U
#define STORAGE_SECTORS (BASE_SECTOR + VMEM_TOTAL_PAGES + 4U)

static uint8_t storage[STORAGE_SECTORS][VMEM_PAGE_SIZE];
static uint64_t fake_time_us;

uint64_t time_us_64(void) {
    return fake_time_us++;
}

bool sd_read_sector(uint32_t sector_lba, uint8_t *buffer) {
    if (sector_lba >= STORAGE_SECTORS) {
        return false;
    }
    memcpy(buffer, storage[sector_lba], VMEM_PAGE_SIZE);
    return true;
}

bool sd_write_sector(uint32_t sector_lba, const uint8_t *buffer) {
    if (sector_lba >= STORAGE_SECTORS) {
        return false;
    }
    memcpy(storage[sector_lba], buffer, VMEM_PAGE_SIZE);
    return true;
}

static void pixel_for_page(uint32_t page, int *x, int *y) {
    uint32_t offset = page * VMEM_PAGE_SIZE;
    *y = (int)(offset / VMEM_WIDTH);
    *x = (int)(offset % VMEM_WIDTH);
}

int main(void) {
    memset(storage, '.', sizeof(storage));
    assert(vmem_init(BASE_SECTOR));

    vmem_put_pixel(0, 0, 'A');
    assert(vmem_get_pixel(0, 0) == 'A');
    vmem_stats_t first = vmem_get_stats();
    assert(first.accesses == 2);
    assert(first.cache_hits == 1);
    assert(first.cache_misses == 1);
    assert(first.sd_reads == 1);

    for (uint32_t page = 1; page <= VMEM_CACHE_SLOTS; page++) {
        int x, y;
        pixel_for_page(page, &x, &y);
        vmem_put_pixel(x, y, (char)('a' + (page % 26)));
    }
    vmem_flush();

    int x, y;
    pixel_for_page(1, &x, &y);
    assert(storage[BASE_SECTOR + 1][0] == (uint8_t)('a' + (1 % 26)));
    assert(vmem_get_pixel(x, y) == (char)('a' + (1 % 26)));

    assert(vmem_get_pixel(-1, 0) == ' ');
    assert(vmem_get_pixel(VMEM_WIDTH, 0) == ' ');
    puts("vmem_fb host tests passed");
    return 0;
}
