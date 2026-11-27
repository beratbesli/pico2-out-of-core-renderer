CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

ROOT := ..
TARGETS := test_vmem_fb test_sd_csd

.PHONY: all clean

all: $(TARGETS)

test_vmem_fb: test_vmem_fb.c $(ROOT)/pico2_cube_demo/vmem_fb.c $(ROOT)/pico2_cube_demo/vmem_fb.h
	$(CC) $(CFLAGS) -Iinclude -I$(ROOT)/pico2_cube_demo -o $@ test_vmem_fb.c $(ROOT)/pico2_cube_demo/vmem_fb.c

test_sd_csd: test_sd_csd.c $(ROOT)/pico2_cube_demo/sd_csd.c $(ROOT)/pico2_cube_demo/sd_csd.h
	$(CC) $(CFLAGS) -I$(ROOT)/pico2_cube_demo -o $@ test_sd_csd.c $(ROOT)/pico2_cube_demo/sd_csd.c

clean:
	rm -f $(TARGETS)
