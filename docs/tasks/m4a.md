# Task: lift the MP2K (m4a) sound library from pokeemerald

The MP2K driver in Boktai 3 is Nintendo's SDK code, which pret reconstructed
as C in [pokeemerald](https://github.com/pret/pokeemerald) `src/m4a.c`
(a sparse checkout is at `/home/user/pokeemerald`). Compiled with our agbcc,
43 of its functions are byte-identical to Boktai's (see
`symbols/functions.csv`, source column mentions pokeemerald).

## Goal

`src/lib/m4a.c`: one translation unit covering the contiguous run of m4a.c
functions in the ROM, from `MidiKeyToFreq` (`0822FDB4`) to the last m4a.c
function before unrelated code (around `ply_xswee` `08231420` and the few
after it: ply_xwait, ply_xcmd_0D, DummyFunc if present). For every function in
that range, in ROM order:

* if pokeemerald's C matches, use it (copy the function body)
* otherwise `INCLUDE_ASM("asm/nonmatching", sub_XXXXXXXX);`. Improving these
  is a bonus, not required.

Unnamed functions in the range map to m4a.c functions by source order. For
example `sub_082300DC` sits where `m4aMPlayImmInit` should be. Record each
mapping in `symbols/proposed/m4a.csv`.

## Things to sort out

* Headers: copy what's needed from pokeemerald `include/gba/` into
  `include/gba/` with a header comment crediting pret (see THIRD_PARTY.md).
  Make `include/global.h` compatible (it already defines u8/u16/...; avoid
  duplicate typedefs, e.g. include `gba/types.h` from global.h instead).
* m4a.c references RAM globals (`gSoundInfo`, `gMPlayJumpTable`, `gCgbChans`,
  `gMPlayMemAccArea`, ...) and ROM tables (`gCgbScaleTable`, `gNoiseTable`,
  `gScaleTable`, `gFreqTable`, `gPcmSamplesPerVBlankTable`, `gCgbFreqTable`,
  `gClockTable`, `gSongTable`, ...). They must resolve to Boktai's addresses:
  * RAM: the linker only auto-defines names of the form `gUnk_02xxxxxx` /
    `gUnk_03xxxxxx`. For named RAM symbols, add `name = 0x0300xxxx;` lines to
    a new `symbols/ram.ld` and extend `tools/build.py` to append it to the
    generated link script. To find each address, compile the matching
    function and read the ROM word at each relocation (the literal pool).
  * ROM data: use the existing data labels (`gUnk_08xxxxxx` in `gen/data.s`)
    via `#define` or a symbol alias in `symbols/ram.ld`
    (`gScaleTable = gUnk_0824DB.. ;`). Don't add data definitions to C.
* m4a.c also *defines* data (e.g. `SoundMainRAM_Buffer`, `gSoundInfo`). Turn
  these into `extern` declarations; the build links `.text` only.
* Pokémon-specific functions (`SetPokemonCry*`, `IsPokemonCryPlaying`)
  don't exist in Boktai. Leave them out.

## Done when

`python3 tools/build.py` prints OK with `src/lib/m4a.c` in place, the unit
contains as many real C functions as possible, and the work is committed in
your worktree. Report which functions are C, which are INCLUDE_ASM, and the
RAM/ROM symbol mapping you derived.
