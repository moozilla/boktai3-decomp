# Tether actor emissions, palette and movement effects, October 14

The contiguous **080489A0–08049168** subsection is fully exact: nine newly
exact functions and the typed existing combined update. Exact code rises
**23.836% → 23.917%**, **+0.0813 percentage points** (+1,964 bytes). There are
**6,479 / 11,471 functions** and **577,678 / 2,415,354 exact bytes**. Without
register pinning: **19.515%**; Register-pinned: **4.402%**, included above.
Seventy-three guarded drafts remain excluded from exact credit: the existing
**08048E0C RGB blend draft** is now an exact match.

| Subsection | Newly exact functions | New exact bytes |
| --- | ---: | ---: |
| Directional, random, ring and point emissions | 4 | 976 |
| Glow, blend and palette selection/controller | 4 | 688 |
| Movement effects | 1 | 300 |

Four new matches use no hints: random emission, ring emission, palette
controller and movement effects. Five retain minimized concessions:
directional emission (one empty barrier), point emission (ten hints), glow
(one), RGB blend (one), palette selection (one). The point helper uses a
packed-vector/word union to preserve the source xy word and z padding while
updating coordinates. No r7 binding or handwritten instruction body is used.

## Effect emission

The first three methods pass packed position/velocity/extent vectors and
fixed trailing arguments **1/3/4** to **08068124**, a wrapper that appends a
zero argument before calling **08067E3C**. The meaning of each trailing
parameter remains unassigned. Sine products divide by **4096 toward zero**.
Coordinates wrap through sixteen-bit storage; padding is preserved.

- **080489A0 directional:** angle `((heading+1)&7)*32`, origin displaced by
  **100** along sine/cosine, unchanged y, velocity **10**, and extent
  **(32,0,32)**. xy components are read before constructing their packed word.
- **08048ABC random:** RNG low byte chooses an angle, velocity magnitude
  **32**, origin displaced by **four velocity steps**, zero extent and sound
  **D3** when the angular low bit is one. Unsigned halfword offsets reproduce
  the original's modulo-sixteen-bit coordinate arithmetic without extra
  signed loads. A named angular mask and random-byte address match naturally.
- **08048BA8 ring:** eight angles `index*32`, velocity magnitude **16**, origin
  displaced by **eight velocity steps** and zero extent. The angular temporary
  is masked in place before the second lookup. No hints remain.
- **08048CB0 point:** clones the caller's packed position, then applies RNG
  offsets **x/z [-15,+16]**, **y [-79,-16]**. Its second parameter is a
  **particle-configuration pointer**, forwarded to
  **`AnimatedParticles_Create(1, &position, config)`**. It creates one
  animated particle; the factory may consume further RNG samples according
  to that configuration. The original signed y is retained in the cached
  xy word while x/z jitter wraps through halfword storage.

The fast checker initially accepted the old numeric name `sub_080154EC` by
address, while full linking requires the existing canonical name
`AnimatedParticles_Create`. Full-build review exposed this and also clarified
the configuration-pointer contract. Both are corrected and revalidated.

## Palette and glow

The actor embeds the previously recovered **DisplayAnimation at 0x8C**.
A narrow union view exposes its sequence-kind palette-data pointer at
**actor offset 0xA4**, without changing the shared display wrapper. The
legacy engine names `DisplayAnimation_SelectTiles` and
`DisplayAnimation_SetTileSource` are retained, but these consumers establish
palette selection/source behavior; broader API renaming can follow separately.

Selection at **08048EEC** uses fixed index **408** for display kind two. Other
kinds select **index+0x283**, copy sixteen halfword colors to **actor+0x24** and
redirect the display's source to that local buffer.

The red-glow method at **08048D70** controls **palette entry one** with
`31*frame/64`, using fields **glow mode 0x38A / frame 0x38C**:
mode zero decays toward zero, mode one ping-pongs over a 128-update cycle,
and mode two grows to 31 and holds. Unknown modes leave it untouched.

The promoted **08048E0C** blends sixteen RGB555 colors between palette
**paletteIndex+0x283** and fixed palette **0x28B**, using weight at **0x394**
and inverse **64-weight**. Each channel is
`((inverse*source + weight*target) >> 6) & 31`; the result contains fifteen
color bits. Widening words before the two component shifts reproduces the
original extraction. A single component-mask r8 hint remains after minimization.

The palette controller at **08048F40** skips actor kind nine entirely.
A request byte at **0x472** or hold value at **0x46E** moves the blend weight
up toward 64; otherwise it moves down toward zero. A nonzero override timer
at **0x390** has priority, choosing palette nine for sequence display or zero
for kind two, and decrementing the timer. The comparison remains against
nine even for kind two, so that path may select its fixed palette repeatedly.
Without override, a nonzero blend selects the blend sentinel eight and updates
the buffer; zero blend restores the normal palette if needed and advances glow.

## Moving effects and ownership

Fields **0x39A moving / 0x3E4 suppressed** gate effects; an inactive or
suppressed actor clears its effect frame at **0x396**. Sound cooldown at
**0x398** decreases on every update. Kind five plays **26A** only at movement
start when cooldown is clear, then sets cooldown to **100**; it does not take
the normal periodic-emission path.

Other kinds play a sound on the first effect frame. Signed x/z high bytes
select the terrain cell, with out-of-range coordinates selecting cell zero.
A dynamic cell descriptor from **0821D64C(cell,1)** overrides the base map's
four-byte cell record. Surface high-nibble values one/two choose **D3**;
otherwise kind nine chooses **4D8**, and other kinds choose **D2**. Directional
effects emit whenever the effect frame is a multiple of eight, then that
halfword counter increments. A shared position pointer and explicit stop path
match without hints.

The existing **0804914C combined update** now takes the recovered actor type
and calls palette update, moving effects, then segment interpolation in that
order. The earlier **08048644 interpolation draft** remains 77.3%, with no
exact credit. The tentative actor name and identity remain unchanged.

## Validation and continuation

The combined build printed **`build/boktai3.gba: OK`** in
`build/tether-effect-combined.log`. **15 complete extent checks** passed after
promotion: all nine new matches, the combined update and all five existing
exact users of the refined actor header. The earlier interpolation was
checked separately with `--nonmatching --record` and remains **77.3%**.
Format, inventory, RAM literals, actor registry, all ten resource archives and
the text-bank round trip pass.

Claude's main updates include **PR 119** (another register-hint removal),
**PR 120** (26 charmap corrections), **PR 121** (graphics studio), **PR 122**
(`make english`) and **PR 123** (map decoding/viewer). The charmap correction
made this worker's extracted `build/text/bank.tsv` stale; it was preserved
as `build/text/bank-before-charmap-correction.tsv` and re-extracted from the
ROM with the current map. Default builds then encode byte-exactly again.
The English and Maps additions do not change the validated default C/build
pipeline and are incorporated without repeating the build for documentation
or separate preview/translation targets.

Continue contiguously at **08049168** through the actor's entry/state and
interaction methods. The existing state setter awaits typing. The **68-byte
08049180 heading-entry helper** is already exact without hints in ignored
scratch work; **080491C4 cardinal flags** and **08049234 dispatch** retain
readable nonmatching scratch candidates. No new emulator observations are
claimed. Continue toward **25% exact or 60% usage remaining**.
