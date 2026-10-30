#include <assert.h>
#include "../firmware/src/cr_dr_clock.h"
static unsigned count;
static uint8_t last_step;
static void sink(void *ctx, uint8_t step, uint8_t mask) {
    (void)ctx; (void)mask;
    last_step = step;
    ++count;
}
int main(void) {
    cr_dr_clock_t c;
    unsigned lane, step;
    cr_dr_clock_init(&c);
    cr_dr_clock_start(&c);
    for (lane = 0; lane < CR_DR_PARTS; ++lane) {
        cr_dr_clock_t fresh;
        cr_dr_clock_init(&fresh);
        cr_dr_clock_start(&fresh);
        cr_dr_mute_toggle(&fresh.drums, lane);
        for (step = 0; step < 16; ++step)
            assert(!(cr_dr_step_mask(&fresh.drums, step) & (1u << lane)));
        cr_dr_queue(&fresh.drums, CR_DR_ACT_FILL);
        cr_dr_bar(&fresh.drums);
        for (step = 0; step < 16; ++step)
            assert(!(cr_dr_step_mask(&fresh.drums, step) & (1u << lane)));
    }
    cr_dr_clock_advance(&c, 0, 48000, 120, sink, 0);
    assert(count == 1 && last_step == 0);
    cr_dr_clock_advance(&c, 6000 * 32, 48000, 120, sink, 0);
    assert(count == 33 && c.drums.bar == 2);
    assert(last_step == 0);
    cr_dr_clock_stop(&c);
    cr_dr_clock_start(&c);
    count = 0;
    cr_dr_clock_advance(&c, 0, 48000, 120, sink, 0);
    assert(count == 1 && last_step == 0);
    return 0;
}
