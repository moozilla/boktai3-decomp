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
