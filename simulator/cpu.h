#ifndef _SIMULATOR_CPU_H_
#define _SIMULATOR_CPU_H_

#define GPR_COUNT   16
#define CPR_COUNT   4


typedef struct cpu_core_s {
    reg_t       regs[GPR_COUNT];
    reg_t       *gp;
    reg_t       *fp;
    reg_t       *sp;
    reg_t       *lr;
    reg_t       *sw;
} cpu_core_t;

typedef struct cpu_coprocessor_s {
    reg_t       regs[CPR_COUNT];
    reg_t       *pc;
    reg_t       *epc;
    reg_t       *hi;
    reg_t       *lo;
} cpu_cop_t;

typedef struct cpu_s {
    cpu_core_t  core;
    cpu_cop_t   cp;
} cpu_t;


#define REG_PC(cp) (*((cp)->pc))
#define REG_EPC(cp) (*((cp)->epc))
#define REG_HI(cp) (*((cp)->hi))
#define REG_LO(cp) (*((cp)->lo))
#define REG_C0R(cp, n) ((cp)->regs[n])

#define REG_GP(cc) (*((cc)->gp))
#define REG_FP(cc) (*((cc)->fp))
#define REG_SP(cc) (*((cc)->sp))
#define REG_LR(cc) (*((cc)->lr))
#define REG_SW(cc) (*((cc)->sw))
#define REG_GPR(cc, n) ((cc)->regs[n])


#define REG_SW_SHIFT_N  0
#define REG_SW_SHIFT_Z  1
#define REG_SW_SHIFT_C  2
#define REG_SW_SHIFT_V  3
#define REG_SW_SHIFT_D  5
#define REG_SW_SHIFT_M  6
#define REG_SW_SHIFT_I  13
#define REG_SW_GET_N(cc)    ((*(cc->sw)) & (1 << REG_SW_SHIFT_N))
#define REG_SW_GET_Z(cc)    ((*(cc->sw)) & (1 << REG_SW_SHIFT_Z))
#define REG_SW_GET_C(cc)    ((*(cc->sw)) & (1 << REG_SW_SHIFT_C))
#define REG_SW_GET_V(cc)    ((*(cc->sw)) & (1 << REG_SW_SHIFT_V))
#define REG_SW_SET_N(cc)    ((*(cc->sw)) |= (1 << REG_SW_SHIFT_N))
#define REG_SW_SET_Z(cc)    ((*(cc->sw)) |= (1 << REG_SW_SHIFT_Z))
#define REG_SW_SET_C(cc)    ((*(cc->sw)) |= (1 << REG_SW_SHIFT_C))
#define REG_SW_SET_V(cc)    ((*(cc->sw)) |= (1 << REG_SW_SHIFT_V))
#define REG_SW_CLEAR_N(cc)  ((*(cc->sw)) &=~(1 << REG_SW_SHIFT_N))
#define REG_SW_CLEAR_Z(cc)  ((*(cc->sw)) &=~(1 << REG_SW_SHIFT_Z))
#define REG_SW_CLEAR_C(cc)  ((*(cc->sw)) &=~(1 << REG_SW_SHIFT_C))
#define REG_SW_CLEAR_V(cc)  ((*(cc->sw)) &=~(1 << REG_SW_SHIFT_V))

static inline void cpu_init(cpu_t *cpu)
{
    cpu_core_t *cc = &cpu->core;
    cpu_cop_t *cp = &cpu->cp;

    cc->gp = &cc->regs[11];
    cc->fp = &cc->regs[12];
    cc->sp = &cc->regs[13];
    cc->lr = &cc->regs[14];
    cc->sw = &cc->regs[15];

    cp->pc = &cp->regs[0];
    cp->epc = &cp->regs[1];
    cp->hi = &cp->regs[2];
    cp->lo = &cp->regs[3];
}


#endif /* _SIMULATOR_CPU_H_ */

