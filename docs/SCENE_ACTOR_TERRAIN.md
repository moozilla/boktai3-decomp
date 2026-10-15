# Scene actor terrain transitions

The adjoining query subsection begins at `08076910`. Its narrow actor view
covers the current terrain cell at `2F4`, position `5C`, direction `59`, and
script parameters written at `1F4` through `20C`. Field roles beyond those
observed consumers remain provisional.

`76910` returns a neighboring cell, or minus one when its terrain flags,
slopes or height transition reject movement. Directions zero through three
add one, add the signed map width, subtract one and subtract the width;
other directions test the current cell. It reads each cell's slope type from
the high nibble, integer height from the low nibble, and blocking flags from
the halfword at offset two. Overrides returned by `0821D64C(cell,1)` provide
the same four-byte cell record at override offset four; otherwise the current
map's records begin at offset twelve. Type one slopes subtract the low byte
of world Z and type two slopes subtract the low byte of world X.

The query tests heights at cell centers and preserves separate source and
neighbor slope constraints, including redundant comparisons in the original
control flow. Different slopes permit an absolute height difference of 255;
two matching type-one or type-two slopes permit 256. The return remains a cell
index rather than a Boolean.

`76C80` reads keyword `I` and `D` values into words at `1F4` and `1F8`, narrowing
each to sixteen bits. If keyword `G` exists, it consumes five values into
`1FC..20C`; absent operands become zero. If the keyword itself is absent,
those five stored parameters are left alone.

The 880-byte neighbor query uses one shared register-one allocation hint in
`SceneActorTerrain_GetHeight`; removing it changes six bytes. Its three local
hints were all removed by `depin.py --write`. The script reader is ordinary C
and reproduces its 92-byte extent. Both use complete reviewed extent checks.
