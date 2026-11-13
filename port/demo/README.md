# PPU browser demo

Open `index.html` directly in a modern browser. The small bundled WebAssembly
module is compiled exclusively from `port/ppu/ppu.c`; it contains no game assets.
The file picker reads `.ppu` or `.ppu.gz` captures and optional matching `.ppm`
references from your computer. Select numbered captures together to play them.
The browser renders each frame by calling the C scanline renderer 160 times.

The page uses an embedded copy of `ppu.wasm` in `ppu-module.js` so it works from
`file://` without fetch permissions or a web server. Both copies are generated
by the same command:

```sh
build/venv/bin/python port/demo/build.py
node port/tests/ppu_demo.mjs build/ppu/startup-delta build/demo/motion
```

The test executes the exact browser bridge and bundled module in Node, compares
all pixels with mGBA, and checks rejection of malformed captures. It does not
automate the browser UI. Gzip loading uses the browser's `DecompressionStream`;
older browsers can load uncompressed `.ppu` files instead.

To make screenshots and an animation from the local save-bank captures:

```sh
build/venv/bin/python port/tests/ppu_regression.py \
  --capture port/tests/ppu_motion.txt --save build/ppu/story/0032/input.sav \
  --out build/demo/motion --wasm
build/venv/bin/python port/demo/artifacts.py
```

`artifacts.py` writes six labeled native-C/mGBA PNG comparisons, a combined
gallery, and a 60-frame APNG at approximately 15 fps. Every animation frame is
checked before export. `build/demo/README.md` lists the generated artifacts.
These files depend on the user's ROM and save bank; never commit them. The
capture-generation commands are documented in `port/README.md`.

This demo replays graphics input. It does not run game logic, emulate the CPU,
or load a ROM directly. Captures preserve HBlank changes; a single end-of-frame
memory dump would lose those effects.
