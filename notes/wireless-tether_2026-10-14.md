# Wireless link indicator and tether actor, October 14

The **080483A0–080485FC wireless indicator** is fully exact. The next
**080485FC–080489A0 tether/auxiliary-sprite subsection** has four new exact
helpers and a guarded interpolation draft. Names for the latter actor remain
tentative.

Exact code rises **23.807% → 23.836%**, **+0.0288 percentage points**
(+696 bytes, seven new functions). There are **6,470 / 11,471 functions** and
**575,714 / 2,415,354 exact bytes**. Without register pinning: **19.464%**;
Register-pinned: **4.371%**, included above. Seventy-four guarded drafts remain
excluded from exact credit.

| Family | New exact functions | New bytes | Existing functions typed |
| --- | ---: | ---: | ---: |
| Wireless link indicator | 3 | 392 | 6 |
| Tether/auxiliary sprite | 4 | 304 | 1 |

Six new functions need no register hints. Only the **144-byte RFU status
update** retains concessions: the indicator pointer in r4 and a one-bit
connected-slot mask in r6. Minimization removed all nine hints from the
strength-frame selector and the sole hint from the shake helper. No
handwritten instruction bodies were used.

## RFU wireless link-strength indicator

`include/wireless_link_indicator.h` describes a **0xA4 task on list 11**, with
singleton **gUnk_020000F4**, overall frame counter at 0x18, animation tick at
0x1C, cached frame at 0x20, relocated resource at 0x24 and a **0x60 animation
entry at 0x44**. Resource **CB05/7C8B** is loaded and relocated before animation
initialization with arguments **34/17/0/0/60/NULL**. Screen position is set by
two halfword stores to the entry's x/y fields. Destruction unregisters the
animation and clears the singleton.

`RfuLinkStatus` in the reconstructed SDK header establishes the source:
**gUnk_03006A80**, parent/child mode at 0, connected-slot flags at 2, and four
signal-strength bytes at 0xA. The link task's `sub_08036A3C` supplies the
present/active predicate and `sub_08036A10` selects the child's slot. Child
mode reads that selected strength; parent mode scans four connected slots and
uses their **minimum** strength. A parent with no slots supplies zero. Missing
link status hides the animation via flag one. A nonzero `sub_08036BEC` flag
freezes the status update entirely.

The selector increments an independent tick, masks it to a **32-update phase**,
and loads a keyframe only when it differs from the cached frame:

| Strength | Frames over phases 0–31 |
| --- | --- |
| >228 | 34 for 0–7, 35 for 8–15, 36 for 16–23, 37 for 24–31 |
| 127–228 | 34 for 0–9, 35 for 10–19, 36 for 20–31 |
| 25–126 | 33 for 0–9, 34 for 10–19, 35 for 20–31 |
| 0–24 | 34 for 0–15, 33 for 16–31 |

The independent **52-byte helper at 08048438** reads child strength slot zero
without the task predicates and hides the entry in parent mode. Its hidden
boundary already existed in build/progress metadata, so there is no inventory
or byte-denominator change. Its descriptive alias stays file-local because
the generated disassembly lacks a function-start label there.

## Seven-segment player tether and auxiliary sprite

Static constructors at **0804D5CC** allocate a **0x484 list-9 task** and retain
it in **gUnk_02000488**. Registry entries include its constructor and an
alternate constructor. The global identity is not established from the hash
alone. `include/tether_actor.h` records only fields supported by this cohort:
world position at 0x50, heading at 0x260, **seven 0x28 effect segments at
0x264**, palette index at 0x37C, opposing shake values at 0x37E/0x382,
shake counter at 0x388, auxiliary ownership byte at 0x39B, auxiliary sprite
at 0x39C and graphics at 0x3C8. Other ranges stay explicit unknown bytes.

Each segment initializes against resource **1C1C**, with offsets **(-4,-4)**,
priority byte 255 and hidden flag one. Teardown removes all seven from the
effect list. The auxiliary sprite is created only when its ownership byte
is zero, uses resource **1C1B**, flags **512**, offsets **(-8,-8)** and frame
six, then marks owned. Removal tests ownership **equal to one**, unregisters
it and clears the byte.

During the final quarter of each **128-update shake cycle**, RNG bit zero
chooses **-5 or +6** for x and the opposite for z. Sound **2AA** plays when
phase first reaches 96. The counter increments regardless of phase.

The **596-byte interpolation at 08048644** is a **77.3% guarded draft**.
Its readable C recovers the following behavior; register allocation and
packed-endpoint caching remain different and earn no exact credit:

- Player slot zero's byte **0x457 equals six** to show the tether; every other
  mode hides all seven segments. This byte is distinct from `player.h`'s
  existing action-mode field at 0x418. Player heading/side at **0x3A2/0x3A3**
  are also distinct from its facing byte at 0x3A4. A narrow player view keeps
  those interpretations separate.
- The player endpoint uses heading `(heading+1)&7`, 20-unit sine/cosine
  combinations chosen by the side byte, and **y+220**. The object endpoint
  uses heading `(heading+5)&7`, a **100-unit** sine/cosine offset and **y-50**.
- For segment i=0..6, each coordinate is
  `(start*(7-i)+end*(i+1)) >> 3`. x/y use incremental accumulators; z uses
  explicit weighted multiplication. Negative interpolation rounds downward;
  the sine-offset normalization divides by 4096 **toward zero**. Endpoints
  truncate through signed sixteen-bit packed coordinates before interpolation.
- Priority is **-20** for player heading four, otherwise **-2**, stored as a
  byte. Every visible segment clears hidden flag one.

The latest exploratory candidates remain ignored under `wip/` and
`build/candidates/`. They include a version whose control-flow normalization
matches more closely but whose packed-coordinate reloads still differ. No
new emulator claims.

Continue contiguously at **080489A0**, the directional-effect request that
uses object heading, radius 100 and velocity 10. Its first readable scratch
candidate has different register allocation and extent. The following
**08048ABC** uses a random direction and speed 32, with optional sound **D3**.
Further actor behavior runs through the 0804Dxxx constructors; examine static
consumers before asserting the actor's gameplay identity.

## Validation and integration

The combined build against **f475e4bf** printed **`build/boktai3.gba: OK`**
in `build/wireless-tether-combined.log`. **21 complete extent checks** passed
after name promotion: seven new matches, seven typed existing helpers, and
seven renamed callers. The interpolation was checked separately with
`--nonmatching --record` and remains **77.3%**. Format, inventory, RAM literals,
actor registry, all ten extracted resource archives, and text-bank round trips
pass. Claude's **PR 115** (resizable resource archives) and **PR 116** (local
text studio) are incorporated. Main CI for PR 114 (**38618538320**) passed.

The latest allowance is **77% remaining**. Continue toward **25% exact or
60% usage remaining** in the same isolated worktree, with batched validation.
