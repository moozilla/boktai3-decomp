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
