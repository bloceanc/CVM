#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "cpu.h"
#include "inst.h"
#include "vm.h"
#include "api/api.h"

#define ROL32(x, n) ((unsigned int)(x) << (unsigned int)(n) | (unsigned int)(x) >> (32 - (unsigned int)(n)))
#define ROR32(x, n) ((unsigned int)(x) >> (unsigned int)(n) | (unsigned int)(x) << (32 - (unsigned int)(n)))

s32_t run_loop(vm_t *vm, u32_t tid)
{
    thread_ctx_t *thread = &vm->threads[tid];
    s8_t *base = vm->mem.start;
    s8_t *sp_end = NULL;

    cpu_t   *cpu = &vm->cpu;
    cpu_core_t *cc = &cpu->core;
    cpu_cop_t *cp = &cpu->cp;

    REG_PC(cp) = (u32_t)(unsigned long)thread->entry;
    REG_SP(cc) = (u32_t)(unsigned long)thread->sp;  /* initialize thread stack register! */

    sp_end = thread->sp_end;  /* for safety */

    while (1) {
        inst_t inst = _INST_LOCAL(*(inst_t *)(REG_PC(cp) + base));
        REG_PC(cp) += sizeof(inst);  /* move PC = PC + 4 pointer to next instrument */

        u32_t   op = _INST_OPC(inst);
        u32_t   ra = _INST_RA(inst);
        u32_t   rb = _INST_RB(inst);
        u32_t   rc = _INST_RC(inst);
        s32_t   c12 = (_INST_CX_12(inst) & (1<<11)) ? _INST_CX_12(inst) | 0xFFFFF000: _INST_CX_12(inst);
        s32_t   c16 = (_INST_CX_16(inst) & (1<<15)) ? _INST_CX_16(inst) | 0xFFFF0000: _INST_CX_16(inst);
        s32_t   c24 = (_INST_CX_24(inst) & (1<<23)) ? _INST_CX_24(inst) | 0xFF000000: _INST_CX_24(inst);

        if ((u32_t)REG_SP(cc) <= (u32_t)(unsigned long)sp_end) {
            printf("stack overflow\n");
            return -3;
        }

        switch (op) {
            case OPC_NOP:
            break;
            case OPC_LD:
                REG_GPR(cc, ra) = *(u32_t *)(base + REG_GPR(cc, rb) + c16);
                break;
            case OPC_ST:
                *(u32_t *)(base + REG_GPR(cc, rb) + c16) = REG_GPR(cc, ra);
                break;
            case OPC_LB: {
                    s8_t t = *(s8_t *)(base + REG_GPR(cc, rb) + c16);
                    REG_GPR(cc, ra) = t;
                }
                break;
            case OPC_LBU: {
                    u8_t t = *(u8_t *)(base + REG_GPR(cc, rb) + c16);
                    REG_GPR(cc, ra) = t;
                }
                break;
            case OPC_SB:
                *(u8_t *)(base + REG_GPR(cc, rb) + c16) = REG_GPR(cc, ra) & 0xFF;
                break;
            case OPC_LH:
                REG_GPR(cc, ra) = *(s16_t *)(base + REG_GPR(cc, rb) + c16);
                break;
            case OPC_LHU: {
                    u16_t t = *(u16_t *)(base + REG_GPR(cc, rb) + c16);
                    REG_GPR(cc, ra) = t;
                }
                break;
            case OPC_SH:
                *(u16_t *)(base + REG_GPR(cc, rb) + c16) = REG_GPR(cc, ra) & 0xFFFF;
                break;
            case OPC_ADDIU:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) + c16;
                break;
	    case OPC_MOVZ:
                if (REG_GPR(cc, rc) == 0) REG_GPR(cc, ra) = REG_GPR(cc, rb);
                break;
	    case OPC_MOVN:
                if (REG_GPR(cc, rc) != 0) REG_GPR(cc, ra) = REG_GPR(cc, rb);
                break;
            case OPC_ANDI:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) & (c16 & 0xFFFF);
                break;
            case OPC_ORI:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) | (c16 & 0xFFFF);
                break;
            case OPC_XORI:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) ^ (c16 & 0xFFFF);
                break;
            case OPC_LUI:
                REG_GPR(cc, ra) = (c16 & 0xFFFF) << 16;
                break;
            case OPC_SLTI:
                if ((s32_t)REG_GPR(cc, rb) < (s32_t)c16) 
                    REG_GPR(cc, ra) = 1;
                else REG_GPR(cc, ra) = 0;
                break;
            case OPC_SLTIU:
                if ((u32_t)REG_GPR(cc, rb) < (u32_t)c16)
                    REG_GPR(cc, ra) = 1;
                else REG_GPR(cc, ra) = 0;
                break;
            case OPC_SLT:
                if ((s32_t)REG_GPR(cc, rb) < (s32_t)REG_GPR(cc, rc))
                    REG_GPR(cc, ra) = 1;
                else REG_GPR(cc, ra) = 0;
                break;
            case OPC_SLTU:
                if ((u32_t)REG_GPR(cc, rb) < (u32_t)REG_GPR(cc, rc))
                    REG_GPR(cc, ra) = 1;
                else REG_GPR(cc, ra) = 0;
                break;
            case OPC_BEQ:
                if (REG_GPR(cc, ra) == REG_GPR(cc, rb))
                    REG_PC(cp) += c16;
                break;
            case OPC_BNE:
                if (REG_GPR(cc, ra) != REG_GPR(cc, rb))
                    REG_PC(cp) += c16;
                break;
            case OPC_BAL:
                REG_LR(cc) = REG_PC(cp);
                REG_PC(cp) += c24;
                thread->prev_sp = REG_SP(cc);
                break;
            case OPC_CMP: {
                s32_t t = (s32_t)REG_GPR(cc, ra) - (s32_t)REG_GPR(cc, rb);
                if (t > 0) {
                    REG_SW_CLEAR_N(cc);
                    REG_SW_CLEAR_Z(cc);
                }
                if (t == 0) {
                    REG_SW_CLEAR_N(cc);
                    REG_SW_SET_Z(cc);
                }
                if (t < 0) {
                    REG_SW_SET_N(cc);
                    REG_SW_CLEAR_Z(cc);
                }
                break;
            }
            case OPC_ADD: {
                ull_t t = REG_GPR(cc, rb) + REG_GPR(cc, rc);
                if (t > 0xFFFFFFFF) {
                    REG_SW_SET_V(cc);
                }
                REG_GPR(cc, ra) = t & 0xFFFFFFFF;
                break;
            }
            case OPC_ADDU:
                *(u32_t *)&REG_GPR(cc, ra) = (u32_t)REG_GPR(cc, rb) + (u32_t)REG_GPR(cc, rc);
                break;
            case OPC_SUB:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) - REG_GPR(cc, rc);
                break;
            case OPC_CLZ: {
                u32_t i, n;
                n = REG_GPR(cc, rb);
                for (i = 0; (i < 32) && (n & 0x80000000 == 0); i++) {
                    n <<= 1;
                }
                REG_GPR(cc, ra) = i;
                break;
            }
            case OPC_CLO: {
                u32_t i, n;
                n = REG_GPR(cc, rb);
                for (i = 0; (i < 32) && (n & 0x80000000 == 0x80000000); i++) {
                    n <<= 1;
                }
                REG_GPR(cc, ra) = i;
                break;
            }
            case OPC_SUBU:
                *(u32_t *)&REG_GPR(cc, ra) = (u32_t)REG_GPR(cc, rb) - (u32_t)REG_GPR(cc, rc);
                break;
            case OPC_MUL: {
                ull_t t = REG_GPR(cc, rb) * REG_GPR(cc, rc);
                if (t > 0xFFFFFFFF) {
                    REG_SW_SET_V(cc);
                }
                REG_GPR(cc, ra) = t & 0xFFFFFFFF;
                break;
            }
            case OPC_AND:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) & REG_GPR(cc, rc);
                break;
            case OPC_OR:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) | REG_GPR(cc, rc);
                break;
            case OPC_XOR:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) ^ REG_GPR(cc, rc);
                break;
            case OPC_ROLI:
                REG_GPR(cc, ra) = ROL32(REG_GPR(cc, rb), (c12 & 0xFFF));
                break;
            case OPC_RORI:
                REG_GPR(cc, ra) = ROR32(REG_GPR(cc, rb), (c12 & 0xFFF));
                break;
            case OPC_SRA: // shift filled with signed bit
                REG_GPR(cc, ra) = (REG_GPR(cc, rb) & 0x80000000) | (REG_GPR(cc, rb) >> (c12 & 0xFFF));
                break;
            case OPC_SHL:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) << (c12 & 0xFFF);
                break;
            case OPC_SHR:
                REG_GPR(cc, ra) = ((unsigned int)REG_GPR(cc, rb)) >> (c12 & 0xFFF);
                break;
            case OPC_SRAV: // shift filled with signed bit
                REG_GPR(cc, ra) = (REG_GPR(cc, rb) & 0x80000000) | (REG_GPR(cc, rb) >> REG_GPR(cc, rc));
                break;
            case OPC_SHLV:
                REG_GPR(cc, ra) = REG_GPR(cc, rb) << REG_GPR(cc, rc);
                break;
            case OPC_SHRV:
                REG_GPR(cc, ra) = ((unsigned int)REG_GPR(cc, rb)) >> REG_GPR(cc, rc);
                break;
            case OPC_ROL:
                REG_GPR(cc, ra) = ROL32(REG_GPR(cc, rb), REG_GPR(cc, rc));
                break;
            case OPC_ROR:
                REG_GPR(cc, ra) = ROR32(REG_GPR(cc, rb), REG_GPR(cc, rc));
                break;
            case OPC_JEQ:
                if (!REG_SW_GET_N(cc) && REG_SW_GET_Z(cc)) {
                    REG_PC(cp) += c24;
                }
                break;
            case OPC_JNE:
                if (!REG_SW_GET_Z(cc)) {
                    REG_PC(cp) += c24;
                }
                break;
            case OPC_JLT:
                if (REG_SW_GET_N(cc) && !REG_SW_GET_Z(cc)) {
                    REG_PC(cp) += c24;
                }
                break;
            case OPC_JGT:
                if (!REG_SW_GET_N(cc) && !REG_SW_GET_Z(cc)) {
                    REG_PC(cp) += c24;
                }
                break;
            case OPC_JLE:
                if (REG_SW_GET_N(cc) || REG_SW_GET_Z(cc)) {
                    REG_PC(cp) += c24;
                }
                break;
            case OPC_JGE:
                if (!REG_SW_GET_N(cc) || REG_SW_GET_Z(cc)) {
                    REG_PC(cp) += c24;
                }
                break;
            case OPC_JMP:
                REG_PC(cp) += c24;
                break;
            case OPC_JALR:
                REG_LR(cc) = REG_PC(cp);
                REG_PC(cp) = REG_GPR(cc, rb);
                thread->prev_sp = REG_SP(cc);
                break;
            case OPC_JSUB:
                REG_LR(cc) = REG_PC(cp);
                REG_PC(cp) += c24;
                thread->prev_sp = REG_SP(cc);
                break;
            case OPC_JR_RET:
                REG_PC(cp) = REG_LR(cc);
                break;
            case OPC_MULT: {
                ll_t t = (s32_t)REG_GPR(cc, ra) * (s32_t)REG_GPR(cc, rb);
                REG_HI(cp) = t >> 32;
                REG_LO(cp) = t & 0xFFFFFFFF;
                break;
            }
            case OPC_MULTU: {
                ull_t t = (u32_t)REG_GPR(cc, ra) * (u32_t)REG_GPR(cc, rb);
                *(u32_t*)&REG_HI(cp) = t >> 32;
                *(u32_t*)&REG_LO(cp) = t & 0xFFFFFFFF;
                break;
            }
            case OPC_DIV:
                REG_HI(cp) = (s32_t)REG_GPR(cc, ra) % (s32_t)REG_GPR(cc, rb);
                REG_LO(cp) = (s32_t)REG_GPR(cc, ra) / (s32_t)REG_GPR(cc, rb);
                break;
            case OPC_DIVU:
                *(u32_t *)&REG_HI(cp) = (u32_t)REG_GPR(cc, ra) % (u32_t)REG_GPR(cc, rb);
                *(u32_t *)&REG_LO(cp) = (u32_t)REG_GPR(cc, ra) / (u32_t)REG_GPR(cc, rb);
                break;
            case OPC_MFHI:
                REG_GPR(cc, ra) = REG_HI(cp);
                break;
            case OPC_MFLO:
                REG_GPR(cc, ra) = REG_LO(cp);
                break;
            case OPC_MTHI:
                REG_HI(cp) = REG_GPR(cc, ra);
                break;
            case OPC_MTLO:
                REG_LO(cp) = REG_GPR(cc, rb);
                break;
            case OPC_MFC0:
                REG_GPR(cc, ra) = REG_C0R(cp, rb);
                break;
            case OPC_MTC0:
                REG_C0R(cp, ra) = REG_GPR(cc, rb);
                break;
            case OPC_C0MOV:
                REG_C0R(cp, ra) = REG_C0R(cc, rb);
                break;
            case OPC_SSC:
                /* sys call */
                api_func[c16](vm, tid);
                break;
            default:
                printf("unknonw instrument: %x at %x\n", op, REG_PC(cp));
                break;
        }

        if (!REG_PC(cp)) {
            return REG_GPR(cc, 2);
        }
    }

    return -1;
}

