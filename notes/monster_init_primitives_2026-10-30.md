# Shared initializer script and terrain operations

All 17 Round 6 initializers match under the stock compiler after this extraction:
67,936 bytes, with zero differing bytes in every full translation unit. The common
operations live in `include/scene_actor_init_primitives.h`. There are no per-species
selectors or separate copies of the terrain-height calculation.

## Shared contracts

- `MonsterInit_ReadWord`, `ReadByte` and `ReadHalfword` read an optional script operand
  with a fallback in the corresponding value domain. `ReadKeywordHalfword` performs
  the same narrowing for a named keyword result. The word reader remains signed;
  limit fallbacks and negative sentinel values require that domain.
- `MonsterInit_ClearPosition` clears the canonical `ContactPosition` through the
  existing word-fill operation.
- `MonsterInit_CellIndex` returns a word index after interpreting stored x/z
  halfwords as signed 8.8 coordinates. `MonsterInit_CellId` narrows that result for
  the actor's stored halfword ID. These operations share the index calculation.
- `MonsterInit_TerrainHeight` is the only height calculation. It resolves a cell
  override or the current map cell, reads the shape nibble and base-height nibble,
  then subtracts the x or z fractional byte for the two observed slope shapes.
  Its signed result permits a slope below the zero-height base.

`ContactPosition` and `EngineMap` are the existing canonical records. Their
unchanged definitions now live in small `contact_position.h` and `engine_map.h`
headers. The original owning headers include those declarations. This avoids
importing unrelated engine function prototypes just to use a position or map.

## What the common implementation exposed

The first shared terrain implementation was already exact in 11 of 17 units.
Four more differed only in stack operands. Slime and Ghost also exposed two
concrete type-boundary problems; neither required a second terrain implementation.

| Consumer | Recovered contract |
|---|---|
| Slime | The stored terrain-cell ID is `u16`, but its query returns a word. This preserves the target's `STRH` followed by `LDRH` before the height call. Its targeting-flags query also returns a word for mask arithmetic. |
| Bee | Palette base/offset queries return words for index arithmetic. The F/R keyword operands use the common halfword reader, matching their explicit halfword interpretation. |
| Ghost | Route-coordinate queries return the raw halfword bits in a word. The following position stores and signed cell calculation can then reuse the loaded values. Palette and targeting-flags queries use word arithmetic domains. |
| Lun | Palette base/offset queries use word arithmetic domains. |
| Tentacle | The targeting-flags query uses a word arithmetic domain. Its stored flag byte remains `u8`. |
| Mimic | The range-radius query returns a word for the squared-range calculation. The stored radius remains a 16-bit field. |

These changes preserve the actual field widths. A field's storage type and the
value type of an operation are separate contracts.

## A small compiler example: storing a cell ID and querying it

The useful comparison is between two typed versions of the same operation:

```c
actor->terrainCell = MonsterInit_CellId(position);
return MonsterInit_TerrainHeight(GetTerrainCell(actor), position);
```

With a halfword return from `GetTerrainCell`, the compiler could forward the
already narrowed result: it emitted shifts to narrow the cell value and omitted
the target's load. A word-valued query restored the observed sequence:

```asm
strh cell, [destination]
ldrh cell, [destination]
```

The stored member remains a halfword in both versions. The query boundary decides
which value the optimizer carries between the store and the terrain operation.
The same contract had already been recovered in the grouped monster initializers.

## A larger compiler example: the stack permutation after sharing terrain

After the physical instructions matched, the PRE stack model gave exact allowed
instruction-count intervals. This distinguishes a type/interface issue from a
semantic or instruction-selection error. The relevant count is the instruction
count immediately before GCSE, not the final assembly length.

| Consumer | Count with shared terrain before query fixes | Nearest required interval |
|---|---:|---:|
| Slime | 1332 | 1328–1331 |
| Bee | 1284 | 1276–1279 |
| Ghost | 1818 | 1804–1807 |
| Lun | 1510 | 1480–1503 |
| Tentacle | 1262 | 1256–1259 |
| Mimic | 1713 | 1708–1711 |

For Bee, using word palette queries reduced the count to 1275. That was still
outside the required interval even though the final instruction shapes matched.
Replacing the two explicit F/R halfword casts with the shared halfword-keyword
operation completed the match. There is still one terrain implementation and one
copy of each reader.

The intervals guided inspection of real value boundaries. No unused expressions,
duplicate flag writes, register pins, inline assembly, compiler overrides or
per-species code-generation selectors remain in this extraction.

## Validation

For each initializer, the stock full-unit check was run as:

```sh
PATH="$PWD/build/binutils/usr/bin:$PATH" \
  build/venv/bin/python tools/check.py src/fn/sub_ADDRESS.c --quiet
```

The final check covered Bat, Ghoul, Slime, Bee, Ghost, Lun, Crow, Spider, Tentacle,
Vorn, Skeleton, Golem, Mummy, Mimic, Octopus, Ivy and Axe. Every result was `MATCH`.
The combined ROM check belongs to the coordinating worktree.
