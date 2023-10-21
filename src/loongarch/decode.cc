#include <cpu.h>
#include <common.h>
#include <paddr.h>

#define R(i) cpu.reg[i]

#define INST_MATCH(opcode, mask, type, name, ...) {\
    if(((opcode ^ inst) & mask) == 0){ \
       decode_oprand(inst, type, rd, src1, src2, dst, imm); \
        __VA_ARGS__ ; \
       goto finish; \
    } \
} 
enum {
    TYPE_2R, TYPE_3R, TYPE_4R, 
    TYPE_2RI8, TYPE_2RI12, TYPE_2RI14, TYPE_2RI16, TYPE_1RI21, 
    TYPE_I26
};
void decode_oprand(uint32_t inst, int type, uint32_t &rd, uint32_t &src1, uint32_t &src2, uint32_t &dst, uint32_t &imm){
    int rj = BITS(inst, 9, 5);
    int rk = BITS(inst, 14, 10);
    // int ra = BITS(inst, 19, 15);
    rd = BITS(inst, 4, 0);

    src1 = R(rj);
    src2 = R(rk);
    dst = R(rd);

    switch(type){
        case TYPE_2R:    imm = 0; break;
        case TYPE_3R:    imm = 0; break;
        case TYPE_4R:    imm = 0; break;
        case TYPE_2RI8:  imm = SBITS(inst, 17, 10); break;
        case TYPE_2RI12: imm = SBITS(inst, 21, 10); break;
        case TYPE_2RI14: imm = SBITS(inst, 23, 10); break;
        case TYPE_2RI16: imm = SBITS(inst, 25, 10); break;
        case TYPE_1RI21: imm = BITS(inst, 25, 10) | (SBITS(inst, 4, 0) << 16); break;
        case TYPE_I26:   imm = BITS(inst, 25, 10) | (SBITS(inst, 9, 0) << 16); break;
        default:         imm = 0; break;
    }

}

void decode_exec(uint32_t inst){
    uint32_t rd = 0;
    uint32_t src1 = 0;
    uint32_t src2 = 0;
    uint32_t imm = 0;
    uint32_t dst = 0;
    uint32_t npc = cpu.pc + 4;
    INST_MATCH(0x00100000, 0xffff8000, TYPE_3R,   ADD.W,        R(rd) = src1 + src2) 
    INST_MATCH(0x00110000, 0xffff8000, TYPE_3R,   SUB.W,        R(rd) = src1 - src2) 
    INST_MATCH(0x00120000, 0xffff8000, TYPE_3R,   SLT,          R(rd) = (int32_t)src1 < (int32_t)src2)
    INST_MATCH(0x00128000, 0xffff8000, TYPE_3R,   SLTU,         R(rd) = src1 < src2)
    INST_MATCH(0x00140000, 0xffff8000, TYPE_3R,   NOR,          R(rd) = ~(src1 | src2))
    INST_MATCH(0x00148000, 0xffff8000, TYPE_3R,   AND,          R(rd) = src1 & src2)
    INST_MATCH(0x00150000, 0xffff8000, TYPE_3R,   OR,           R(rd) = src1 | src2)
    INST_MATCH(0x00158000, 0xffff8000, TYPE_3R,   XOR,          R(rd) = src1 ^ src2)
    INST_MATCH(0x00170000, 0xffff8000, TYPE_3R,   SLL.W,        R(rd) = src1 << src2)
    INST_MATCH(0x00178000, 0xffff8000, TYPE_3R,   SRL.W,        R(rd) = src1 >> src2)
    INST_MATCH(0x00180000, 0xffff8000, TYPE_3R,   SRA.W,        R(rd) = (int32_t)src1 >> src2)
    INST_MATCH(0x001c0000, 0xffff8000, TYPE_3R,   MUL.W,        R(rd) = (int32_t)src1 * (int32_t)src2)
    INST_MATCH(0x001c8000, 0xffff8000, TYPE_3R,   MULH.W,       R(rd) = ((int64_t)(int32_t)src1 * (int64_t)(int32_t)src2) >> 32)
    INST_MATCH(0x001d0000, 0xffff8000, TYPE_3R,   MULH.WU,      R(rd) = ((uint64_t)src1 * (uint64_t)src2) >> 32)
    INST_MATCH(0x00200000, 0xffff8000, TYPE_3R,   DIV.W,        R(rd) = (int32_t)src1 / (int32_t)src2)
    INST_MATCH(0x00208000, 0xffff8000, TYPE_3R,   MOD.W,        R(rd) = (int32_t)src1 % (int32_t)src2)
    INST_MATCH(0x00210000, 0xffff8000, TYPE_3R,   DIV.WU,       R(rd) = src1 / src2)
    INST_MATCH(0x00218000, 0xffff8000, TYPE_3R,   MOD.WU,       R(rd) = src1 % src2)
    INST_MATCH(0x002a0000, 0xffff8000, TYPE_3R,   BREAK,        cpu.state = SIM_END; cpu.halt_pc = cpu.pc)
    // INST_MATCH(0x002b0000, 0xffff8000, SYSCALL, TYPE_3R, )
    INST_MATCH(0x00408000, 0xffff8000, TYPE_2RI8,  SLLI.W,      R(rd) = src1 << BITS(imm, 4, 0))
    INST_MATCH(0x00448000, 0xffff8000, TYPE_2RI8,  SRLI.W,      R(rd) = src1 >> BITS(imm, 4, 0))
    INST_MATCH(0x00488000, 0xffff8000, TYPE_2RI8,  SRAI.W,      R(rd) = (int32_t)src1 >> BITS(imm, 4, 0))
    INST_MATCH(0x02000000, 0xffc00000, TYPE_2RI12, SLTI,        R(rd) = (int32_t)src1 < (int32_t)imm)
    INST_MATCH(0x02400000, 0xffc00000, TYPE_2RI12, SLTUI,       R(rd) = src1 < imm)
    INST_MATCH(0x02800000, 0xffc00000, TYPE_2RI12, ADDI.W,      R(rd) = src1 + imm)
    INST_MATCH(0x03400000, 0xffc00000, TYPE_2RI12, ANDI,        R(rd) = src1 & BITS(imm, 11, 0))
    INST_MATCH(0x03800000, 0xffc00000, TYPE_2RI12, ORI,         R(rd) = src1 | BITS(imm, 11, 0))
    INST_MATCH(0x03c00000, 0xffc00000, TYPE_2RI12, XORI,        R(rd) = src1 ^ BITS(imm, 11, 0)) 
    INST_MATCH(0x14000000, 0xfe000000, TYPE_1RI21, LU12I.W,     R(rd) = BITS(inst, 24, 5) << 12)
    INST_MATCH(0x1c000000, 0xfe000000, TYPE_1RI21, PCADDU12I,   R(rd) = cpu.pc + (BITS(inst, 24, 5) << 12))
    INST_MATCH(0x28000000, 0xffc00000, TYPE_2RI12, LD.B,        R(rd) = SBITS(paddr_read(src1 + imm, 1), 7, 0))
    INST_MATCH(0x28400000, 0xffc00000, TYPE_2RI12, LD.H,        R(rd) = SBITS(paddr_read(src1 + imm, 2), 15, 0))
    INST_MATCH(0x28800000, 0xffc00000, TYPE_2RI12, LD.W,        R(rd) = paddr_read(src1 + imm, 4);)
    INST_MATCH(0x29000000, 0xffc00000, TYPE_2RI12, ST.B,        paddr_write(src1 + imm, BITS(dst, 7, 0), 1))
    INST_MATCH(0x29400000, 0xffc00000, TYPE_2RI12, ST.H,        paddr_write(src1 + imm, BITS(dst, 15, 0), 2))
    INST_MATCH(0x29800000, 0xffc00000, TYPE_2RI12, ST.W,        paddr_write(src1 + imm, dst, 4)) 
    INST_MATCH(0x2a000000, 0xffc00000, TYPE_2RI12, LD.BU,       R(rd) = BITS(paddr_read(src1 + imm, 1), 7, 0))
    INST_MATCH(0x2a400000, 0xffc00000, TYPE_2RI12, LD.HU,       R(rd) = BITS(paddr_read(src1 + imm, 2), 15, 0))
    // PRELD
    // INST_MATCH(0x2a800000, 0xffc00000, PRELD, TYPE_2RI12, )
    // DBAR
    // INST_MATCH(0x2b000000, 0xffc00000, DBAR, TYPE_2RI12, )
    // IBAR
    // INST_MATCH(0x2b400000, 0xffc00000, IBAR, TYPE_2RI12, ) 
    INST_MATCH(0x4c000000, 0xfc000000, TYPE_2RI16, JIRL,        R(rd) = cpu.pc + 4; npc = src1 + (imm << 2))
    INST_MATCH(0x50000000, 0xfc000000, TYPE_I26,   B,           npc = cpu.pc + (imm << 2))
    INST_MATCH(0x54000000, 0xfc000000, TYPE_I26,   BL,          R(1) = cpu.pc + 4; npc = cpu.pc + (imm << 2))
    INST_MATCH(0x58000000, 0xfc000000, TYPE_2RI16, BEQ,         if(src1 == dst) npc = cpu.pc + (imm << 2))
    INST_MATCH(0x5c000000, 0xfc000000, TYPE_2RI16, BNE,         if(src1 != dst) npc = cpu.pc + (imm << 2))
    INST_MATCH(0x60000000, 0xfc000000, TYPE_2RI16, BLT,         if((int32_t)src1 < (int32_t)dst) npc = cpu.pc + (imm << 2))
    INST_MATCH(0x64000000, 0xfc000000, TYPE_2RI16, BGE,         if((int32_t)src1 >= (int32_t)dst) npc = cpu.pc + (imm << 2))
    INST_MATCH(0x68000000, 0xfc000000, TYPE_2RI16, BLTU,        if(src1 < dst) npc = cpu.pc + (imm << 2))
    INST_MATCH(0x6c000000, 0xfc000000, TYPE_2RI16, BGEU,        if(src1 >= dst) npc = cpu.pc + (imm << 2))

    

finish:
    R(0) = 0;
    cpu.pc = npc;
    return;
}

uint32_t inst_fetch(uint32_t pc){
    return paddr_read(pc, 4);
}
