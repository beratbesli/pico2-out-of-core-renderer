CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

ROOT := ..
TARGET := test_vmem_fb

.PHONY: all clean

all: $(TARGET)

$(TARGET): test_vmem_fb.c $(ROOT)/pico2_cube_demo/vmem_fb.c $(ROOT)/pico2_cube_demo/vmem_fb.h
	$(CC) $(CFLAGS) -Iinclude -I$(ROOT)/pico2_cube_demo -o $@ test_vmem_fb.c $(ROOT)/pico2_cube_demo/vmem_fb.c

clean:
	rm -f $(TARGET)
