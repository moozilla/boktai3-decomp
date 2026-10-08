# Handoff: state of the project and how to continue

Last updated at the end of session 3 (2026-10-09). This file is the single
entry point for a new agent, a new thread after compaction, or a helper on
another plan. Read it fully, then `docs/WORKER.md` (the matching playbook).

## 1. Where things stand

| | |
|---|---|
| Matched C | **4,145 / 11,025 functions (37.6%)**, ~9.1% of code bytes (`PROGRESS.md`) |
| Build | `make` / `python3 tools/build.py` rebuilds the ROM **bit-identical** (SHA-1 `2651c5e6875ac60abff734510d152166d211c87c`) |
| Shiftable | all data relocatable; `make shifttest` (3 scenarios, 35 screenshots) passes |
| Code layout | `src/fn/sub_XXXXXXXX.c`, one function per file; `src/lib/m4a.c` (MP2K sound, 57/58) |
| Names | still mostly `sub_XXXXXXXX`; naming/context pass not started |

Why bytes trail functions: workers go easiest-first. ~7,000 functions remain;
most small ones (<=40 insns) are done in most ranges, so the frontier is the
41-150 instruction tier, switches, and the logged skip classes below.

History in one paragraph: session 1 built the disassembly (gbadisasm),
shiftable data (46k symbolized pointers), the C pipeline and the first 664
matches. Session 2 added `check.py`, `clones.py`, `permute.py`, found 3,096
hidden functions (data-table seeds), and solved three "impossible" codegen
patterns. Session 3 ran 5-6 Sonnet workers overnight (~1,950 matches).

## 2. Setting up (new machine/session)

The ROM is never in git. Upload it, then:

```
ln -s /path/to/rom.gba baserom.gba        # SHA-1 above (Japanese, U33J)
apt install build-essential cmake binutils-arm-none-eabi libmgba-dev python3-numpy python3-pil
pip install capstone pycparser toml        # pycparser/toml only for the permuter
make setup      # gbadisasm (patched), agbcc, armips, emulator harness
make disasm     # ~15-20 min: gen/code.s -> gen/code_sym.s + gen/data.s (11,025 functions)
make            # must print build/boktai3.gba: OK
git clone https://github.com/simonlindholm/decomp-permuter ../decomp-permuter   # optional
```

`gen/` and `build/` are generated, gitignored, never committed (ROM-derived).

## 3. How to match a function (the inner loop)

```
python3 tools/worklist.py --range 08000000 080492B0 --max-insns 80   # unmatched, easiest first
python3 tools/asmat.py 0800E08C 60      # the asm (runs past the end; stop at the next func_start)
python3 tools/ghidra_c.py 0800E08C      # Ghidra draft, if gen/ghidra export exists (optional)
mkdir -p wip && $EDITOR wip/sub_0800E08C.c
python3 tools/check.py wip/sub_0800E08C.c   # ~0.5 s; side-by-side diff, '!' = differs; ends MATCH/MISMATCH
cp wip/sub_0800E08C.c src/fn/ && python3 tools/build.py   # must print OK, then commit
```

Rules that matter (all in `docs/WORKER.md`, which also has ~40 agbcc tricks):
* Only commit files that MATCH and a build that prints OK. Never hand-written asm.
* Never write raw ROM data addresses (`0x086xxxxx`) in C: declare the
  `gen/data.s` label (`extern const u8 gUnk_08603300[];`). build.py rejects them;
  they break the shift test.
* RAM: `extern T gUnk_02xxxxxx;` (the linker defines it from the name).
* Callees: declare locally with their `sub_` name. Functions that have a real
  name in gen (e.g. `Script_*`, m4a names) must be defined under that name.
* Always read check.py's final MATCH line; a compile error prints no `!` lines.

## 4. Tools

| Tool | What |
|---|---|
| `tools/build.py` | split asm around src/*.c, compile (agbcc -O2 -mthumb-interwork; per-file `// CFLAGS:` override), link, SHA-1 check |
| `tools/check.py FILE` | one file vs ROM at its address, side-by-side diff |
| `tools/worklist.py` | unmatched functions with size/branch/jumptable signals; `--chunks N` to split ranges |
| `tools/clones.py` | clone families: `--stats`, `--port [--loose] [--range A B]` copies matched C to identical siblings (loose = immediates may differ), keeps what check.py accepts. ~6 min for the full ROM. Run after every merge. |
| `tools/permute.py FILE --run SECS` | decomp-permuter for register-allocation misses |
| `tools/progress.py [--markdown PROGRESS.md]` | numbers |
| `tools/disasm.py` / `symbolize.py` | regenerate gen/ (seeds: BL targets, code literals, data-region function tables, aligned gap targets) |
| `tools/shift_test.sh`, `make shifttest` | move data 64 KiB, compare emulator screenshots |
| `tools/emu/harness` | headless mGBA: scripted input, screenshots, coverage, `dump/watch/poke/freeze` |
| `tools/mgba/centaur.lua` | desktop mGBA: input recorder + peek/poke console |

## 5. Running parallel workers (what worked)

* `tools/worktree.sh NAME` -> `../wt/NAME` on branch `work/NAME`, sharing gen/,
  the ROM and tools by symlink. Existing worktrees: `../wt/s0`..`s5`, `bk`, `disc`.
* Ranges used in session 3 (each has ~1,000+ functions left):
  s0 `08000000-080492B0`, s1 `080492B0-0810ECEC`, s2 `0810ECEC-08182F24`,
  s3 `08182F24-081F612C`, s4 `081F612C-0824DAFA` (skip MP2K `0822F248-08231440`).
* Give each worker `docs/WORKER_ROUND.md` with NAME/START/END. Sonnet-class
  models did 80-135 matches per round at 130-340k tokens. Workers commit
  after every batch, so a usage-limit cutoff loses little.
* Lead merges: `git merge work/NAME`; on add/add conflicts (two workers or
  clones.py matched the same function) keep either side (`git checkout --ours`),
  check for `<<<<<<<`, build, push. Then `clones.py --port --loose`.
* A "seeder" worker is high-leverage: list unseeded clone families
  (see the snippet in the session-3 log: `clones.LOOSE=True; clones.families(...)`,
  families with no matched member, largest first), match one representative
  each, then port siblings. Top families gave 10-50 functions each.
* Run `make shifttest` after big merges; once, a worker's raw addresses broke it.
* Builds slow down to 30-120 s with 5-6 workers on 4 cores; workers batch.

### Helping from another plan / another agent

Pick a range nobody else is working on (coordinate via a GitHub issue or a
branch name), branch from `main`, follow sections 2-3, commit one function per
commit (`match sub_XXXXXXXX`), and open a PR. Every file must MATCH and
`build.py` must print OK on the branch. Good first targets:
`python3 tools/worklist.py --range START END --max-insns 40`, then 41-80.
Skips are logged per range in `notes/s*.md` (with reasons) — a fresh model
solving those is the most useful test of capability.

## 6. Known-hard classes (backlog for stronger models / permuter)

Solved (see WORKER.md): `mov ip, r0` leaves (struct arrays); redundant null
check (`return &p->unk0`); materialised bools (`static inline u8` helpers);
function pointers; switches (case order = asm order); fall-off-the-end returns.

Open:
1. A big constant derived from another constant's register (`movs r2,#0x94;
   lsls; ... subs r2,#4`) instead of separate literals (081A6108, 081A6EF8,
   0813BA24, 0814DF0C). Real struct fields get partway.
2. Alloc/init/free wrapper whose null path jumps straight to the pop with r0
   untouched (~10 clones in 081F-0821: 081FB788, 081FBB38, ...).
3. `movs r1,#3; ands r1,r0` (result in the constant's register): 081F612C family.
4. Stack `s16[3]` vectors whose address lives in a callee-saved register
   (081C43B0 family; 08201CAC's `u16 *q = (u16 *)&v` trick may help).
5. Loops where agbcc hoists or strength-reduces what the original recomputes
   (0811E8B0 family, 080150D0).
6. Pure register-allocation swaps (most remaining skips): `tools/permute.py`.
7. `CgbSound` (last m4a function): stack layout solved, registers left;
   `notes/cgb.md`, best permuter candidate `notes/cgb_permuted_1480.c`.
8. ROM-tail libgcc/libc pieces (0824xxxx) may need `// CFLAGS: -O2` (no
   interworking) or are hand asm (`pop {r4, pc}` epilogues).

## 7. Other open work

* decomp.dev report / CI (`tools/progress.py --json` is a start).
* Naming/context pass: tag functions by the screens they run on (harness
  coverage per `mark` segment), name subsystems, apply `symbols/proposed/*.csv`.
* Cleanup: merge `src/fn/*.c` into real translation units with shared headers
  and structs (contiguous runs); extract assets (graphics, tilemaps, songs:
  classify the 1,483 song-table entries).
* Coverage: record playthroughs with `tools/mgba/centaur.lua`; only ~6% of
  code is exercised by the scripted tests.
* Lua pseudo debug menu (user idea): extend `centaur.lua` with scene jumps,
  flag toggles, warps, sun level, using RAM addresses as the decomp names them.

## 8. Translation (separate repo)

[boktai3trans](https://github.com/moozilla/boktai3trans), branch
`claude/kind-shannon-ugkern`: text tooling (`decomp/tools/text_dump.py`,
`charmap.py`, `text/script.jsonl` with Japanese + 2007 English per string).
Ideas: inserter with box-width checks (1.47 MB free ROM space), `{1F}{xx}`
accents, VWF via `Text_DrawGlyph` (`08218D1C`). Lan Hikari's 0.9 PR there is
still open (recommendation: merge for players, ask for sources).
