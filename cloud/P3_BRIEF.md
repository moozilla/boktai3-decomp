# Phase 3 push: take the last concessions out of the Boktai 3 decomp

You are one of four Opus workers in a coordinated push. The repository is ~/git/boktai3-decomp
(main; the coordinator merges there). You work in your own worktree, given below, and commit to
its branch. The goal is the project's: **C that the stock compiler (agbcc -O2 -mthumb-interwork)
builds into a byte-identical ROM, written the way the original programmers wrote it.**

## Where the project is (2026-11-09)

* Phase 1 (draft) and phase 2 (match) are done: every function, 11,810 of 11,810, matches in C.
* Phase 3 (no concessions) is the work now. Code without concessions is 91.5%. **420 functions
  (204,516 bytes) still hold at least one concession.** Your queue is your share of them.
* Phase 4 (cleanup: readable, human-like, well-organized code) comes next. Everything you do
  should move toward it: real struct fields and types in `include/`, names that say what things
  are, shared helpers that the original code plausibly had.

A **concession** is C that matches only because it steers the compiler. `tools/concessions.py` is
the one list: `progress.py`, `scope.py land` and the decomp.dev report all read it.
* **Never acceptable:** register pins (`register T x asm("r4")`), `asm("")` operands, clobbers
  and barriers.
* **Judgment calls**, a concession unless excused: DEBUG_NOP(), an empty `do { } while (0)`, an
  empty `if` body, a single-use statement macro, a volatile local (`T *volatile`,
  `*(volatile T *)&x`), an `extern ... asm("Symbol")` second prototype, a non-default
  `// CFLAGS:` line, and a constant raw offset `*(u16 *)((u8 *)p + 0x172)` where a struct field
  belongs. The escape hatch is `// concession-ok(kind): <evidence the original had it>`. Use it
  only with real evidence (the same hook in every twin, a debug string nearby), never "it matches".
* `goto` is tracked but fine until phase 4.
* decomp.dev counts a function as matched only when it has no concession. The others score the
  share of their lines without one, so removing three of five pins still counts.

## Read first

`CLAUDE.md` (ground rules), `docs/WORKER.md`, `tools/scope/WORKER.md` (the loop),
`tools/scope/README.md` "agbcc facts established with scope" (facts 1-67; look them up as needed,
and read 50-67, which are recent), and the `tools/concessions.py` docstring.

Notes from earlier passes over these same functions, which are your starting points:
* `notes/round11-c-holdouts.md` and `notes/round11-d-holdouts.md` (on main): GPT-6 Pro's notes,
  one row per holdout, with its best score, the blocking compiler decision and often the what-if
  knob that matches.
* `~/git/wt/depin-shared/build/depin/QUEUE_PLAYBACK.md` (local, not committed): the depin
  worker's notes on playback functions, with drafts in `~/git/wt/depin-shared/build/scope/drafts/`
  (`aac/`, `he/`, `c10v/`, `sec_k1.c`, `spk12.c`, ...).
* matchsearch stores in `build/matchsearch/` of other worktrees (`~/git/wt/*/build/matchsearch/FUNC`).

## The loop, and how hard to try

Go through your queue in ROM order and **stay on a function until it matches without
concessions**. Naming the compiler decision that blocks it is the halfway point; the work is
finding ordinary C that produces the ROM's decision.

1. `python3 tools/scope/scope.py rom FUNC`, `scope.py like FUNC`, `tools/relatives.py FUNC --c`
   (matched relatives), `scope.py fields OFF...` (structs with members at those offsets).
2. `python3 tools/scope/scope.py FILE.c pins FUNC`: the cost of removing each pin, and the
   pin-free draft to start from (build/scope/drafts/).
3. `scope.py FILE.c FUNC` on a draft: the differing hunks with the C line and compiler pass behind
   each; `why UID` explains one. Confirm a hypothesis with a what-if knob (`AGBCC_PRI=name:+N`,
   `AGBCC_FORCE=pseudo:reg`, `AGBCC_SPILL`, `AGBCC_MOVE=UID`, `CSE_NOCANON`, ...). **The knob that
   matches is the diagnosis, not the answer.** Then find C with the same effect.
4. For a long search, use `build/venv/bin/python -m tools.matchsearch init
   build/matchsearch/FUNC FILE.c FUNC`, which keeps hypotheses, frontier and evidence across
   sessions.
5. Land with `python3 tools/scope/scope.py land FUNC`, which refuses anything that adds a
   concession. Never use `--allow-pins`. Check the unit with `python3 tools/check.py FILE.c`. Make
   one commit per function (or per family that shares a fix). A shared-header, data or tool change
   gets a commit of its own.
6. When a function resists after a real, deep effort, don't stop the run. Write its best pin-free
   draft and a note (what you tried, which knob matches, the next idea) into your queue's status
   column and `notes/phase3-holdouts.md` (create it if missing; one section per worker), then
   move on. Come back to it if a later function teaches you the trick.

### The forms that worked recently

Most of what's left is **allocation only**: the pin-free draft has the ROM's instructions, but a
register or stack slot differs. These C forms moved those decisions without concessions:

* **Use the shared helper the siblings use.** This is the biggest single lever for playback code:
  `PlayerCue_StepSequence(Ended)`, `SpriteEffectController_StepDrawable`,
  `include/display_sequence_inline.h` (the `union DisplaySequenceControl` flip test),
  `SceneParticle_StepSprite`, `ScenePanelRig_StepSprite`, the `child_take` and `set_direction`
  helpers in `actor_animation_controller.c`. Rewriting a private copy of a stepper on its matched
  shared form took one function from about 40 concessions to none.
* **Reference density:** global.c orders pseudos by priority (refs x frequency / live length), and
  find_reg can take a register away from a sparser local-alloc pseudo. A `static inline u8` helper
  for one test (`return slot->status[index] != 1;`) added insns that combine later folds, which
  lengthened `index`'s life until its density tied the table's, so nothing was evicted
  (ActorAttachment_BeginPartJitter). scope's agbcc now writes K/E records for these takeovers in
  names.txt. **`why` doesn't read them yet** (explain_alloc in scope.py); making it do so is a
  high-value tool fix.
* **Plain indexing instead of pointer locals:** `catalog[page * 8 + i]` in nested `for` loops.
  Strength reduction builds, hoists and spills the pointers the way the ROM does.
* **Byte and halfword member stores** (fact 63): `p->arr[i] = v` goes through store_bit_field
  (load, and, or), while `*(p->arr + i) = v` is a plain store. `spellings.sp_member_index`
  switches between them.
* **A u8 OR narrowed to QImode** (fact 54): read into a `u32` first.
* **A statement in each switch case or branch** instead of once after it. cse carries the constant
  into each case, and jump2 merges the tails (facts 27, 39, 59, 65; `sp_tail_into_branches`).
  `if (A) X; else X';` arms that jump2 cross-jumps leave the ROM's dead test (fact 57).
* **Union and array register forms:** a union DisplaySequenceControl read stops regmove tying an
  AND result to the loaded value (fact 66). A `u16 x[] = {0}` gives a halfword store its own zero
  (facts 61, 67).
* **Declaration order** sets stack-slot order and breaks priority ties.
* **cse:** an inline's folded field address isn't merged with the body's copy (fact 55). A store
  through a pointer local invalidates a cached global where a field store doesn't (fact 60,
  `sp_store_via_ptr`). A PRE copy is shared by two paths (facts 50, 52).
* **Raw offsets:** give the struct the field (its own header commit, layout unchanged) and use it.

An allocation-only form (a one-member union, a reused variable, a no-op cast, a one-element array,
a helper that exists only to shift a density) is legal but counts against phase 4 cleanliness.
Prefer a named type or helper that plausibly existed, and name the form in the commit message.

## Think big: the tools are part of the product

This project's method is tooling first: when you hit a gap, the fix to scope, fix.py, spellings,
hint.py, matchsearch, concessions.py or the compiler instrumentation is as valuable as the match,
because it pays off on every remaining function and in phase 4. Known gaps:
* `why`/explain_alloc doesn't read the K/E eviction records yet.
* The matchsearch evaluator has its own older concession detector that doesn't know the newer kinds.
* fix.py and spellings miss levers that worked by hand; add each as a spelling with a fix-suite
  case (`tests/fix/`; the suite is `tools/fix_suite.py`).
Add any new compiler fact to `tools/scope/README.md` with the next free number. Check for
collisions first: `grep -oE '^[0-9]+\. \*\*' tools/scope/README.md | sort | uniq -d` (28 is a
known old duplicate). Four workers add facts in parallel, so expect to renumber on merge. Run the
unit tests that cover a tool you change: `build/venv/bin/python -m unittest discover -s tests` and
`-s tests/tools`.

While you're in a unit, phase 4 work that comes naturally is welcome: recover a struct, name a
field or function (`symbols/functions.csv`, `confidence=low` for guesses), or replace a private
copy with the shared helper. Don't go on a rename spree outside your files.

## Rules

* Byte-identical with stock agbcc, from ordinary C. No new concession of any kind. No selector
  macros. Use real struct fields and types from `include/`. A header change must keep every user
  matching: `tools/check.py` each unit that includes it (`grep -l` the header).
* Don't build the whole ROM (the coordinator does at integration). Don't edit `build/asm/*`,
  `PROGRESS.md` or `PROGRESS.json`.
* Stay inside your queue's files. The other three workers own the other subsystems, and a shared
  header change should be small and layout-preserving.
* Never push, merge into main or open PRs. The coordinator integrates from your branch.
* No PII in commits (real names, usernames, `/Users/<name>` paths; write `~`). Never commit the
  ROM or anything from build/. Move files to ~/.Trash instead of `rm -rf`. US spelling.
* End each commit message with `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.

## Reporting

After every few landed functions (about every 30-45 minutes), SendMessage to "main" with a short
report: the commits, what decided each match, tool changes, holdouts, and what's next. Before each
report, merge the coordinator's latest main (`git fetch ~/git/boktai3-decomp main && git merge
FETCH_HEAD`), so your branch merges cleanly. If the coordinator tells you to stop, commit
everything (drafts and notes too) and end. Otherwise keep going through your queue: never stop a
run early. When the queue is done, take the holdouts again with what you learned, then improve
the tools.
