#ifndef __MMIO_H__
#define __MMIO_H__

#define CONFIG_SERIAL_MMIO          0xa00003f8
#define CONFIG_RTC_MMIO             0xa0000040
#define CONFIG_KEYBOARD_DATA_MMIO   0xa0000060

#define CONFIG_VGA_CTL_MMIO         0xa0000100
#define CONFIG_FB_ADDR              0xa1000000
#define CONFIG_FFB_ADDR             0xa2000000

#include <common.h>

typedef void (*io_handler_t)(void* base, uint32_t offset, uint32_t len, bool is_write);

typedef struct {
    const char *name;
    // paddr_t low;
    // paddr_t high;
    void *base;
    io_handler_t handler;
} io_map_t;

void register_io_map(const char* name, paddr_t addr, paddr_t len, void* base, io_handler_t handler);
word_t mmio_read(paddr_t addr, uint32_t len);
void mmio_write(paddr_t addr, word_t data, uint32_t len);
#endif