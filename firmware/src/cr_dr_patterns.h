/* SPDX-License-Identifier: GPL-3.0-only
 * ChoralRoot+ deterministic 16-step rhythm patterns.
 * Header-only, no allocation; called by the audio scheduler at 16th-note boundaries.
 */
#ifndef CR_DR_PATTERNS_H
#define CR_DR_PATTERNS_H
#include <stdint.h>
#include "cr_live_drums.h"
/* GM note numbers: kick, snare, clap, closed hat, open hat, tom, rim, bell. */
static const uint8_t cr_dr_gm_notes[CR_DR_PARTS] = {36,38,39,42,46,45,37,56};
static const uint16_t cr_dr_pattern_a[CR_DR_PARTS] = {
    0x1111u, 0x1010u, 0x0000u, 0x5555u, 0x0000u, 0x0000u, 0x0000u, 0x0000u
};
/* Half-time feel at the SAME BPM and bar length as A:
 * kick on beats 1 and 3, snare on beat 3, hats on quarter notes.
 * Fills/breaks remain governed by the common 16-step transport. */
static const uint16_t cr_dr_pattern_b[CR_DR_PARTS] = {
    0x0101u, 0x0100u, 0x0000u, 0x1111u, 0x0000u, 0x0000u, 0x0000u, 0x0000u
};
/* Returns a bitmask of simultaneous drum strikes; mutes apply to fills too.
 * Caller owns tempo and the 16th-note clock; this function is pure. */
static inline uint8_t cr_dr_step_mask(const cr_live_drums_t *d, unsigned step) {
    unsigned lane;
    uint8_t hits = 0;
    const uint16_t *pat = d->variation ? cr_dr_pattern_b : cr_dr_pattern_a;
    if (!d->running) return 0;
    step &= 15u;
    for (lane = 0; lane < CR_DR_PARTS; lane++)
        if ((pat[lane] >> step) & 1u) hits |= (uint8_t)(1u << lane);
    if (d->active_action == CR_DR_ACT_FILL && step >= 12u) {
        hits |= (uint8_t)(1u << CR_DR_SNARE);
        if (step == 14u) hits |= (uint8_t)(1u << CR_DR_TOM);
    } else if (d->active_action == CR_DR_ACT_BRIDGE) {
        hits &= (uint8_t)~((1u << CR_DR_KICK) | (1u << CR_DR_SNARE));
        if (step == 0u) hits |= (uint8_t)(1u << CR_DR_BELL);
    } else if (d->active_action == CR_DR_ACT_BREAK) {
        hits = step == 0u ? (uint8_t)(1u << CR_DR_KICK) : 0u;
    }
    return (uint8_t)(hits & (uint8_t)~d->mute_mask);
}
#endif
