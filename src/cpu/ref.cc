#include <common.h>
#include <difftest.h>
#include <paddr.h>
#include <cpu.h>
__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {
    if (direction == DIFFTEST_TO_REF) {
        memcpy(addr_convert(addr), buf, n);
    } else {
        memcpy(buf, addr_convert(addr), n);
    }
}
__EXPORT void difftest_regcpy(uint64_t *dut, bool direction) {
    if (direction == DIFFTEST_TO_REF) {
        memcpy(&cpu, dut, REG_SIZE * sizeof(uint64_t));
    } else {
        memcpy(dut, &cpu, REG_SIZE * sizeof(uint64_t));
    }
}

__EXPORT void difftest_exec(uint64_t n) {
    cpu_exec(n);
}

