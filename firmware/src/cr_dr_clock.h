/* SPDX-License-Identifier: GPL-3.0-only
 * ChoralRoot+ sample-accurate 16th-note drum scheduler.
 * Pure timing layer; the host chooses how to render emitted hits.
 */
#ifndef CR_DR_CLOCK_H
#define CR_DR_CLOCK_H
#include <stdint.h>
#include "cr_dr_patterns.h"
typedef struct {
    cr_live_drums_t drums;
    uint64_t phase;
    uint8_t step;
} cr_dr_clock_t;
static inline void cr_dr_clock_init(cr_dr_clock_t *c) {
    cr_dr_init(&c->drums);
    c->phase = 0;
    c->step = 0;
}
static inline void cr_dr_clock_start(cr_dr_clock_t *c) {
    c->phase = 0;
    c->step = 0;
    c->drums.bar = 0;
    c->drums.running = 1;
}
static inline void cr_dr_clock_stop(cr_dr_clock_t *c) {
    c->drums.running = 0;
    c->phase = 0;
    c->step = 0;
}
/* Call once per audio block. Every due 16th-note is delivered to emit(),
 * including skipped steps in long blocks. No dynamic memory or floats.
 * BPM can change without restarting the phase.
 * The first step is emitted immediately on the first running block.
 */
static inline void cr_dr_clock_advance(cr_dr_clock_t *c, uint32_t frames,
                                       uint32_t sample_rate, uint32_t bpm,
                                       void (*emit)(void *, uint8_t, uint8_t),
                                       void *context) {
    const uint64_t threshold = (uint64_t)sample_rate * 60u;
    if (!c->drums.running || !sample_rate || !bpm || !emit) return;
    if (c->step == 0 && c->phase == 0) {
        emit(context, 0, cr_dr_step_mask(&c->drums, 0));
        c->step = 1;
    }
    c->phase += (uint64_t)frames * bpm * 4u;
    while (c->phase >= threshold) {
        c->phase -= threshold;
        if (c->step == 16) {
            c->step = 0;
            cr_dr_bar(&c->drums);
        }
        emit(context, c->step, cr_dr_step_mask(&c->drums, c->step));
        c->step++;
    }
}
#endif
