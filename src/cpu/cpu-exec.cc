#include <common.h>
#include <paddr.h>
#include <cpu.h>
#include <decode.h>
#include <disasm.h>
#include <device.h>

#ifdef ITRACE
typedef struct{
    uint32_t pc;
    uint32_t inst;
}inst_log_t;

#define ILOG_SIZE 16
static inst_log_t ilog[ILOG_SIZE];
static int ilog_idx = 0;
void print_ilog(){
    for(int i = 0; i < ILOG_SIZE; i++){
        std::cout << std::hex << std::setw(8) << std::setfill('0') << ilog[(ilog_idx+i)%ILOG_SIZE].pc << ": ";
        std::cout << std::hex << std::setw(8) << std::setfill('0') << ilog[(ilog_idx+i)%ILOG_SIZE].inst << '\t';
        char buf[100];
        disasm(buf, ilog[(ilog_idx+i)%ILOG_SIZE].inst);
        std::cout << buf << std::endl;
    }
}
#endif

void reset(){
    cpu.state = SIM_STOP;
    cpu.pc = RESET_VECTOR;
}

void cpu_exec(uint64_t n){
#ifndef CONFIG_REF
    switch(cpu.state){
        case SIM_ABORT: case SIM_END:
        std::cout << "Program execution has ended." << std::endl;
        return;
        default: cpu.state = SIM_RUNNING;
    }
#endif
    while(n--){
        uint32_t inst = inst_fetch(cpu.pc);
        
#ifdef ITRACE
        ilog[ilog_idx].pc = cpu.pc;
        ilog[ilog_idx].inst = inst;
        ilog_idx = (ilog_idx + 1) % ILOG_SIZE;
        // std::cout << "pc = " << std::hex << cpu.pc << '\t';
        // char buf[100];
        // disasm(buf, inst);
        // std::cout << buf << std::endl;
#endif
        decode_exec(inst);
// #ifdef DEVICE
//         if(cpu.state == SIM_RUNNING) device_update();
// #endif
        if(cpu.state != SIM_RUNNING) break;
    }
#ifndef CONFIG_REF
#ifdef ITRACE
    print_ilog();
#endif
    switch(cpu.state){
        case SIM_END: 
        Log("simulation %s at pc = " FMT_WORD, (cpu.reg[4] == 0 ? INLINE_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) : 
            INLINE_FMT("HIT BAD TRAP", ANSI_FG_RED)), cpu.halt_pc); break;
        case SIM_ABORT:
        Log("simulation %s at pc = " FMT_WORD, INLINE_FMT("ABORT", ANSI_FG_RED), cpu.halt_pc); break;
        case SIM_STOP:
        Log("simulation %s at pc = " FMT_WORD, INLINE_FMT("STOP", ANSI_FG_YELLOW), cpu.halt_pc); break;
        break;
    }
#endif
}

