#include <mmio.h>
#include <common.h>

static void serial_io_handler(void *base, uint32_t offset, uint32_t len, bool is_write) {
    assert(len == 1);
    assert(offset == 0);
    assert(is_write);
    putchar(*(char *)base);
}

void init_serial(){
    register_io_map("serial", CONFIG_SERIAL_MMIO, 8, new uint8_t[8], serial_io_handler);
}