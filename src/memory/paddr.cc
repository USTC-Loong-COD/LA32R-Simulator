#include <common.h>
#include <cpu.h>
#include <mmio.h>

uint8_t pmem[CONFIG_PMEM_SIZE];

bool in_pmem(uint32_t addr) {
    return addr >= CONFIG_PMEM_BASE && addr < CONFIG_PMEM_BASE + CONFIG_PMEM_SIZE;
}
uint8_t* addr_convert(uint32_t addr){
    return pmem + addr - CONFIG_PMEM_BASE;
}
uint32_t host_read(uint8_t* p, uint32_t len){
    switch(len){
        case 1: return *p;
        case 2: return *(uint16_t*)p;
        case 4: return *(uint32_t*)p;
        default: assert(0);
    }
}
uint32_t pmem_read(uint32_t addr, uint32_t len) {
    uint8_t* p = addr_convert(addr);
    return host_read(p, len);
}

uint32_t paddr_read(uint32_t addr, uint32_t len) {
    if(!in_pmem(addr)){
#ifndef DEVICE
        // std::cout << "Read: Physical address " << std::hex << addr << " is outside of physical memory!" << std::endl;
        // cpu.state = SIM_ABORT;
        return 0;
#else
        return mmio_read(addr, len);
#endif
    }
    return pmem_read(addr, len);
}

void host_write(uint8_t* p, uint32_t data, uint32_t len){
    switch(len){
        case 1: *p = data; break;
        case 2: *(uint16_t*)p = data; break;
        case 4: *(uint32_t*)p = data; break;
        default: assert(0);
    }
}
void pmem_write(uint32_t addr, uint32_t data, uint32_t len) {
    uint8_t* p = addr_convert(addr);
    host_write(p, data, len);
}
void paddr_write(uint32_t addr, uint32_t data, uint32_t len) {
    if(!in_pmem(addr)){
#ifndef DEVICE
        // std::cout << "Write: Physical address " << std::hex << addr - CONFIG_PMEM_BASE << " is outside of physical memory!" << std::endl;
        // cpu.state = SIM_ABORT;
        return;
#else
        mmio_write(addr, data, len);
        return;
#endif
    }
    pmem_write(addr, data, len);
    return;
}