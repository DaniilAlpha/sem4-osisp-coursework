#include "unibus_test.h"

#include <unistd.h>

#include "miunte.h"
#include "pdp11/pdp11.h"

static Pdp11 pdp = {0};

/***********
 ** tests **
 ***********/

static MiunteResult unibus_test_setup() {
    MIUNTE_EXPECT(pdp11_init(&pdp) == Ok, "`pdp11_init` should not fail");
    pdp11_cpu_pc(&pdp.cpu) = 0x100;
    pdp11_cpu_sp(&pdp.cpu) = 0x1000;
    MIUNTE_PASS();
}
static MiunteResult unibus_test_teardown() {
    pdp11_uninit(&pdp);
    MIUNTE_PASS();
}

static MiunteResult unibus_test_npr() {
    UnibusDevice const *const device = &pdp.periphs[0];

    uint16_t const addr = 0x42;
    uint16_t const dato = 0xF00D;
    uint16_t dati = dato + 1;

    MIUNTE_EXPECT(
        unibus_npr_dato(&pdp.unibus, device, addr, dato) == Ok,
        "NPR DATO should not fail"
    );
    MIUNTE_EXPECT(
        unibus_npr_dati(&pdp.unibus, device, addr, &dati) == Ok,
        "NPR DATI should not fail"
    );
    MIUNTE_EXPECT(dati == dato, "data should be written correctly");

    MIUNTE_EXPECT(
        unibus_npr_datob(&pdp.unibus, device, addr + 1, (uint8_t)dato) == Ok,
        "NPR DATOB should not fail"
    );
    MIUNTE_EXPECT(
        unibus_npr_dati(&pdp.unibus, device, addr, &dati) == Ok,
        "NPR DATI should not fail"
    );
    MIUNTE_EXPECT(
        dati == ((uint16_t)(dato << 8) | (uint8_t)dato),
        "data should be written correctly"
    );

    MIUNTE_PASS();
}

static void *lower_cpu_priority_thread(void *const vcpu) {
    Pdp11Cpu *const cpu = vcpu;

    while (pdp11_cpu_state(cpu) != PDP11_CPU_STATE_HALT) sleep(0);

    for (uint8_t new_priority = pdp11_psw_priority(&pdp11_cpu_psw(cpu));
         new_priority >= 2;
         new_priority--)
        pdp11_psw_set_priority(&pdp11_cpu_psw(cpu), new_priority);

    return NULL;
}
static MiunteResult unibus_test_br() {
    UnibusDevice const *const device = &pdp.periphs[0];

    uint16_t const trap = 040, trap_pc = 0xACE;
    MIUNTE_EXPECT(
        unibus_npr_dato(&pdp.unibus, device, trap, trap_pc) == Ok,
        "NPR DATO should not fail"
    );

    MIUNTE_EXPECT(
        pdp11_cpu_pc(&pdp.cpu) != trap_pc,
        "PC should not be on trap before BR"
    );

    pdp11_psw_set_priority(&pdp11_cpu_psw(&pdp.cpu), 07);
    pthread_t thread;
    pthread_create(&thread, NULL, lower_cpu_priority_thread, &pdp.cpu);
    MIUNTE_EXPECT(
        pdp11_psw_priority(&pdp11_cpu_psw(&pdp.cpu)) >= 03,
        "test will be more useful if starting priority is greater than that of an interrupt"
    );

    unibus_br_intr(&pdp.unibus, 03, device, trap);

    MIUNTE_EXPECT(
        pdp11_psw_priority(&pdp11_cpu_psw(&pdp.cpu)) < 03,
        "interrupt should wait til CPU priority becomes sufficient"
    );
    pthread_join(thread, NULL);

    usleep(10 * 1000);

    MIUNTE_EXPECT(
        pdp11_cpu_pc(&pdp.cpu) != trap_pc,
        "PC should not be on trap even after the BR, before instruction is finished executing"
    );
    unibus_npr_dato(
        &pdp.unibus,
        device,
        pdp11_cpu_pc(&pdp.cpu),
        0010000 /* mov R0, R0 */
    );
    pdp11_cpu_single_step(&pdp.cpu);
    while (pdp11_cpu_state(&pdp.cpu) != PDP11_CPU_STATE_HALT) sleep(0);
    MIUNTE_EXPECT(
        pdp11_cpu_pc(&pdp.cpu) == trap_pc,
        "PC should be on trap after BR, after the next instruction is executed"
    );

    MIUNTE_PASS();
}

/**********
 ** main **
 **********/

int test_unibus_run(void) {
    MIUNTE_RUN(
        unibus_test_setup,
        unibus_test_teardown,
        {
            unibus_test_npr,
            unibus_test_br,
        }
    );
}
