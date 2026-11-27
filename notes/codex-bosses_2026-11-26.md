# Boss actor layouts and state transitions

Tags: #bosses #types #matching #phase3

| Object | Printed warning lines | Weighted steering uses |
|---|---:|---:|
| Vanargand | 332 → 318 | 418 → 405 |
| Sabata | 142 → 139 | 159 → 156 |
| Hresvelgr | 142 → 140 | 149 → 148 |
| Ratatosk | 120 → 116 | 131 → 123 |
| Nidhoggr | 117 → 112 | 132 → 127 |
| Count | 105 → 104 | 111 → 110 |
| Bosses | 958 → 929 | 1,100 → 1,069 |

All 45 changed translation units and all 183 dependents of changed headers
match in stock checks. The boss scan reports zero linter errors. Code
matching stays at 100%, a gain of zero percentage points. The combined ROM
build belongs to batch integration.

## Recovered code

- Vanargand uses indexed teardown for the paired halves (081B1E24–081B5D8C)
  and a size-checked damage part with native sprite, sequence, collider, and
  target fields. The charged attack (081ABA64) shares the signed fixed-point
  projection across five coordinates. Twenty-nine part transitions use
  the existing action-phase operation.
- Sabata's 0xC68 actor shares its animation resource, sprite, colliders,
  palette banks, and tracking state between presentation, damage handling,
  and script dispatch. The turn callback uses the shared four-way and
  eight-way facing conversions.
- Hresvelgr's 0x7E8 actor uses the native sprite and three colliders. Its
  0x90 shot uses one checked layout for creation, contact initialization,
  and update. The collider preparation at 081A2858 keeps its signed
  halfword read, and the target setup at 081A0558 uses the existing DMA fill.
- Ratatosk's presentation uses the 0xF30 actor and five direct frame resets.
  The tracking-shot dispatcher uses `ContactPosition`, and selected
  transitions use the existing phase and action entry points.
- Nidhoggr's duel and ship layouts expose the actor body, sprite resource,
  effect pool, and target registry. The nine-part health reset and 26-part
  teardown match as indexed loops. Four ship and duel effects use the
  engine's combined pause query.
- The Count's presentation (080289FC), damage handler (080284FC), and
  constructor (08029798) share the 0xA10 actor. Animation visibility uses
  `AnimSprite_Hide` and `AnimSprite_Show`. The interpolation amount keeps
  the byte view that gives the ROM's unsigned load and sign extension.

## Remaining compiler decisions

Vanargand's part initializer (081A7040) still selects a different
palette-key store base after CSE with direct typed fields. An indexed
update at 081B5D8C changes the saved member induction values and the
timer's allocation. Sabata's launch helpers and Hresvelgr's collider
loop still require the ROM's immediate-before-field-load order. A direct
quadrant calculation at 081A0BC8 changes the overlap between the palette
bank and the random mask.

Ratatosk's dispatcher (081F6B90) still exchanges the receiver and loop
counter in allocation with a fully typed receiver. Removing the one-pass
loop in Nidhoggr's duel contact callback lowers the effect pointer's
priority below its peer collider. A compiler diagnostic reproduced the
ROM order by restoring that priority.

The Count's indexed pool walk (08043C4C) matches the instruction set but
moves the index initialization and increment relative to the slot
address. Loop strength reduction emits the slot address increment first.
The direct palette-blend resets at 080289FC made the draft four bytes
longer. The signed blend member also emits a different load.
