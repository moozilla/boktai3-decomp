# Yu-Gi-Oh! EDS decomp wiki pages (vendored)

Tags: #matching #agbcc

Two pages copied unchanged from the wiki of the Yu-Gi-Oh! The Eternal Duelist Soul
decomp, a 100%-matching GBA decomp built with pret's agbcc:

* Source: <https://github.com/CosmicScribe64/eds-decomp>, `wiki/concepts/`, commit
  `857002acd7a2e3ce98afb2992fe9761144308c9a` (2026-10-04).
* License: MIT, copyright "EDS decomp contributors". The notice is in [LICENSE](LICENSE)
  and must stay with these files.

| File | What it is |
|---|---|
| [matching-tricks.md](matching-tricks.md) | Their catalog of matching techniques (about 1,000 lines), grouped by the discrepancy each one fixes, with failed variants kept. |
| [compiler-flags.md](compiler-flags.md) | How they told `old_agbcc` from `agbcc` and `-fprologue-bugfix` from the ROM, with the evidence matrix. |

## Reading them for Boktai 3

* **Compiler.** EDS game code is built with `old_agbcc`; Boktai 3 uses `agbcc` (verified
  against the real SDK compiler). The two differ in constant-first vs load-first
  read-modify-write and in the leaf `push {lr}` (see compiler-flags.md). Tricks that rely on
  those differences won't transfer. Everything after the front end (cse, gcse, loop, flow,
  combine, local/global alloc, reload, sched) is the same GCC 2.95 tree, so the sections on
  CSE/loop/jump mechanics, allocation priority, reload rotation and scheduling apply directly.
* **Thumb.** EDS's C is Thumb. The mechanics also hold for our `agbcc_arm` units, but
  register names and constant synthesis differ.
* **Addresses and names** are EDS ROM addresses and EDS functions. `[[name]]` links point
  at EDS wiki pages that aren't copied here; read them upstream.
* **Their rules aren't ours.** EDS accepts `FAKEMATCH` hints and register pins that this project's
  rules forbid or count separately (see `CLAUDE.md`). Treat those entries as diagnostics.

Most useful sections of matching-tricks.md: "Register allocation priority and reload rotation",
"CSE, loop and jump mechanics", "Initialized compiler hints and scheduling", "Loops, scope,
escapes and live ranges", and "Failed, partial and rejected approaches".
