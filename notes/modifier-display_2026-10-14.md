# Modifier menu graphics and input (tentative)

Reviewed contiguous block: `080408A4–08041908`, following the selection/stat
preview block. This batch adds ten exact functions, 2,876 bytes, **0.119
percentage points**. Including Claude's three new matches in PRs 76/77, the
combined branch reaches **22.817%** (6,309 functions; 551,106 bytes).

## Observed behavior

The header/frame initializer owns 18 display objects at menu +0x17C, each
0x60 bytes, and two 32-byte graphics resources at +0x13C/+0x15C. Header frames
track mode and category. Objects 9–12 are source panels; 14–17 are selection
markers. The overview hides the pending-choice preview and renders source stats.
The +0x118 32-byte buffer and display +0x48 pointer are tentatively named palette;
the copy size is observed, but the ultimate graphics consumer remains untraced.

The three modes select source records, primary entries, and secondary entries.
Category/index are signed bytes within an aligned 32-bit cursor word; the whole
word is saved/restored per mode. Named nested union/struct members are required:
this agbcc does not support anonymous members with the intended offsets.

Overview A enters selection; left/right switches modes. Selection accepts both
L/R and left/right for category changes and up/down for the six item rows.
A applies a pending selection, with capacity failures producing an error sound
and 40-frame error flash. B saves the cursor and returns to overview or enters
the changed-summary state. The summary accepts A/B after 39 frames. Start/B
closing behavior depends on the special-header flag. The previous name
`ModifierMenu_OpenSelection` for the state-3 transition remains provisional;
state-3 lifecycle needs review before renaming it.

Text renderers choose bank/index from mode, category, availability, and current
selection. Primary selected labels are collected from a 32-bit mask and padded
to capacity; available labels draw six rows with an unavailable placeholder.
The window wrapper at 080337FC consumes its caller's window ID, now reflected
in the C signature; its exact size and bytes are unchanged.

## Matching evidence

Full reviewed extents: 080408A4 420; 08040A74 72; 08040AE4 276; 08040BF8 252;
08041020 264; 08041128 344; 08041280 240; 08041514 312; 0804164C 608;
080418AC 88. Each new match has its own commit and passed the exact checker.
`depin.py --write` minimized hints before acceptance. Overview input and changed
input use no hints; selection input retains one counter-pointer hint.
Nine existing consumers were checked after the packed-cursor layout change.
The complete combined build printed `build/boktai3.gba: OK`
(`build/modifier-display-combined.log`); format, progress inventory and
RAM-literal validation passed.

Fixed r7 variables produced invalid frame-pointer behavior in agbcc experiments;
let allocation choose r7 naturally. Excess empty barriers can change branch
length estimates and introduce an extra branch even though they emit no code.
The shared repeat helper tests negative direction first, then positive with the
original counter, and restores the first counter when neither changes. Period
initialization before the saved-counter read reproduced the outgoing stack
argument order without pins in overview input.

## Remaining pending-choice renderer

08040CF4 remains a guarded, **84.2%** draft, with reviewed extent 716 bytes and
55 differing bytes. It earns zero exact credit. It renders the selected source
panel and current markers, with distinct category-3 handling for absent IDs.
Do not use the older r1-pinned category variant: it incorrectly lost category-0
control flow. The retained variant has the complete original structure.

After merging Claude's PRs 76–78, `regdiag.py --nonmatching --asm` reported
297 candidate instructions, **52 register differences and zero structural
differences**. Most are reload temporaries; two global allocations also differ.
`isolate.py` split multi-name declarations and allowed a bounded 60-second,
two-worker permuter search to start (base score 285). No accepted candidate
resulted. Forcing the first differing display offset into r3 gave 85.1% fuzzy
but worsened instruction placement; retained only as local evidence, not source.
Use reload-rotation evidence in docs/REGALLOC.md for a new targeted hypothesis.

## Next contiguous subsection

Start at 08041908 and the unmatched 0804194C / 08041A44: 3D preview construction,
model state and task lifecycle. The current menu struct describes only its
known prefix; observed later fields at +0x1086 are outside that prefix.
