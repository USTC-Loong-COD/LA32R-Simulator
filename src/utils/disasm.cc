#include <common.h>
#include <arch.h>

using namespace std;

bitset<32> inst_bit;


void disasm(char* buf, uint32_t inst) {
    inst_bit = bitset<32> (inst);

    // register file
    const char *rd = reg_name[BITS(inst, 4, 0)];
    const char *rj = reg_name[BITS(inst, 9, 5)];
    const char *rk = reg_name[BITS(inst, 14, 10)];

    if(inst_bit[31]){
        sprintf(buf, "%-16s", "unimp\t");
        return;
    }
    if(inst_bit[30]){
        switch(BITS(inst, 29, 26)){
            // jirl rd, rj, offs
            case 3: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "jirl", rd, rj, SBITS(inst, 25, 10) << 2); return;
            // b offs
            case 4: sprintf(buf, "%-12s %d\t", "b", (BITS(inst, 25, 10) | (SBITS(inst, 9, 0) << 16)) << 2); return;
            // bl offs
            case 5: sprintf(buf, "%-12s %d\t", "bl", (BITS(inst, 25, 10) | (SBITS(inst, 9, 0) << 16)) << 2); return;
            // beq rj, rd, offs
            case 6: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "beq", rj, rd, SBITS(inst, 25, 10) << 2); return;
            // bne rj, rd, offs
            case 7: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "bne", rj, rd, SBITS(inst, 25, 10) << 2); return;
            // blt rj, rd, offs
            case 8: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "blt", rj, rd, SBITS(inst, 25, 10) << 2); return;
            // bge rj, rd, offs
            case 9: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "bge", rj, rd, SBITS(inst, 25, 10) << 2); return;
            // bltu rj, rd, offs
            case 10: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "bltu", rj, rd, SBITS(inst, 25, 10) << 2); return;
            // bgeu rj, rd, offs
            case 11: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "bgeu", rj, rd, SBITS(inst, 25, 10) << 2); return;
            
            default: sprintf(buf, "%-16s", "unimp\t"); return;
        }
    }
    if(inst_bit[29]){
        if(inst_bit[28]){
            switch(BITS(inst, 27, 15)){
                // dbar hint
                case 0x10e4: sprintf(buf, "%-12s %d\t", "dbar", SBITS(inst, 14, 0)); return;
                // ibar
                case 0x10e5: sprintf(buf, "%-12s %d\t", "ibar", SBITS(inst, 14, 0)); return;

                default: sprintf(buf, "%-16s", "unimp\t"); return;
            }
        }
        else {
            if(!inst_bit[27]){
                // automatic
                sprintf(buf, "%-12s %-4s, %-4s, %d\t", inst_bit[24] ? "sc.w" : "ll.w", rd, rj, SBITS(inst, 23, 10));
            }
            else{
                switch(BITS(inst, 26, 22)){
                    // ld.b rd, rj, si12
                    case 0: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "ld.b", rd, rj, SBITS(inst, 21, 10)); return;
                    // ld.h rd, rj, si12
                    case 1: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "ld.h", rd, rj, SBITS(inst, 21, 10)); return;
                    // ld.w rd, rj, si12
                    case 2: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "ld.w", rd, rj, SBITS(inst, 21, 10)); return;
                    // st.b rd, rj, si12
                    case 4: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "st.b", rd, rj, SBITS(inst, 21, 10)); return;
                    // st.h rd, rj, si12
                    case 5: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "st.h", rd, rj, SBITS(inst, 21, 10)); return;
                    // st.w rd, rj, si12
                    case 6: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "st.w", rd, rj, SBITS(inst, 21, 10)); return;
                    // ld.bu rd, rj, si12
                    case 8: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "ld.bu", rd, rj, SBITS(inst, 21, 10)); return;
                    // ld.hu rd, rj, si12
                    case 9: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "ld.hu", rd, rj, SBITS(inst, 21, 10)); return;
                    // preld hint, rj, si12
                    case 10: sprintf(buf, "%-12s %d, %-4s, %d\t", "preld", BITS(inst, 4, 0), rj, SBITS(inst, 21, 10)); return;

                    default: sprintf(buf, "%-16s", "unimp\t"); return;
                }
            }
        }
    }
    if(inst_bit[28]){
        switch(BITS(inst, 27, 25)){
            // lu12i.w rd, si20
            case 2: sprintf(buf, "%-12s %-4s, %d\t", "lu12i.w", rd, SBITS(inst, 24, 5)); return;
            // lui rd, si20
            case 6: sprintf(buf, "%-12s %-4s, %d\t", "pcaddu12i", rd, SBITS(inst, 24, 5)); return;
            default: sprintf(buf, "%-16s", "unimp\t"); return;
        }
    }
    if(inst_bit[27]){
        // float TODO
        sprintf(buf, "%-16s", "unimp\t");
        return;
    }
    if(inst_bit[26]){
        const char *csr;
        if(csr_name.find(BITS(inst, 19, 10)) != csr_name.end()){
            csr = csr_name[BITS(inst, 19, 10)];
        }
        else{
            sprintf(buf, "%-16s", "unimp\t");
            return;
        }
        if(BITS(inst, 25, 24) == 0){
            switch(BITS(inst, 9, 5)){
                // csrrd rd, csr
                case 0: sprintf(buf, "%-12s %-4s, %s\t", "csrrd", rd, csr); return;
                // csrrw rd, csr
                case 1: sprintf(buf, "%-12s %-4s, %s\t", "csrrw", rd, csr); return;
                // csrxchg rd, rj, csr
                default: sprintf(buf, "%-12s %-4s, %-4s, %s\t", "csrxchg", rd, rj, csr); return;
            }
        }
        else if(BITS(inst, 25, 24) == 2){
            if(BITS(inst, 23, 22) == 0) {
                // cacop code, rj, si12
                sprintf(buf, "%-12s %d, %-4s, %d\t", "cacop", BITS(inst, 4, 0), rj, BITS(inst, 21, 10));
            }
            else if(BITS(inst, 23, 22) == 1){
                if(BITS(inst, 21, 17) != 0x04){
                    sprintf(buf, "%-16s", "unimp\t");
                }
                else{
                    if(BITS(inst, 16, 15) == 3){
                        // invtlb op, rj, rk
                        sprintf(buf, "%-12s %d, %-4s, %-4s\t", "invtlb", BITS(inst, 4, 0), rj, rk);
                    }
                    else if(BITS(inst, 16, 15) == 1){
                        // idle level
                        sprintf(buf, "%-12s %d\t", "idle", BITS(inst, 14, 0));
                    }
                    else if(BITS(inst, 16, 15) == 0){
                        switch(BITS(inst, 14, 10)){
                            case 0xa: {
                                if(BITS(inst, 9, 0) != 0) sprintf(buf, "%-16s", "unimp\t");
                                // tlbsearch
                                else sprintf(buf, "%-12s\t", "tlbsearch");
                            }
                            case 0xb: {
                                if(BITS(inst, 9, 0) != 0) sprintf(buf, "%-16s", "unimp\t");
                                // tlbrd
                                else sprintf(buf, "%-12s\t", "tlbrd");
                            }
                            case 0xc: {
                                if(BITS(inst, 9, 0) != 0) sprintf(buf, "%-16s", "unimp\t");
                                // tlbwr
                                else sprintf(buf, "%-12s\t", "tlbwr");
                            }
                            case 0xd: {
                                if(BITS(inst, 9, 0) != 0) sprintf(buf, "%-16s", "unimp\t");
                                // tlbfill
                                else sprintf(buf, "%-12s\t", "tlbfill");
                            }
                            case 0xe: {
                                if(BITS(inst, 9, 0) != 0) sprintf(buf, "%-16s", "unimp\t");
                                // ertn
                                else sprintf(buf, "%-12s\t", "ertn");
                            }
                            default: sprintf(buf, "%-16s", "unimp\t");
                        }
                    }
                    else {
                        sprintf(buf, "%-16s", "unimp\t");
                    }
                }
            }
            else {
                sprintf(buf, "%-16s", "unimp\t");
            }
        }
        else {
            sprintf(buf, "%-16s", "unimp\t");
        }
    }
    if(inst_bit[25]){
        switch(BITS(inst, 24, 22)){
            // slti rd, rj, si12
            case 0: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "slti", rd, rj, SBITS(inst, 21, 10)); return;
            // sltui rd, rj, si12
            case 1: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "sltui", rd, rj, SBITS(inst, 21, 10)); return;
            // addi.w rd, rj, si12
            case 2: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "addi.w", rd, rj, SBITS(inst, 21, 10)); return;
            // andi rd, rj, si12
            case 5: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "andi", rd, rj, SBITS(inst, 21, 10)); return;
            // ori rd, rj, si12
            case 6: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "ori", rd, rj, SBITS(inst, 21, 10)); return;
            // xori rd, rj, si12
            case 7: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "xori", rd, rj, SBITS(inst, 21, 10)); return;

            default: sprintf(buf, "%-16s", "unimp\t"); return;
        }
    }
    if(inst_bit[24]){
        // float
        sprintf(buf, "%-16s", "unimp\t");
    }
    if(inst_bit[23]){
        // no legal
        sprintf(buf, "%-16s", "unimp\t");
    }
    if(inst_bit[22]){
        switch(BITS(inst, 21, 15)){
            // slli.w rd, rj, ui5
            case 0x1: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "slli.w", rd, rj, SBITS(inst, 14, 10)); return;
            // srli.w rd, rj, ui5
            case 0x9: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "srli.w", rd, rj, SBITS(inst, 14, 10)); return;
            // srai.w rd, rj, ui5
            case 0x11: sprintf(buf, "%-12s %-4s, %-4s, %d\t", "srai.w", rd, rj, SBITS(inst, 14, 10)); return;

            default: sprintf(buf, "%-16s", "unimp\t"); return;
        }
    }
    if(inst_bit[21]){
        switch(BITS(inst, 20, 15)){
            // div.w rd, rj, rk
            case 0x0: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "div.w", rd, rj, rk); return;
            // mod.w rd, rj, rk
            case 0x1: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "mod.w", rd, rj, rk); return;
            // div.wu rd, rj, rk
            case 0x2: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "div.wu", rd, rj, rk); return;
            // mod.wu rd, rj, rk
            case 0x3: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "mod.wu", rd, rj, rk); return;
            // break code
            case 0x14: sprintf(buf, "%-12s %d\t", "break", BITS(inst, 14, 0)); return;
            // syscall code
            case 0x15: sprintf(buf, "%-12s %d\t", "syscall", BITS(inst, 14, 0)); return;

            default: sprintf(buf, "%-16s", "unimp\t"); return;
        }
    }
    if(inst_bit[20]){
        switch(BITS(inst, 19, 15)){
            // add.w rd, rj, rk
            case 0x0: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "add.w", rd, rj, rk); return;
            // sub.w rd, rj, rk
            case 0x2: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "sub.w", rd, rj, rk); return;
            // slt rd, rj, rk
            case 0x4: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "slt", rd, rj, rk); return;
            // sltu rd, rj, rk
            case 0x5: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "sltu", rd, rj, rk); return;
            // nor rd, rj, rk
            case 0x8: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "nor", rd, rj, rk); return;
            // and rd, rj, rk
            case 0x9: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "and", rd, rj, rk); return;
            // or rd, rj, rk
            case 0xa: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "or", rd, rj, rk); return;
            // xor rd, rj, rk
            case 0xb: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "xor", rd, rj, rk); return;
            // sll.w rd, rj, rk
            case 0xe: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "sll.w", rd, rj, rk); return;
            // srl.w rd, rj, rk
            case 0xf: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "srl.w", rd, rj, rk); return;
            // sra.w rd, rj, rk
            case 0x10: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "sra.w", rd, rj, rk); return;
            // mul.w rd, rj, rk
            case 0x18: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "mul.w", rd, rj, rk); return;
            // mulh.w rd, rj, rk
            case 0x19: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "mulh.w", rd, rj, rk); return;
            // mulh.wu rd, rj, rk
            case 0x1a: sprintf(buf, "%-12s %-4s, %-4s, %-4s\t", "mulh.wu", rd, rj, rk); return;

            default: sprintf(buf, "%-16s", "unimp\t"); return;
        }
    }
    if(inst_bit[19] || inst_bit[18] || inst_bit[17] || inst_bit[16] || inst_bit[15]){
        sprintf(buf, "%-16s", "unimp\t");
        return;
    }
    if(BITS(inst, 14, 13) == 3){
        switch(BITS(inst, 12, 10)){
            // rdcntid.w rj or rdcntvl.w rd
            case 0: sprintf(buf, "%-12s %-4s\t", BITS(inst, 4, 0) != 0 ? "rdcntvl.w" : "rdcntid.w", BITS(inst, 4, 0) != 0 ? rd : rj); return;
            // rdcntvh.w rd
            case 1: sprintf(buf, "%-12s %-4s\t", "rdcntvh.w", rd); return;
        }
    }
    else {
        sprintf(buf, "%-16s", "unimp\t");
        return;
    }
}
// int main(){
//     uint32_t inst;
//     char buf[100];
//     ifstream fin("../../test/test", ios::binary);
//     ofstream four("./result");
//     while(!fin.eof()){
//         fin.read((char*)&inst, sizeof(inst));
//         disasm(buf, inst);
//         four << buf << endl;
//     }
//     return 0;
// }
    