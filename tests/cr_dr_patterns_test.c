#include <assert.h>
#include "../firmware/src/cr_dr_patterns.h"
int main(void) {
    cr_live_drums_t d;
    cr_dr_init(&d);
    assert(cr_dr_step_mask(&d, 0) == 0);
    d.running = 1;
    assert(cr_dr_step_mask(&d, 0) & (1u << CR_DR_KICK));
    cr_dr_mute_toggle(&d, CR_DR_KICK);
    assert(!(cr_dr_step_mask(&d, 0) & (1u << CR_DR_KICK)));
    cr_dr_mute_toggle(&d, CR_DR_KICK);
    d.variation = 1;
    assert(cr_dr_step_mask(&d, 0) != 0);
    cr_dr_queue(&d, CR_DR_ACT_FILL);
    cr_dr_bar(&d);
    assert(cr_dr_step_mask(&d, 13) & (1u << CR_DR_SNARE));
    cr_dr_mute_toggle(&d, CR_DR_SNARE);
    assert(!(cr_dr_step_mask(&d, 13) & (1u << CR_DR_SNARE)));
    cr_dr_queue(&d, CR_DR_ACT_BRIDGE);
    cr_dr_bar(&d);
    assert(!(cr_dr_step_mask(&d, 0) & (1u << CR_DR_KICK)));
    cr_dr_queue(&d, CR_DR_ACT_BREAK);
    cr_dr_bar(&d);
    assert(cr_dr_step_mask(&d, 1) == 0);
    return 0;
}
