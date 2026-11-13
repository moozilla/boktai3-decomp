# Local browser demonstration

Tags: #port #demo #wasm

Build from the repository root:

```sh
build/venv/bin/python port/demo/build_demo.py --hardware-root ../cx-port-hw/port --ppu-root ../cx-port-ppu/port
```

After branch integration, omit both directory overrides. Open
`build/demo/index.html` directly in a browser. It embeds the compiled C module;
no server, network request or runtime dependency is needed. The source
`port/demo/index.html` also works directly: choose the generated
`build/demo/boot.wasm` with its first file picker.

Choose `.ppu` or `.ppu.gz` captures made by port-ppu's capture harness. Rendering
runs in the linked C PPU, with hardware banks at the real GBA addresses. Multiple
captures play in filename order at 15 fps; use captures spaced four GBA frames
apart for approximately real-time playback. The optional reference picker accepts
a 240×160 mGBA PNG or PPM and reports exact RGB pixel differences.

The ROM picker runs the original early RTC, clock and video initialization C,
including the GPIO protocol and date synchronization. Its deterministic device
clock is 2025-11-13 00:00:00 UTC.
Its result is the initial white forced-blank frame. Full gameplay boot remains
in development; capture playback supplies recorded state rather than executing
the game loop. No ROM, captures, screenshots or encoded assets are committed.

The browser and Node share `replay.js`. To verify a capture set and generate PPMs
from the browser's compiled module:

```sh
node port/demo/render_captures.cjs build/demo/boot.wasm CAPTURE_FOLDER build/demo/rendered
```

`build_demo.py` produces only code artifacts. User-selected files remain local;
the page does not upload them. Modern browsers need WebAssembly and
`DecompressionStream` for gzip captures (uncompressed captures need no gzip API).
# Hardware audio showcase

Open `hardware.html` directly in a browser. Pulse, wave and noise buttons run the
portable C PSG without a ROM. For music, choose your Boktai 3 ROM and press Play.
The file remains on your computer. The scope displays actual wasm PCM; Save WAV
renders ten seconds with the same C code. This is an audio demonstration, not a
full-game boot. The small test sequencer omits LFO, MEMACC, XCMD and PSG notes.

`hardware.wasm` and its base64 companion `hardware-wasm.js` contain compiled C
only. No ROM, sample data, BIOS or game captures are distributed. The embedded
module allows `file://` use without a server or network request.

Rebuild and reproduce the artifacts from the repository root:

```sh
build/venv/bin/python port/demo/build_hardware.py
node port/demo/hardware_test.js
node port/demo/hardware_test.js baserom.gba
build/venv/bin/python port/hw/build.py --mgba-rom baserom.gba
build/venv/bin/python port/demo/render_audio_artifacts.py
```

The build also produces `build/demo/hardware.html`, a self-contained copy.
Runtime-ROM WAVs, plots, comparison data and animations stay under `build/demo/`.
The Node test exercises the browser's actual wasm module; Web Audio playback
and browser appearance still require a manual browser check. The native suite
runs with AddressSanitizer and UBSan. See `port/README.md` for integration limits.
