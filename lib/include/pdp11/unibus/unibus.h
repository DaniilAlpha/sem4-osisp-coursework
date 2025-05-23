#ifndef UNIBUS_H
#define UNIBUS_H

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

#include <result.h>
#include <woodi.h>

#include "pdp11/unibus/unibus_device.h"

#define UNIBUS_DEVICE_COUNT (8)

#define UNIBUS_DEVICE_CPU (NULL)

#define UNIBUS_CPU_PSW_ADDRESS (0177776)

typedef struct Pdp11Cpu Pdp11Cpu;
typedef struct Unibus {
    UnibusDevice devices[UNIBUS_DEVICE_COUNT];

    pthread_mutex_t _bbsy, _sack;
    UnibusDevice const volatile *_master, *_next_master;

    Pdp11Cpu *_cpu;
} Unibus;

void unibus_init(Unibus *const self, Pdp11Cpu *const cpu);
void unibus_uninit(Unibus *const self);
void unibus_reset(Unibus *const self);

static inline bool unibus_is_running(Unibus const *const self) {
    return self->_master == UNIBUS_DEVICE_CPU;
}

void unibus_br_intr(
    Unibus *const self,
    unsigned const priority,
    void const *const device,
    uint8_t const intr
);

Result unibus_npr_dati(
    Unibus *const self,
    void const *const device,
    uint16_t const addr,
    uint16_t *const out
);
Result unibus_npr_dato(
    Unibus *const self,
    void const *const device,
    uint16_t const addr,
    uint16_t const data
);
Result unibus_npr_datob(
    Unibus *const self,
    void const *const device,
    uint16_t const addr,
    uint8_t const data
);

Result
unibus_cpu_dati(Unibus *const self, uint16_t const addr, uint16_t *const out);
Result
unibus_cpu_dato(Unibus *const self, uint16_t const addr, uint16_t const data);
Result
unibus_cpu_datob(Unibus *const self, uint16_t const addr, uint8_t const data);

#endif
