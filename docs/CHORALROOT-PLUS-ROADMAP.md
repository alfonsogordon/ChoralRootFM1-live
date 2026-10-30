# ChoralRoot+ — live performance additions

This repository is the development home for ChoralRoot+ (display name), based on ChoralRootFM1. Preserve upstream licensing and attribution.

## UX rules
- Preserve normal chord keys and all existing tap/hold functions.
- Expand the existing KEY layer with a SCALE page: Ionian, Dorian, Phrygian, Lydian, Mixolydian, Aeolian, Locrian; harmonic/melodic minor; pentatonics; blues; whole tone; diminished; Phrygian dominant; Hungarian minor.
- Make diatonic chord harmonisation optional, without silently changing explicitly selected major/minor chords.
- Add DRUMS as a page in the existing PERF layer; four controls: style, complexity, swing, level.
- Add an explicit DRUM LIVE keyboard mode with four part mutes (kick, snare, hats, percussion), A/B variation, fill, break/bridge, start/stop. Root keys play chords as normal outside this mode.
- Tap FILL queues a bar-aligned fill; hold FILL repeats; hold BREAK + tap FILL queues a bridge and automatically returns to the groove. Keep on-screen labels and a clear exit.
- Use existing drum synthesis as a starting point, without compromising chord voice allocation.
- Follow internal tempo or external MIDI clock; support quantised mutes and transitions.

## Implementation stages
1. Inspect and unit-test existing KEY layer, scale masks, chord builder, clock and input routing.
2. Implement modal scale selection and persistence, then test in emulator.
3. Implement drum transport/pattern generation and four independently mutable drum parts.
4. Wire PERF/DRUMS and DRUM LIVE tap/hold controls; verify no collisions with chords, looper, or panic.
5. Integrate emulator controls and audio; test hardware performance, save/restore and installation safety.

Status: roadmap committed; features not yet implemented.

## Confirmed upstream drum-engine status (source inspection)
- `firmware/src/choralroot.c` defaults `FELUCCA_DRUM` to `0`: the inherited `eng_drum.c` is not compiled in the stock ChoralRoot build.
- `firmware/src/engines.c` conditionally registers engine slot 10 only when `FELUCCA_DRUM` is enabled; otherwise the slot is `ENG_GONE`.
- `firmware/src/cr_ui.c` explicitly excludes `ENGI_DRUM` from its pitched-sound engine picker.
- Reuse the existing eight-lane `eng_drum.c` / `drum_voice.c` synthesis, but do not simply flip the compile flag without validating memory, audio part routing, engine registration, and host/emulator builds.
- The four live mute groups are kick, snare/clap, closed/open hats, and tom/rim/bell percussion. All eight internal voices can remain distinct while grouped into four convenient live controls.
- Stock ChoralRoot remains unchanged; this work belongs in the fork.
