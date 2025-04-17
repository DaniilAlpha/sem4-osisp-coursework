#ifndef PDP11_CPU_H
#define PDP11_CPU_H

#include <stdbool.h>
#include <stdint.h>

#include "pdp11/cpu/pdp11_cpu_stat.h"
#include "pdp11/unibus/unibus.h"

#define PDP11_CPU_REG_COUNT (8)

typedef struct Pdp11Cpu {
    Pdp11CpuStat _stat;
    uint16_t _r[PDP11_CPU_REG_COUNT];

    Unibus *_unibus;
    enum {
        PDP11_CPU_STATE_RUNNING,
        PDP11_CPU_STATE_HALTED,
        PDP11_CPU_STATE_WAITING
    } volatile _state;  // TODO? maybe redo with condition vars
} Pdp11Cpu;

typedef enum Pdp11CpuTrap {
    PDP11_CPU_TRAP_CPU_ERR = 0004,
    PDP11_CPU_TRAP_CPU_STACK_OVERFLOW = 0004,
    PDP11_CPU_TRAP_ILLEGAL_INSTR = 0010,
    PDP11_CPU_TRAP_BPT = 0014,
    PDP11_CPU_TRAP_IOT = 0020,
    PDP11_CPU_TRAP_POWER_FAIL = 0024,
    PDP11_CPU_TRAP_EMT = 0030,
    PDP11_CPU_TRAP_TRAP = 0034,
    PDP11_CPU_TRAP_MEM_ERR = 0114,
    PDP11_CPU_TRAP_PROGRAM_IRQ = 0240,
    PDP11_CPU_TRAP_FP_ERR = 0240,
} Pdp11CpuTrap;

void pdp11_cpu_init(
    Pdp11Cpu *const self,
    Unibus *const unibus,
    uint16_t const pc,
    Pdp11CpuStat const stat
);
void pdp11_cpu_uninit(Pdp11Cpu *const self);

static inline uint16_t *pdp11_cpu_rx(Pdp11Cpu *const self, unsigned const i) {
    return (uint16_t *)(self->_r + i);
}
#define pdp11_cpu_rx(SELF_, I_) (*pdp11_cpu_rx((SELF_), (I_)))
static inline uint8_t *pdp11_cpu_rl(Pdp11Cpu *const self, unsigned const i) {
    return (uint8_t *)(self->_r + i);
}
#define pdp11_cpu_rl(SELF_, I_) (*pdp11_cpu_rl((SELF_), (I_)))
#define pdp11_cpu_pc(SELF_)     pdp11_cpu_rx((SELF_), 7)
#define pdp11_cpu_sp(SELF_)     pdp11_cpu_rx((SELF_), 6)

// TODO! volatile here is not actually needed. Instead better to place in some
// specific places (waiting for sufficient priority, also in tests)
static inline Pdp11CpuStat volatile *pdp11_cpu_stat(Pdp11Cpu *const self) {
    return &self->_stat;
}
#define pdp11_cpu_stat(SELF_) (*pdp11_cpu_stat(SELF_))

// TODO! this is badly designed function and is the worst part of the system at
// this point
void pdp11_cpu_trap(Pdp11Cpu *const self, uint8_t const trap);
void pdp11_cpu_continue(Pdp11Cpu *const self);

uint16_t pdp11_cpu_fetch(Pdp11Cpu *const self);
void pdp11_cpu_decode_exec(Pdp11Cpu *const self, uint16_t const instr);

#endif
