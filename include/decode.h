#ifndef __DECODE_H__
#define __DECODE_H__

#include <common.h>
uint32_t inst_fetch(uint32_t pc);
void decode_exec(uint32_t inst);
void cpu_exec(uint64_t n);
#endif
