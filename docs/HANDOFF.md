# Handoff: state of the project and how to continue

Last updated during production batch 4 (2026-10-08 Pacific /
2026-10-09 UTC). This file is the single
entry point for a new agent, a new thread after compaction, or a helper on
another plan. Read it fully, then `docs/WORKER.md` (the matching playbook).

## 1. Where things stand

| | |
|---|---|
| Matched C | **4,456 / 11,047 functions (40.34%)**, 268,626 / 2,415,354 code bytes (11.122%; `PROGRESS.md`) |
| Build | `make` / `python3 tools/build.py` rebuilds the ROM **bit-identical** (SHA-1 `2651c5e6875ac60abff734510d152166d211c87c`) |
| Shiftable | all data relocatable; `make shifttest` (3 scenarios, 35 screenshots) passes |
| Code layout | `src/fn/sub_XXXXXXXX.c`, one function per file; `src/lib/m4a.c` (MP2K sound, 57/58) |
| Names | still mostly `sub_XXXXXXXX`; first evidence-based pause-menu leads in `docs/MENU_CONTEXT.md` |

Why bytes trail functions: workers go easiest-first. ~7,000 functions remain;
most small ones (<=40 insns) are done in most ranges, so the frontier is the
41-150 instruction tier, switches, and the logged skip classes below.

History in one paragraph: session 1 built the disassembly (gbadisasm),
shiftable data (46k symbolized pointers), the C pipeline and the first 664
matches. Session 2 added `check.py`, `clones.py`, `permute.py`, found 3,096
hidden functions (data-table seeds), and solved three "impossible" codegen
patterns. Session 3 ran 5-6 Sonnet workers overnight (~1,950 matches).

The local session built and verified the native macOS toolchain, added ROM-free
progress Actions and registered [decomp.dev](https://decomp.dev/moozilla/boktai3-decomp).
The reporting denominator now includes three already-matched SDK boundaries
that the old numerator counted but the old denominator omitted (11,028 report
functions versus 11,025 generated disassembly starts). This is an accounting
correction, not three new matches.

An isolated six-target pilot produced six new matches: Sol 5/6, Astra 6/6,
Luna 4/6 after an explicit continuation. See `docs/MODEL_BENCHMARK.md` for
attempts, elapsed times and limitations. Use Sol as the initial default matcher;
escalate hard cases to Astra and give Luna bounded tasks with explicit retry
rules. No per-model plan-usage/cost measurement was available. This runtime
allows three simultaneous workers alongside the orchestrator. Workers retain
per-function commits; the orchestrator validates and merges batches into main.
Progress Actions run only for main pushes or manual dispatch.

Production round 1 then added **52 new matches**: Luna A 15, Luna B 11,
Sol 26 including a compiler-thunk follow-up. One redundant rewrite was excluded.
The verified `_call_via_r4` alias unblocks ordinary C indirect calls through r4.
Per-worker patterns and failures are in `notes/round1-*.md`; detailed accounting
and limitations are appended to `docs/MODEL_BENCHMARK.md`.

The community-reference pass found real permuter wrapper bugs: symbolic versus
numeric addresses incurred false penalties, retained data inflated scores, and
regex source stripping could erase compact loops. `tools/permute.py --debug`
now diagnoses the base without those errors. It preserves static inline helpers,
requires one target ROM function and creates isolated run directories. See
`docs/COMPILER_REFERENCES.md` for tested GCC patterns, upstream/fork evaluation
and validation. A 60-second trial on `081A55D4` did not solve its remaining
register allocation difference; keep that candidate for future escalation.

Production round 2 added **113 new functions and 14,996 progress-span bytes**:
Luna A five, Luna B 21, Sol 40, main-thread counter family seven and libgcc 40.
Three redundant rewrites were excluded. Sol's bounded permuter trials solved
three targets, and allocator-wrapper null fallthroughs are now matched across
the family. The main thread matched both floating-point runtime units with
`old_agbcc -O2`; 18 hidden library boundaries were added to the inventory.
See `docs/LIBGCC.md`, `notes/round2-*.md` and `docs/MODEL_BENCHMARK.md`.

The user authorized continued work until 20% matched code bytes or weekly
allowance below **40% remaining** (extended from 50% by explicit user confirmation),
then draining all existing assignments. Check
the account meter before each new round. Two Luna workers began round 3 while
a single requested Astra worker reviewed external development-history claims
and SolDec. Its completed `docs/PROCESS_REVIEW.md` identifies concrete GCL-family
script correspondences, B3-specific decoding differences and a 722-target actor
registry. `tools/soldec_audit.py` supplies bounded, read-only checks; no speculative
engine/compiler claims were adopted. The main thread continues hard matching and reviewed batch integration.
Do not conflate worker commit counts with new coverage.

Production batch 3 adds **97 functions and 22,800 progress-span bytes**:
Luna A six, Luna B four, Sol 40, MGS-focused Sol 25 across two rounds, and the
main thread 22 fdlibm functions. See `docs/LIBM.md` for exact historical source
matches and `docs/MGS_GCL_MATCHING.md` for all MGS comparisons, B3 differences
and remaining candidates. One Sol worker remains dedicated to MGS at the
user's request. No speculative gameplay names were installed.

The user challenged continuing Luna after its lower recent output. Luna B's
next 9-minute round added zero matches and was stopped; its slot now runs Sol
6.1 on retained family seeds. This supersedes the earlier decision to keep
Luna production workers running. No measured Pareto-efficiency claim is valid:
account usage cannot be attributed to individual models, and these rounds have
different targets. Prioritize useful native-byte output over further benchmarking.

Production batch 4 adds 43 functions / 6,142 progress bytes (6,144 emitted):
Sol family rounds 1–2 (13), Sol round 4 (8), MGS round 3 (4), and root's
14 unoptimized SIIRTC routines, two memcpy/memset primitives and two clone
ports. See `docs/RTC.md` for the compiler/bitfield evidence. A reviewed but
unintegrated RTC entry at `08248E70` is now an unmatched progress boundary,
preventing 464 retained assembly bytes from receiving false C credit.
Terminal code alignment is handled once by the splitter. All 35 combined
shift screenshots and 32 tool/context tests pass.

The new `docs/SCRIPT_TRACING.md` records 698 native calls and 90 actor callbacks
from the intro, verified against command tables and the 722-entry actor registry.
The main thread fixed execution-sample PC adjustment and IRQ timing in the
harness. **Regenerate older execution coverage before entry-level claims.**
Existing memory-hook evidence is unaffected. `tests/script_probes.txt` and
`tools/script_trace.py` provide a reproducible read-only runtime trace.

The deeper task connects emulator coverage to function boundaries:
`tools/function_context.py`, `docs/RUNTIME_CONTEXT.md`, and `docs/MENU_CONTEXT.md`.
Screenshot review corrected a one-tab offset in the menu replay's labels and
separated transition marks from stable screens. Inputs and all 12 screenshots
remain unchanged. Config/sleep/save confirmation handlers are promising naming
leads; an A-button replay is the next evidence step before adopting the names.

## 2. Setting up (new machine/session)

The ROM is never in git. On the current Mac, it is already at `baserom.gba`,
extracted from the user's Downloads ZIP and SHA-1 verified. `build/venv`,
`build/tools`, `gen/`, and a verified baseline `build/boktai3.elf` are ready.
Activate `source build/venv/bin/activate` before using Python tools. Homebrew
provides `arm-none-eabi-binutils` and mGBA; `make setup` builds the pinned
compiler, disassembler, assembler, permuter and harness. See README for macOS
installation instructions. Full `make` and all 35 shift-test screenshots passed
after the production batches were integrated.

On a fresh Linux machine, supply the ROM, then:

```
ln -s /path/to/rom.gba baserom.gba        # SHA-1 above (Japanese, U33J)
apt install build-essential cmake binutils-arm-none-eabi libmgba-dev python3-numpy python3-pil
pip install -r requirements.txt
make setup      # gbadisasm (patched), agbcc, armips, permuter, emulator harness
make disasm     # ~15-20 min: gen/code.s -> gen/code_sym.s + gen/data.s (11,025 functions)
make            # must print build/boktai3.gba: OK
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

* `tools/worktree.sh NAME` -> `../wt/NAME` on branch `codex/NAME`, sharing gen/,
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
2. Alloc/init/free wrappers: solved in round 2 with guarded initialization and
   non-void null fallthrough retaining allocator r0; see notes/round2-sol.md.
3. `movs r1,#3; ands r1,r0` (result in the constant's register): 081F612C family.
4. Stack `s16[3]` vectors whose address lives in a callee-saved register
   (081C43B0 family; 08201CAC's `u16 *q = (u16 *)&v` trick may help).
5. Loops where agbcc hoists or strength-reduces what the original recomputes
   (0811E8B0 family, 080150D0).
6. Pure register-allocation swaps (most remaining skips): `tools/permute.py`.
7. `CgbSound` (last m4a function): stack layout solved, registers left;
   `notes/cgb.md`, best permuter candidate `notes/cgb_permuted_1480.c`.
8. Float/double libgcc units are now matched with `// COMPILER: old_agbcc` and
   `// CFLAGS: -O2`. Remaining libc and integer helpers are library candidates;
   compiler choice requires exact-byte evidence (docs/LIBGCC.md).

## 7. Other open work

* Progress reporting is live on decomp.dev; maintain the ROM-free report and
  batched main-only CI as the source layout evolves (`docs/PROGRESS_CI.md`).
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
