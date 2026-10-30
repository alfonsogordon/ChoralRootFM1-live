/* SPDX-License-Identifier: GPL-3.0-only
 * ChoralRoot+ live drum performance state.
 * Standalone control layer: audio transport/renderer and UI wiring follow separately.
 */
#ifndef CR_LIVE_DRUMS_H
#define CR_LIVE_DRUMS_H
#include <stdint.h>
enum { CR_DR_KICK, CR_DR_SNARE, CR_DR_HATS, CR_DR_PERC, CR_DR_PARTS };
enum { CR_DR_ACT_NONE, CR_DR_ACT_FILL, CR_DR_ACT_BRIDGE, CR_DR_ACT_BREAK };
typedef struct {
    uint8_t mute_mask;       /* bit 1 = muted; independent of current variation */
    uint8_t variation;       /* 0 = A, 1 = B */
    uint8_t running;
    uint8_t queued_action;
    uint8_t active_action;
    uint8_t action_bars;
    uint8_t fill_held;
    uint32_t bar;
} cr_live_drums_t;
static inline void cr_dr_init(cr_live_drums_t *d) {
    d->mute_mask = d->variation = d->running = 0;
    d->queued_action = d->active_action = d->action_bars = d->fill_held = 0;
    d->bar = 0;
}
static inline void cr_dr_mute_toggle(cr_live_drums_t *d, unsigned part) {
    if (part < CR_DR_PARTS) d->mute_mask ^= (uint8_t)(1u << part);
}
static inline int cr_dr_audible(const cr_live_drums_t *d, unsigned part) {
    return part < CR_DR_PARTS && !(d->mute_mask & (1u << part));
}
static inline void cr_dr_variation_toggle(cr_live_drums_t *d) { d->variation ^= 1u; }
static inline void cr_dr_queue(cr_live_drums_t *d, unsigned action) {
    if (action >= CR_DR_ACT_FILL && action <= CR_DR_ACT_BREAK) d->queued_action = (uint8_t)action;
}
static inline void cr_dr_fill_hold(cr_live_drums_t *d, int held) {
    d->fill_held = (uint8_t)!!held;
    if (held) cr_dr_queue(d, CR_DR_ACT_FILL);
}
/* Call once at each bar boundary from the existing clock. A queued action replaces
 * only the transient action, never the base variation or per-part mute states.
 * Holding fill re-queues it for the following bar. */
static inline void cr_dr_bar(cr_live_drums_t *d) {
    d->bar++;
    if (d->action_bars) {
        --d->action_bars;
        if (!d->action_bars) d->active_action = CR_DR_ACT_NONE;
    }
    if (d->queued_action) {
        d->active_action = d->queued_action;
        d->action_bars = 1;
        d->queued_action = CR_DR_ACT_NONE;
    } else if (d->fill_held && d->running) {
        d->active_action = CR_DR_ACT_FILL;
        d->action_bars = 1;
    }
}
#endif
