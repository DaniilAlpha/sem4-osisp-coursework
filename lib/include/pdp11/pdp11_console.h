#ifndef PDP11_CONSOLE_H
#define PDP11_CONSOLE_H

#include <stdint.h>

#include "pdp11/cpu/pdp11_cpu.h"

#define PDP11_CONSOLE_CPU_REG_ADDRESS      (0177700)
#define PDP11_CONSOLE_SWITCH_REGISTER_ADDR (0177570)
#define PDP11_BOOTLOADER_ADDR              (0137744)

typedef enum Pdp11ConsolePowerControl {
    PDP11_CONSOLE_POWER_CONTROL_OFF,
    PDP11_CONSOLE_POWER_CONTROL_POWER,
    PDP11_CONSOLE_POWER_CONTROL_LOCK,
} Pdp11ConsolePowerControl;

typedef struct Pdp11Console {
    Pdp11Cpu *_cpu;
    Unibus *_unibus;

    Pdp11ConsolePowerControl _power_control_switch;
    uint16_t _switch_register;

    uint16_t _addr_register, _data_register;
    bool _is_deposit_pressed_consecutively, _is_examine_pressed_consecutively;

    bool _enable_switch;
} Pdp11Console;

void pdp11_console_init(
    Pdp11Console *const self,
    Pdp11Cpu *const cpu,
    Unibus *const unibus
);

static inline Pdp11ConsolePowerControl pdp11_console_power_control(
    Pdp11Console const *const self
) {
    return self->_power_control_switch;
}
static inline uint16_t pdp11_console_switch_register(
    Pdp11Console const *const self
) {
    return self->_switch_register;
}
static inline bool pdp11_console_enable_switch(Pdp11Console const *const self) {
    return self->_enable_switch;
}

uint16_t pdp11_console_address_indicator(Pdp11Console const *const self);
uint16_t pdp11_console_data_indicator(Pdp11Console const *const self);

bool pdp11_console_run_light(Pdp11Console const *const self);
bool pdp11_console_bus_light(Pdp11Console const *const self);
bool pdp11_console_fetch_light(Pdp11Console const *const self);
bool pdp11_console_exec_light(Pdp11Console const *const self);
bool pdp11_console_source_light(Pdp11Console const *const self);
bool pdp11_console_destination_light(Pdp11Console const *const self);
unsigned pdp11_console_address_light(Pdp11Console const *const self);

void pdp11_console_next_power_control(Pdp11Console *const self);
void pdp11_console_prev_power_control(Pdp11Console *const self);

void pdp11_console_toggle_control_switch(
    Pdp11Console *const self,
    unsigned const i
);

void pdp11_console_press_load_addr(Pdp11Console *const self);
void pdp11_console_press_deposit(Pdp11Console *const self);
void pdp11_console_press_examine(Pdp11Console *const self);

void pdp11_console_press_continue(Pdp11Console *const self);
void pdp11_console_toggle_enable(Pdp11Console *const self);
void pdp11_console_press_start(Pdp11Console *const self);

void pdp11_console_insert_bootloader(Pdp11Console *const self);

UnibusDevice pdp11_console_ww_unibus_device(Pdp11Console *const self);

#endif
