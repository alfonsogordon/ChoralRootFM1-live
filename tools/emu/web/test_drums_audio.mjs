// SPDX-License-Identifier: GPL-3.0-only
// Test the actual firmware-backed WebAssembly drum audio, not a JS synthesizer.
import fs from "node:fs";
const bytes = fs.readFileSync("build/emu-web/choralroot.wasm");
let mem;
const wasi = {
  clock_time_get: (_id, _prec, out) => { new DataView(mem.buffer).setBigUint64(out, 0n, true); return 0; },
  fd_write: (_fd, iov, n, out) => {
    const d = new DataView(mem.buffer);
    let len = 0;
    for (let i = 0; i < n; ++i) len += d.getUint32(iov + i * 8 + 4, true);
    d.setUint32(out, len, true); return 0;
  },
  proc_exit: () => {}
};
const stub = new Proxy(wasi, { get: (t, k) => k in t ? t[k] : () => 0 });
const { instance } = await WebAssembly.instantiate(bytes, {
  wasi_snapshot_preview1: stub, env: new Proxy({}, { get: () => () => 0 })
});
const ex = instance.exports;
mem = ex.memory;
ex._initialize?.();
ex.web_boot();
console.log("Drum diag immediately after boot:", ex.web_drums_diag?.().toString(16));
ex.web_master(850);
for (let i = 0; i < 30; ++i) ex.web_render(128);
console.log("Drum diag after warmup:", ex.web_drums_diag?.().toString(16));
if (typeof ex.web_drums !== "function" || typeof ex.web_drums_state !== "function")
  throw Error("Drum controls not exported by WASM");
if (ex.web_drums_state() & 256) throw Error("Drums should start stopped");
if (!ex.web_drums(1, 0)) throw Error("Start command rejected");
let energy = 0;
for (let i = 0; i < 800; ++i) {
  ex.web_render(128);
  const l = new Float32Array(mem.buffer, ex.web_out_l(), 128);
  for (const v of l) energy += v * v;
}
if (!(ex.web_drums_state() & 256)) throw Error("Drums never started");
console.log("Firmware drum audio energy:", energy, "engine/voices/ready:", ex.web_drums_diag?.().toString(16));
if (energy <= 0.000001) throw Error("No audible firmware drum audio in WebAssembly output");
if (!ex.web_drums(2, 0)) throw Error("Mute command rejected");
ex.web_render(128);
if (!(ex.web_drums_state() & 1)) throw Error("Kick mute did not reach firmware");
if (!ex.web_drums(0, 0)) throw Error("Stop command rejected");
ex.web_render(128);
if (ex.web_drums_state() & 256) throw Error("Drum transport did not stop");
console.log("PASS: integrated WASM drum audio, transport and lane mute");
