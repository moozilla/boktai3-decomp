# Boktai 3 on the original Game Boy

From the repository root, build the cartridge and reproduce its gameplay checks with one command.

```sh
build/venv/bin/python demake/build.py
```

Open `build/demake/boktai.gb` in mGBA or a DMG-compatible emulator. The cartridge targets monochrome DMG hardware with an MBC5 mapper. Everything converted from the game stays under `build/`. The inputs are the project's verified GBA ROM, decoded asset tools and C sources. Python needs NumPy and Pillow. Music conversion uses Node and the repository's Studio MP2K sequencer. Building the SM83 code requires no external Game Boy toolchain.

## Play

Start at the title. The D-pad walks in screen directions. A fires in Django's last direction, or opens a door when he stands at its threshold. Solar Fruit restores energy and Earth Fruit restores health. Touching an enemy costs a heart. Losing all three returns Django to the room entrance. The route covers all twenty Sealed Prison rooms, with a side passage off scene 2759. The opening ghoul and two skeletons in the skylight chamber provide the first combat encounters.

Start opens the current floor plan. The arrow marks Django's position on the HP grid. A opens a scene message when available. A or Right advances, Left goes back, Select switches languages, and B returns to the plan. The journal renders Japanese from the cartridge font. It also includes English when the local `build/text/english.tsv` translation is available. Select on the floor plan saves a checkpoint. Start at the title resumes it and A starts a fresh run. A new run writes the battery only when the player saves.

**Sunlight.** Select in the field opens the sun meter. Hold a thumb just above the reflective LCD and match its shadow to the three examples with Left or Right. Press A to record it. Hold B beneath a scripted skylight to store energy. A sharp shadow charges one pip per eight ticks, a soft shadow per 32, and no shadow gives no charge.

**Game Boy Camera.** On the meter, Start turns the link port into a Game Boy Printer receiver. Connect a second Game Boy with a Camera, photograph the same pale card at a fixed exposure, and choose Print. A accepts the resulting brightness. The SM83 measures the shade density of the image as the bytes arrive, including compressed strips. The four levels give charge intervals of never, 32, 16, and 8 ticks. The reading depends on the photograph and camera settings. The receiver has been exercised with printer packets on two emulator cores. Physical DMG, Camera and cable testing remains open.

## Reuse the converters

Run these commands with `build/venv/bin/python demake/build.py` followed by the arguments below.

| Arguments | Generated output under `build/demake/` |
| --- | --- |
| `rom` | Default cartridge and its bank, resource and symbol manifest |
| `rom --scenes 3243` | A different playable cartridge under `custom/`, using the selected scene's stage, entrances, terrain and song |
| `rom --area-start 2698` | Discover nearby scenes by following decoded door links. An optional `--song N` chooses the arrangement |
| `room 2712` | Raw 2bpp BG patterns, signed tile indices, HP cells, entrances, routes and a source comparison |
| `stage CF70 F8EA 7239 --palette E39B` | Convert an MP/PTN/HP resource triple directly. Supply a palette for maps without a captured or scripted one |
| `actor DE23 --frames 0,2,4,23` | OBJ tile pairs, placement records and a source comparison. `--frames all` exports every pose and paginates the sheets |
| `actor 9247 --frames 0,2,5,7 --palette 403` | Select a variant from the OBJ palette pool. `--size 24 32`, `--scale`, `--obp`, `--archive` and `--record` control layout and resource selection |
| `music 18` | Four-voice register score, wave RAM data and MP2K loop provenance |
| `catalog --check` | Audit every decoded stage triple and sprite frame. The JSON lists archive names, record indices, frame counts and captured palette matches |

## Conversion and runtime

* The host composites GBA MP metatiles in layer order, resolves PTN tile pools, flips and subpalettes, and applies CIE L* contrast separately to the used palette banks. Dither follows a tile-aligned 4×4 pattern and fades near sharp contours. A frequency-weighted tile dictionary preserves common masonry and high-contrast edges within the signed 256-pattern DMG BG address range.
* The floor crop follows the projected HP footprint. A room fits a 512×512 pixel world. Native scale survives where that footprint fits. The cartridge streams entering rows and columns into the 32×32 BG ring during VBlank. It retains the original isometric X/Z projection and both stair axes. Eighth-cell coordinates replace the GBA's fixed-point movement. Other affine layers and color effects are baked into the room presentation.
* CB05 packages and 98F5 actor records share the OBJ exporter. A 24×32 actor uses six 8×16 objects. Three opaque indices preserve dark outlines, the middle tone and highlights. Django and enemies use different hardware palettes. The renderer stages poses in spare VRAM and swaps tile bases after upload. It emits the player and actors first, enforces the ten-objects-per-line limit, and transfers OAM from HRAM via DMA.
* Script extraction and the C sources feed compact tables and decisions in a hand-written SM83 engine. Door records carry source boxes and destination entrance positions. The ghoul samples a different HP route point, waits and moves, marks its death animation at frame 0x28 and leaves after 0x78. The skeleton winds up, bursts and decelerates from 0x48, halving speed every fourth tick. The engine supports one patrolling ghoul or up to two skeleton patrols per room. Other enemy families, moving puzzle objects, full cutscene choreography and the original story VM remain future work.
* The score converter assigns two pulse voices, one wave voice and noise percussion, then encodes changes and the source loop. Shot, hit and pickup effects temporarily use the pulse or noise channel.

## Evidence

`contact.png` contains native replay frames. `prison-atlas.png` shows every room from an uninterrupted D-pad traversal. `rooms.png`, `django.png`, `ghoul.png` and `skeleton.png` compare original color assets with the packed grayscale result. `prison.wav` is stereo output captured from the cartridge APU. `manifest.json`, `checks.json` and the `*.replay` files contain the inputs and results.

The repository's GBA harness installs ARM-specific hooks and crashes on the GB probe. The demake's small C driver uses libmgba's public GB core. The independent Python SM83/PPU executes the actual cartridge opcodes, checks DMA and LCD access timing, compares visible tiles against the ROM map at each step, and renders the hardware registers. The verifier checks native screenshots pixel by pixel after normalizing mGBA's four display values, along with 128 bytes of game-state WRAM. If libmgba headers and a C compiler are unavailable, the command still produces the reference frames and replays. `MGBA_PREFIX` can point to a local installation.
