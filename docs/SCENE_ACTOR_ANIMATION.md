# Scene actor profile animation selection

The reviewed selector family starts at `08076034` and ends at `08076910`.
It consumes the profile entries initialized by `08073CC4`; these are animation
records rather than generic behavioral states. Names remain provisional where
only bit behavior is known. Actor fields `308` and `2C4` select the profile ID
and mode; `59` is its direction, `2B7` caches the selected animation and `2B3`
requests a restart. A distinct animation-phase view avoids broadening unrelated
registry consumers.

`08076818` uses the entry value as a sequence index, state byte as a variant,
and flags byte as playback options. Bit `10` forces a tile animation restart
and is removed before forwarding. In the sequence drawable path, the animation
state is at drawable `48` with its resource pointer at `58`; the selected entry
value is cached. In the tile path, state begins at drawable `20`, and the
selected entry plus variant is cached. An unchanged selection can skip the
update when restart is zero and the current mirroring flag agrees with options
zero or one. Other option combinations deliberately proceed. Option bit four
chooses mode six rather than four for the tile loader; bit one controls tile
flag four. Both paths clear the actor restart byte after updating.

The hidden entry `080762A8` has its own prologue and return, ending at `080763D0`.
Its reviewed extent is 296 bytes; the preceding `08076174` extent is 308 bytes.
This boundary must be represented separately during integration rather than
crediting the raw hidden bytes to the preceding selector.

The first exact selector (`08076818`, 248 bytes) is ordinary C with no matching
hints. Its complete reviewed extent was checked against the private baseline.

The completed eight-function subsection adds 2,268 exact bytes, approximately
0.0939 percentage points: 804 bytes in ordinary C and 1,464 with minimized
allocation hints. Each selector has its own fast check over the entire reviewed
body, literal pool and padding; `build/scene-actor-animation-final.json` records
the final eight checks. The integrator still performs the combined ROM build.

| Address | Variant | Playback flags | Bytes | Hints |
| --- | --- | --- | ---: | ---: |
| 76034 | Direction fold | Mirror OR entry flags | 320 | 6 |
| 76174 | Direction fold | Mirror OR extra OR entry flags | 308 | 0 |
| 762A8 | Zero | Mirror OR extra OR entry flags | 296 | 1 |
| 763D0 | Zero | Extra OR entry flags | 248 | 0 |
| 764C8 | Explicit argument | Mirror OR entry flags | 304 | 3 |
| 765F8 | Explicit argument | Explicit argument | 256 | 1 |
| 766F8 | Zero | Mirror OR entry flags | 288 | 4 |
| 76818 | Entry state byte | Entry flags | 248 | 0 |

The direction fold uses `((direction + 0x60) >> 6) & 3`; quadrants greater than
one produce mirror one and variant three minus quadrant, otherwise mirror zero
and variant quadrant. Some selectors preserve both byte stores even when the
variant is unused. Explicit pointer lifetimes retain those original accesses.
Caller argument narrowing is explicit and the APIs use the canonical
`engine.h` declarations, avoiding duplicate animation setter prototypes.
The explicit-argument variants add the full argument to the tile selection;
only the sequence-path row is narrowed to a byte. The final tile mirror control
uses option mask `1`, distinct from playback mode mask `4`.

Allocation concessions retain pointer/value lifetimes and call-argument setup
order. `765F8` uses three empty-asm input references to the same initialized
flags value to change global allocation priority; reducing that to two changes
the actor/flags allocation. No hinted selector binds register seven. Each
hinted source was passed through `tools/depin.py --write` before promotion.
