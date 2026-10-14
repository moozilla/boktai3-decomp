# Contact swarm, 08043F64–08044C80

Gameplay identity is tentative. Static evidence establishes a second sixteen-slot
contact pool, 0x154 bytes per slot and 0x1590 bytes per task. Twelve new exact
functions add **0.098 percentage points** (2,368 bytes). Existing empty resource
callback, destructors and task factory gain types/names with no additional credit.

| State | Address | Observed behavior |
|---|---|---|
| 0 | 080441CC | Empty callback; independently confirmed by table 08605920 |
| 1 | 080441D0 | Enable health collider, move along heading at advanceSpeed, enter tracking after advanceFrames |
| 2 | 08044290 | Refresh heading toward player target every sixteen frames staggered by slot; enter charge with random heading jitter |
| 3 | 0804437C | Move along fixed velocity, refresh health collider, refresh resource collider on alternating slot parity, hide after frame 119 |
| 4 | 08044480 | Disable health contact, play E8 and reaction sequence, decelerate for twenty frames, blink and hide |

Movement precedes threshold tests. Advance with threshold N therefore moves on
N+1 updates; charge moves on frames 0 through 120. Health callback 08043F64 enters
reaction and takes direction from the source when its flags include 0x100,
otherwise deriving direction from the two contact positions. Resource callback
08043FAC is empty. Coverage tags are not new execution evidence.

Pool_Update (08044594, 536 bytes) exits if the active mask is zero. Otherwise it
copies player position plus 0x30, drives palette fading between banks +5A60 and
+5AE0, alternates resource contact parity, dispatches the five-state table, and
runs packed sequence playback for each initially active slot. A handler hiding a
slot does not skip its playback during that same iteration. The pool frame then
increments. Initialization resolves the same sequence resource as the preceding
pool and initializes all sixteen slots. ScriptProc_Start is the historical name
for resource lookup, not a script launch.

The shared header composes health collider, sequence, sprite and graphics views;
it preserves actual embedded objects instead of unrelated pointer overlays.
Resource collider is slot+14, health collider +68, sequence +BC, sprite +CC,
graphics +F8 and shadow +114. Pool target is +1588; fade fields +1564–1567 and
palette +1568. Slot state is signed. The factory already matched before this batch.

SpawnCharge (08044854, 224 bytes) finds the first free slot, starts state 3, copies
position and parameters, enables display/shadow, configures resource contact,
and disables health contact until entry processing. It returns zero even if full.
Its volatile final parameter preserves the observed stack-argument load timing.
The owner callers at 08025DEC and 080280CC pass their +9D4 pool pointer.

Two guarded drafts preserve paired advance spawning: 08044934 is 93.1% fuzzy
(420 compiled versus 416 original bytes); 08044AD4 is 86.5% (428 bytes each).
They alternate headings direction±96 with random ±32 jitter, randomized advance
duration, and per-slot tracking delay. Two random-table reads update the shared
counter only after the second increment. The second wrapper accepts a signed
slot count without a bound check. Both initiate palette fade at level 20.
Fuzzy scores do not establish correctness and these drafts receive no exact credit.

Hidden boundaries at 080441CC, 08044934 and 08044AD4 are recorded in build and
progress inventories. The first is an actual table callback; the latter two have
independent prologues, returns and ten-word literal pools. This prevents the
single-slot spawn from receiving credit for its former assembly tail.

All exact additions passed reviewed full-extent checks and hint minimization.
Advance and hide became unpinned. Remaining hints are marked concessions, with
no r7 bindings or handwritten instruction bodies. Failed allocation and type
experiments remain in ignored build/candidates. Claude's latest REGALLOC.md explains
spill-register rotation and literal versus symbolic RAM reloads; read it first
when resuming the paired-spawn register mismatch.

Next contiguous block begins at 08044C80: already-exact bitfield readers/writers,
a seed generator at 08044D58, and remaining encoded-record routines from 08044D6C.
