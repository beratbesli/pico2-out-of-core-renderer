#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "vmem_fb.h"

#define BASE_SECTOR 32U
#define STORAGE_SECTORS (BASE_SECTOR + VMEM_TOTAL_PAGES + 4U)

static uint8_t storage[STORAGE_SECTORS][VMEM_PAGE_SIZE];
static uint64_t fake_time_us;
static int32_t fail_read_sector = -1;
static int32_t fail_write_sector = -1;

uint64_t time_us_64(void) {
    return fake_time_us++;
}

bool sd_read_sector(uint32_t sector_lba, uint8_t *buffer) {
    if (sector_lba == (uint32_t)fail_read_sector) {
        memset(buffer, '?', VMEM_PAGE_SIZE); // Simulate a partially copied failed read.
        return false;
    }
    if (sector_lba >= STORAGE_SECTORS) {
        return false;
    }
    memcpy(buffer, storage[sector_lba], VMEM_PAGE_SIZE);
    return true;
}

bool sd_write_sector(uint32_t sector_lba, const uint8_t *buffer) {
    if (sector_lba == (uint32_t)fail_write_sector) return false;
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

    char ch;
    assert(vmem_put_pixel(0, 0, 'A'));
    assert(vmem_get_pixel(0, 0, &ch) && ch == 'A');
    vmem_stats_t first = vmem_get_stats();
    assert(first.accesses == 2);
    assert(first.cache_hits == 1);
    assert(first.cache_misses == 1);
    assert(first.sd_reads == 1);

    for (uint32_t page = 1; page <= VMEM_CACHE_SLOTS; page++) {
        int x, y;
        pixel_for_page(page, &x, &y);
        assert(vmem_put_pixel(x, y, (char)('a' + (page % 26))));
    }
    assert(vmem_flush());

    int x, y;
    pixel_for_page(1, &x, &y);
    assert(storage[BASE_SECTOR + 1][0] == (uint8_t)('a' + (1 % 26)));
    assert(vmem_get_pixel(x, y, &ch) && ch == (char)('a' + (1 % 26)));

    assert(vmem_get_pixel(-1, 0, &ch) && ch == ' ');
    assert(vmem_get_pixel(VMEM_WIDTH, 0, &ch) && ch == ' ');

    // A failed writeback cannot evict or discard the dirty victim.
    assert(vmem_init(BASE_SECTOR));
    assert(vmem_put_pixel(0, 0, 'X'));
    for (uint32_t page = 1; page < VMEM_CACHE_SLOTS; page++) {
        pixel_for_page(page, &x, &y);
        assert(vmem_get_pixel(x, y, &ch));
    }
    fail_write_sector = BASE_SECTOR;
    pixel_for_page(VMEM_CACHE_SLOTS, &x, &y);
    assert(!vmem_put_pixel(x, y, 'Z'));
    assert(vmem_get_pixel(0, 0, &ch) && ch == 'X');
    assert(!vmem_flush());
    assert(storage[BASE_SECTOR][0] != 'X');
    fail_write_sector = -1;
    assert(vmem_flush());
    assert(storage[BASE_SECTOR][0] == 'X');

    // A failed read, even one that wrote to its buffer, is never cached.
    assert(vmem_init(BASE_SECTOR));
    fail_read_sector = BASE_SECTOR;
    assert(!vmem_get_pixel(0, 0, &ch));
    fail_read_sector = -1;
    assert(vmem_get_pixel(0, 0, &ch) && ch == 'X');

    // A failed format reports the error and retains cached dirty data.
    assert(vmem_put_pixel(0, 0, 'Y'));
    fail_write_sector = BASE_SECTOR + 2;
    assert(!vmem_clear(' '));
    assert(vmem_get_pixel(0, 0, &ch) && ch == 'Y');
    fail_write_sector = -1;
    assert(vmem_flush());
    assert(storage[BASE_SECTOR][0] == 'Y');
    puts("vmem_fb host tests passed");
    return 0;
}
