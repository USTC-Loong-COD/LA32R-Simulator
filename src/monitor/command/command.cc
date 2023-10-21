#include <common.h>
#include <cpu.h>

void command_mainloop(){
    cpu_exec(-1);
    return;
}