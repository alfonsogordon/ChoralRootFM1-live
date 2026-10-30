#include <assert.h>
#include "../firmware/src/cr_dr_clock.h"
static unsigned calls, hits;
static void record(void *unused, uint8_t step, uint8_t mask) {
    (void)unused;
    assert(step < 16);
    calls++;
    if (mask) hits++;
}
int main(void) {
    cr_dr_clock_t c;
    cr_dr_clock_init(&c);
    cr_dr_clock_advance(&c, 48000, 48000, 120, record, 0);
    assert(calls == 0);
    cr_dr_clock_start(&c);
    cr_dr_clock_advance(&c, 6000, 48000, 120, record, 0);
    assert(calls == 2);
    cr_dr_clock_advance(&c, 90000, 48000, 120, record, 0);
    assert(calls == 17);
    assert(c.drums.bar == 1);
    assert(hits > 0);
    cr_dr_clock_stop(&c);
    cr_dr_clock_advance(&c, 48000, 48000, 120, record, 0);
    assert(calls == 17);
    return 0;
}
