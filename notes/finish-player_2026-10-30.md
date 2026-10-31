# Finish-player worker batch

Tags: #matching #player #compiler #structs #cleanup #data #studio

## Checkpoint at the requested quota stop

Nine of eleven assigned functions match in ordinary C on `codex/finish-player`:
65.519% of this queue, a gain of 65.519 percentage points within the queue and
0.401089 project code percentage points (9,668 exact bytes). The continuation
added two matches and 0.069365 project percentage points (1,672 bytes).
The two remaining C drafts are preserved in Git under `NONMATCHING`; they do
not receive exact credit. Work stopped at the owner's request to retain at
least 20% weekly quota; the final wrap checks reported 22% remaining.

All nine exact functions have stock per-unit evidence and no register pins,
barriers, instruction bodies or compiler overrides. Both new guarded drafts
were formatted, scored with explicit reviewed extents and independently checked
in normal fallback mode. A combined ROM build and progress regeneration remain
for integration; this worker did not merge, push, regroup or open a PR.

## Results in ROM order

| Address | Bytes | Result | Deciding source property |
|---|---:|---|---|
| 08143B54 | 892 | Exact: `Player_ConstrainTetherTarget` | X-before-Z scaling, named scale result, initial position view and saved unsigned X copy after both cell coordinates. |
| 08143ED0 | 1,068 | Exact: `Player_UpdateTetherMovement` | Combined nullable movement predicate plus a by-value one-pointer player reference preserve the null branch and flag-load boundary. |
| 08148318 | 752 | Exact: `Player_UpdateWaveringMovement` | Ordinary shared playback replaces pins/barriers; RNG origin capture and separate facing/mask operations restore ordering. |
| 0814B1F8 | 4,176 | Guarded: `Player_UpdateTargetMotion`, 99.6% fuzzy | Nine register instructions differ, ten bytes; shape zero and exact extent. See the remaining reload below. |
| 0814C3A4 | 2,952 | Exact: `Player_UpdateGrabMotion` | Halfword bitfield mask, byte folded-facing value, typed fields and branch-local hit results. |
| 0814D264 | 1,216 | Exact: `Player_UpdateAttackColliderMotion` | One shared radial-radius variable across both branches, then commute the final arc Z product. Its dying operand changes the radius/table preferences without hints. |
| 08160108 | 456 | Exact: `PlayerBurst_InitializeParticles` | Correct 60-byte particle records, separate trig captures, raw halfword sample before masking, and a real ten-bit RNG cursor type. |
| 0816233C | 604 | Exact: `PlayerAttackEffect_UpdateParticles` | Shared playback boundaries, branch-local stored/reloaded angle, by-value sprite reference and frame-add operand order. |
| 082003BC | 1,620 | Exact: `PlayerClone_UpdateBehavior` | Typed records, signed halfword flags, palette bitfields and reuse of the height-field pointer to derive the blink drawable. |
| 08200C94 | 912 | Guarded: `PlayerClone_UpdatePool`, 96.1% fuzzy | Scope has 15/15 differences, shape four, exact extent; one missing vector-pointer copy causes the attack reload cascade. |
| 0823C4B4 | 108 | Exact: `Player_GetLinkedSmoothedDirection` | Matched twin's ordinary form: wide sample plus explicit second history read into a signed halfword result. |

Names with uncertain gameplay interpretations remain low confidence in
`symbols/functions.csv`. Existing raw-address aliases are retained for this
worktree's generated assembly. No project-wide declaration change was made.

## New exact matches

`0814D264` is commit `45f04f21e`. The earlier table-priority blocker was solved
by sharing one radius variable and changing only the dying operand's order in
the final multiply. The final source expands the geometry directly and has no
register concession. Stock verification covers all 1,216 bytes.

`08160108` is commit `218da4993`. The ten-bit cursor expresses the RNG ring's
wraparound and changes the implicit mask's lifetime, resolving both the
particle/table allocation and the initial reload sequence. Trig-capture tail
resets keep loop.c from hoisting those captures; flow deletes the resets.
The stock 456-byte match uses no compiler knobs or register declarations.

## Remaining target motion: 0814B1F8

The improved guarded draft is commit `91bffdc26`. Stock `check.py --nonmatching
--target-size 4176 --record` reports 99.6%, ten differing bytes and nine
realigned register instructions, zero register-blind differences. Its normal
assembly fallback passes the same explicit extent. Root checks are
`build/check/run-8mjc9bes` and `build/check/run-hj3bqw44` respectively.

Recovered source boundaries include actual stack aggregate ownership, four
BLK particle views, a plain final position owner, direct polar-table reads,
separate branch animation owners, and the actual narrow primary callee types.
The phase-two result alias and copy were unnecessary; writing the existing
vector directly restores PRE's table/output capture order.

The remaining nine rows are one phase-five reload rotation. A diagnostic
`AGBCC_SPILL=137:1` gives a whole match. More decisively, evicting native flags
pseudo 1930 before reload introduces an output reload in a phase-four negative
flag-update block that jump2 later deletes. That hidden reload fixes every
phase-five register row; its extra spill slot only changes the frame from
0x2C to 0x30. Thus the source problem is a flags/control value boundary that
needs the existing dead position slot, not nine independent register choices.
A typed control aggregate or read-control macro is the next bounded hypothesis;
that last macro hypothesis was not executed at the stop.

Disabling reload inheritance across the whole function on an isolated compiler
has no effect. The private compiler's identity and positive hook execution
were verified. Do not repeat the inheritance search. Reports and exact source
hashes live in `build/scope/campaign/sub_0814B1F8/round4/CURRENT.md` and
`round5-helper/REPORT.md`; private diagnostic artifacts remain ignored.

## Remaining clone pool: 08200C94

The improved guarded draft is commit `1a1ee07ba`. Stock explicit-extent scoring
is 96.1%, 146 differing bytes; anchordiff reports 18 differing instructions,
five register-blind. Scope's alignment metric is 15/15, shape four, 912 bytes.
These metrics differ because moved code/padding changes encoded branch and
call offsets. Neither metric is an exact match. Root stock evidence is
`build/check/run-6z5nzrcb`; its normal fallback passes in
`build/check/run-b7e1_hvl`.

The source now recovers the actual indirect callback call: `08249240` is a
`bx r2` thunk, not a semantic three-argument API. An eight-byte operation view
contains the typed callback and two halfword parameters observed in
`PlayerClone_InitConfig`; by-value extraction narrows to the single required
load and prevents the incorrect callback/late-attack address family. The
fabricated third callback argument and attack-offset workaround are gone.

Other recovered structure:

* A real area-work record saves the collider after the callback and the facing
  pointer after the movement-mask read. Both snapshots now occupy the target
  stack slots.
* The facing address is computed after the callback-release test, before
  `SetFacing`, in the same basic block as its snapshot. `last_use_this_basic_block`
  therefore permits its independent slot-derived walk. Indexed facing is
  rejected: its increment occurs before i++, contrary to the ROM.
* An unconditional animation owner, explicit palette setter helpers and the
  later metadata capture produce exactly the required state/facing/animation/
  body-timer initializer and increment order.
* One reused collider pointer gives the desired collider/angle/radius roles.
  A by-value collider-header flag test with a signed halfword mask restores
  the body-timer-relative load and mask-copy order without the old address PRE.
* An actual pair of projection vectors plus an early cached position reference
  gives every target stack slot. A late explicit delta reference puts its
  address capture before the facing load.
* A signed-byte direction getter preserves the actual field type, lengths and
  main register roles while retaining the word-sized facing mask.

Only the first X read in the final vector sum lacks the target's extra pointer
copy (`adds r1,r2,#0`). The draft instead uses a reload r5; that one extra reload
rotates the following attack block. Diagnostic `AGBCC_SPILL=36:4` fixes that
entire attack block, leaving only the vector-copy difference and padding.
The next source question is an actual by-value vector/component reader or
reference boundary, not more attack-field changes. The final 83-decision
canonical/PRE/GIV/hoist probe changes no count; it does not cover arbitrary
source types or all reload decisions.

Temporary local views and the mode-preserving member-offset helper remain in
this guarded draft. Shared-header unification is still phase-four work.
`build/scope/campaign/PlayerClone_UpdatePool/round4/REPORT.md` and
`round6-fresh/REPORT.md` retain the source frontiers and rejected mechanisms.

## Cleanup prepared, not applied

The requested cleanup was prepared in parallel and deliberately remains
separate from the two unfinished matches. No pin-removal or data/Studio patch
has been applied to tracked sources yet.

`build/scope/campaign/finish-player-cleanup/depin-fresh/REPORT.md` and
`verified-candidates.json` preserve stock-exact candidates and patches:

* Five freely removable pins across `0814B078` (one), `0814C248` (three) and
  `0814D9C4` (one), covering 360, 336 and 780 bytes respectively.
* `08146EAC` has a stock-exact 2,188-byte ordinary playback replacement for six
  pins and five barriers; a separate game-state address pin remains.
* Expanded `0814DF58` proves two removable `MODE_OFF` barriers over 132 exact
  bytes; specialize the shared macro rather than landing the expanded draft.

The ROM-order scan covered all 77 concession-bearing functions in this
subsection. Shared playback-family and record inventories are in
`build/scope/campaign/finish-player-cleanup/INVENTORY.md`. The seven orbit-effect
functions share a 0x3C particle record and 0x4C8 task layout; Clone records and
the playback-family views are the next shared-header candidates.

`build/scope/campaign/finish-player-cleanup/data-studio/` contains reviewed
`data-studio.patch`, optional `roundtrip-records.patch`, candidates and REPORT:
seven Player callback arrays (186 entries, 744 bytes), optional owner/selector/
dispatcher manifest metadata and a generic Studio callback-table browser with
Code/ROM links and cache refresh. All eighteen existing plus proposed tables
round-trip 1,080 bytes; the seven new tables retain 121 relocations and 65 null
slots. Fourteen focused tests, Python/JavaScript syntax and fixture UI checks
pass. The GUI check used a read-only fixture, not a complete live Studio index.

The high-ROM 90-word callback region is explicitly deferred: observed indices
cross its apparent label boundaries, and the manifest currently lacks alias
views. Do not describe separate physical labels as independently bounded
logical arrays. The eight-byte `08610E24` scalar range is not a callback table.

## Tooling and integration

The requested cherry-pick `02f36efc6` landed as `1c2fa2aed`, fixing current ELF
names in `check.py` and short matchsearch sockets. Scope resident caches from
before it must be bypassed with `SCOPE_SERVER=0`; use a new matchsearch context
when symbol/data classification changed.

Two further diagnostic gaps are recorded, not patched in production: built
scope compilers silently lack the `PREF`/`NOINHERIT` hooks found in patch source,
and fast source grouping misses a macro-defined old function name in this
renamed target-motion unit. Only isolated private diagnostic copies were used.
Generated named assembly can also be absent for newly promoted names; a local
read-only alias fixed target-motion jump-table classification. No generated
assembly, ROM, emulator dump or compiler binary is committed.

At integration, review the nine exact source commits and both guarded drafts,
finish the remaining C matches, apply/recheck the prepared cleanup, then run
one complete combined build (`build/boktai3.gba: OK`). Regenerate and commit both
progress reports once for that batch. This worker leaves them unchanged.
