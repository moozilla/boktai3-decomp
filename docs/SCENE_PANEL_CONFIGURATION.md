# Scene panel script configuration and subject creation

The reviewed span `0805BCEC–0805C310` contains seven exact functions, five
new (1,472 bytes, +0.0609 percentage points) and two existing (100 bytes).
The partial `ScenePanelConfigView` records fields observed in these consumers;
it does not establish the complete allocation or identify a particular scene.

`BCEC` initializes the main palette sprite unless byte C0F suppresses it.
Resource C039 supplies graphics. Sixteen palette halfwords are copied into the
sprite's embedded palette at offset 48, then its graphics palette pointer is
redirected there. This is distinct from the rig sprite's sequence at offset
48. Render mode is three, the three transform halfwords are zero, position is
the panel origin, and active is one. This function retains five minimized
register bindings; no explicit r7 binding or instruction body is present.
`BD78` initializes the auxiliary sprite from resource 3641, sets flag two,
parameters eight/nine to 96 and render mode one, then places it 72 units above
the center. Its code is ordinary C.

`BDD0` reads optional keyword g into two creation arguments (defaults 100 and
zero). Keyword a supplies one initial state per subject; absent initial states are zero.
Each subject slot is cleared before reading its initial state. Keyword S supplies a
halfword parameter, default zero. The category-one flag is passed to each
creation call. Copies of the origin receive the following X/Z offsets, sequence row/variant arguments, and an appearance delay:

| Slot | X | Z | Row | Variant | Delay |
| --- | --- | --- | --- | --- | --- |
| 0 | -514 | 76 | 1 | 0 | 6 |
| 1 | 514 | -76 | 0 | 1 | 9 |
| 2 | 76 | -514 | 1 | 1 | 3 |
| 3 | -76 | 514 | 0 | 0 | 0 |

Y and the copied position padding are preserved. The recovered factory at 0805DCAC forwards row, variant, initial state, kind,
contact limit, collider setting and appearance delay to the subject initializer.
The formerly tentative direction code is an appearance delay; profiles supply
initial states. The g arguments are contact limit and collider setting.
The original assumes the stored subject count fits the four local profiles.
Source case order and the scope of the shared negative offset recover the
entire routine without pins or barriers.

`BFC8` optionally reads keyword c into origin XYZ, then sets approach position
to origin plus (324,0,324), and center to origin minus (20,0,20). The 508 field
is the same player approach destination used by the interaction cohort.
`C054` reads keyword p into property 17AC, default zero.

`C084` reads keyword b into a resource index and two global resource values;
defaults are zero and 6000/6000. Warning stage is zero when current is greater
than reference/4, otherwise one. Five resource indices select four warning
sound IDs; other indices clear them. Keyword T selects the resource ID,
default ten, before initializing the scheduled action. The default path uses
a volatile pointer slot to preserve the observed fresh global reload between
stores. Resource ownership beyond these accesses remains tentative.

`C2DC` requires every counted subject to have state zero; the empty set passes.
All six functions besides BCEC use ordinary C. Full reviewed extents were
checked before integration; the combined ROM check is recorded in HANDOFF.
