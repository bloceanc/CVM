#ifndef _SIMULATOR_INST_H_
#define _SIMULATOR_INST_H_

#include <arpa/inet.h>

#define _INST_LOCAL(inst) (inst)    /* decrypt the instruction */
#define _INST_OPC(inst) ((inst) >> 24)
#define _INST_RA(inst) (((inst) >> 20) & 0xF)
#define _INST_RB(inst) (((inst) >> 16) & 0xF)
#define _INST_RC(inst) (((inst) >> 12) & 0xF)
#define _INST_CX_12(inst) ((inst) & 0xFFF)
#define _INST_CX_16(inst) ((inst) & 0xFFFF)
#define _INST_CX_24(inst) ((inst) & 0xFFFFFF)

/* instructions */
#define OPC_NOP 0x00
#define OPC_LD 0x01
#define OPC_ST 0x02
#define OPC_LB 0x03
#define OPC_LBU 0x04
#define OPC_SB 0x05
#define OPC_LH 0x06
#define OPC_LHU 0x07
#define OPC_SH 0x08
#define OPC_ADDIU 0x09
#define OPC_MOVZ 0x0A
#define OPC_MOVN 0x0B
#define OPC_ANDI 0x0C
#define OPC_ORI 0x0D
#define OPC_XORI 0x0E
#define OPC_LUI 0x0F
#define OPC_CMP 0x10
#define OPC_ADDU 0x11
#define OPC_SUBU 0x12
#define OPC_ADD 0x13
#define OPC_SUB 0x14
#define OPC_CLZ 0x15
#define OPC_CLO 0x16
#define OPC_MUL 0x17
#define OPC_AND 0x18
#define OPC_OR 0x19
#define OPC_XOR 0x1A
#define OPC_ROLI 0x1B
#define OPC_RORI 0x1C
#define OPC_SRA 0x1D
#define OPC_SHL 0x1E
#define OPC_SHR 0x1F
#define OPC_SRAV 0x20
#define OPC_SHLV 0x21
#define OPC_SHRV 0x22
#define OPC_ROL 0x23
#define OPC_ROR 0x24
#define OPC_JEQ 0x30
#define OPC_JNE 0x31
#define OPC_JLT 0x32
#define OPC_JGT 0x33
#define OPC_JLE 0x34
#define OPC_JGE 0x35
#define OPC_JMP 0x36
#define OPC_JALR 0x39
#define OPC_JSUB 0x3B
#define OPC_JR_RET 0x3C
#define OPC_MULT 0x41
#define OPC_MULTU 0x42
#define OPC_DIV 0x43
#define OPC_DIVU 0x44
#define OPC_MFHI 0x46
#define OPC_MFLO 0x47
#define OPC_MTHI 0x48
#define OPC_MTLO 0x49
#define OPC_MFC0 0x50
#define OPC_MTC0 0x51
#define OPC_C0MOV 0x52
#define OPC_SLTI 0x26
#define OPC_SLTIU 0x27
#define OPC_SLT 0x28
#define OPC_SLTU 0x29
#define OPC_BEQ 0x37
#define OPC_BNE 0x38
#define OPC_BAL 0x3A
#define OPC_SSC 0x53

#endif /* _SIMULATOR_INST_H_ */

