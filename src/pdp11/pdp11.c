#include "pdp11/pdp11.h"

#include <stdio.h>
#include <stdlib.h>

#include "conviniences.h"

/*************
 ** private **
 *************/

static inline uint16_t
pdp11_read(Pdp11 const *const self, uint16_t const addr) {
    return self->ram[addr / elsizeof(self->ram)];
}
static void pdp11_execute(Pdp11 *const self, uint16_t const instr);

/************
 ** public **
 ************/

Result pdp11_init(Pdp11 *const self) {
    // ram
    uint16_t *const ram = malloc(PDP11_RAM_WORD_COUNT * elsizeof(ram));
    if (!ram) return OutOfMemErr;
    self->ram = ram;

    // cpu
    self->cpu.pc = PDP11_STARTUP_PC;
    self->cpu.ps = PDP11_STARTUP_PS;

    return Ok;
}
void pdp11_uninit(Pdp11 *const self) {
    // ram
    free(self->ram), self->ram = NULL;

    // cpu
    self->cpu.pc = 0;
    self->cpu.ps = 0;
}

void pdp11_step(Pdp11 *const self) {
    pdp11_execute(self, pdp11_read(self, self->cpu.pc));
    self->cpu.pc += 2;
}

/******************
 ** private impl **
 ******************/

static inline void pdp11_op_mov(Pdp11 *const self) { printf("mov\n"); }

static inline void pdp11_op_ccc_scc(
    Pdp11 *const self,
    bool const do_set,
    bool const n,
    bool const z,
    bool const v,
    bool const c
) {
    self->cpu.ps =
        do_set ? self->cpu.ps | (n ? PDP11_PS_N : 0) | (z ? PDP11_PS_Z : 0) |
                     (v ? PDP11_PS_V : 0) | (c ? PDP11_PS_C : 0)
               : self->cpu.ps & ~(n ? PDP11_PS_N : 0) & ~(z ? PDP11_PS_Z : 0) &
                     ~(v ? PDP11_PS_V : 0) & ~(c ? PDP11_PS_C : 0);
}

static void pdp11_execute(Pdp11 *const self, uint16_t const instr) {
    uint16_t const opcode_15_12 = (instr >> 12) & 0b1111,
                   opcode_15_9 = (instr >> 9) & 0b1111111,
                   opcode_15_6 = (instr >> 6) & 0b1111111111,
                   opcode_15_3 = (instr >> 3) & 0b1111111111111,
                   opcode_15_0 = instr;

    printf(
        "executing : 0%06o, (0%02o 0%03o 0%04o 0%05o 0%06o)\n",
        instr,
        opcode_15_12,
        opcode_15_9,
        opcode_15_6,
        opcode_15_3,
        opcode_15_0
    );
    switch (opcode_15_12) {
    case 001: return pdp11_op_mov(self);
    case 011: return;  // movb
    case 002: return;  // cmp
    case 012: return;  // cmpb
    case 003: return;  // bit
    case 013: return;  // bitb
    case 004: return;  // bic
    case 014: return;  // bicb
    case 005: return;  // bis
    case 015: return;  // bisb
    case 006: return;  // add
    case 016: return;  // sub
    }
    switch (opcode_15_9) {
    case 0070: return;  // mul
    case 0071: return;  // div
    case 0072: return;  // ash
    case 0073: return;  // ashc
    case 0074: return;  // xor

    case 0000:
        if (!((instr >> 8) & 0b1)) break;
        return;         // br
    case 0001: return;  // bne/beq
    case 0002: return;  // bge/blt
    case 0003: return;  // bge/blt
    case 0100: return;  // bpl/bmi
    case 0101: return;  // bhi/blos
    case 0102: return;  // bvc/bvs
    case 0103: return;  // bhis/blo

    case 0077: return;  // sob

    case 0004: return;  // jsr

    case 0104: return;  // emt/trap
    }
    switch (opcode_15_6) {
    case 00001: return;  // jmp
    case 00003: return;  // swab
    case 00050: return;  // clr
    case 01050: return;  // clrb
    case 00051: return;  // com
    case 01051: return;  // comb
    case 00052: return;  // inc
    case 01052: return;  // incb
    case 00053: return;  // dec
    case 01053: return;  // decb
    case 00054: return;  // neg
    case 01054: return;  // negb
    case 00055: return;  // adc
    case 01055: return;  // adcb
    case 00056: return;  // sbc
    case 01056: return;  // sbcb
    case 00057: return;  // tst
    case 01057: return;  // tstb
    case 00060: return;  // ror
    case 01060: return;  // rorb
    case 00061: return;  // rol
    case 01061: return;  // rolb
    case 00062: return;  // asr
    case 01062: return;  // asrb
    case 00063: return;  // asl
    case 01063: return;  // aslb
    case 01064: return;  // mtps
    case 00065: return;  // mfpi
    case 01065: return;  // mfpd
    case 00066: return;  // mtpi
    case 01066: return;  // mtpd
    case 00067: return;  // sxt
    case 01067: return;  // mfps

    case 00064: return;  // mark

    case 00002:
        if (!((instr >> 5) & 0b1)) break;
        return pdp11_op_ccc_scc(
            self,
            (instr >> 4) & 0b1,
            (instr >> 3) & 0b1,
            (instr >> 2) & 0b1,
            (instr >> 1) & 0b1,
            (instr >> 0) & 0b1
        );
    }
    switch (opcode_15_3) {
    case 000020: return;  // rts
    }
    switch (opcode_15_0) {
    case 0000002: return;  // rti
    case 0000003: return;  // bpt
    case 0000004: return;  // iot
    case 0000006: return;  // rtt

    case 0000000: return;  // halt
    case 0000001: return;  // wait
    case 0000005: return;  // reset
    }

    // ill
}
