# Pico 2 out-of-core renderer

This Raspberry Pi Pico 2 (RP2350) experiment renders a large ASCII 3D scene
through a 32-slot LRU page cache. The virtual canvas is 1,000 × 1,000 bytes,
while only 16 KiB of framebuffer pages stay in RAM. Dirty pages are stored on a
MicroSD card through SPI0.

This is a hardware experiment, not a general-purpose filesystem or swap
implementation. The SD card region is addressed as raw sectors.

> [!CAUTION]
> The demo writes sectors starting at LBA 4096 and formats 1,954 sectors after
> explicit serial confirmation. Use a dedicated, disposable test card or a
> reserved raw region. Never point it at a card containing a filesystem or
> irreplaceable data.

## Hardware

| MicroSD signal | Pico 2 pin | Function |
|---|---:|---|
| MISO | GP16 | SPI0 RX |
| CS | GP17 | Chip select |
| SCK | GP18 | SPI0 clock |
| MOSI | GP19 | SPI0 TX |
| VCC | 3V3(OUT) | 3.3 V supply |
| GND | GND | Common ground |

The driver supports SDSC and SDHC cards. The card must have at least
`4096 + 1954` addressable sectors. The demo does not create or preserve a
filesystem in the reserved region.

## Build

Install CMake, a C compiler, and the ARM GNU toolchain, then use the pinned
Pico SDK release:

```bash
cmake -S pico2_cube_demo -B build \
  -DPICO_BOARD=pico2 \
  -DPICO_SDK_FETCH_FROM_GIT=ON \
  -DPICO_SDK_FETCH_FROM_GIT_TAG=2.1.1
cmake --build build --parallel
```

The generated `build/pico2_cube_demo.uf2` can be copied with `picotool` or
dragged to the board's BOOTSEL volume. Do not commit generated binaries.

## Run safely

1. Confirm wiring and insert a dedicated test card.
2. Flash the UF2 and connect the USB CDC serial console at 115200 baud.
3. Read the capacity and raw-sector warning printed by the firmware.
4. Type exactly `ERASE` followed by Enter only after verifying the card and
   reserved region.

Without that confirmation the firmware refuses to format or generate the
world. After generation, the viewport continuously reads the SD-backed canvas.

## Tests

The host test replaces the SD and Pico timing APIs with an in-memory sector
store. It verifies page mapping, cache hits, dirty eviction, flush behavior,
and out-of-bounds reads without requiring a board or SD card:

```bash
make -C tests
./tests/test_vmem_fb
```

GitHub Actions runs this host test and a pinned Pico SDK cross-build.

## License

[MIT](LICENSE)
