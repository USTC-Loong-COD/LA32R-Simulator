#ifndef __ARCH_H__
#define __ARCH_H__

#include <bits/stdc++.h>
#include <config.h>

// loongarch reg name
extern const char* reg_name[32];
// loongarch csr name 
extern std::unordered_map<unsigned int, const char*> csr_name;
#define RESET_VECTOR CONFIG_PMEM_BASE
#define REG_SIZE  (32 + 1)

typedef struct {
    uint32_t reg[32];
    uint32_t pc;
    uint32_t csr[512];
    int state;
    uint32_t halt_pc;
    uint32_t inst;
} CPU_State;

#endif // __ARCH_H__