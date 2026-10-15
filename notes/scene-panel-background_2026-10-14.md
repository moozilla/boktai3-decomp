# Scene panel background effects and overlay setup

The subsection080586C0–0805907C contains six functions /2,492 bytes. Five are
exact C, totaling1,436 bytes; the1,056-byte overlay renderer58BD8 remains a
scratch draft and is excluded from exact progress. Newly exact functions add
1,372 bytes /0.0568 percentage points:280 without hints and1,092 with minimized
concessions. The existing64-byte burst entry receives typed contracts only.
These are static observations of the same tentative ScenePanel object.

Background effect58700 is1,012 exact bytes. It uses GameState.unk618 to select
between a countdown alpha/shake path and two palette/particle paths. The object
has effect frame1328, signed shake displacement132A, center510 and phase1378.
The countdown and terminal burst clear display-shadow bit200 when exhausted;
active effects set it and write BLDALPHA. Full timings, RGB palette selection
and RNG store ordering are documented in docs/SCENE_PANEL_EFFECTS.md. Starting
a burst sets frame64, displacement0 and phase1, then plays sound297.

Overlay initialization58AF4 stores raw C091/AD57 at4DC, copies its24-byte header
locally and relocates three local pointer fields by adding the original base.
The local relocation is retained even though this function does not subsequently
consume those fields. It sets display-shadow bit800, configures layer3 through
08215A74(3,1), stores92B3/E2BA+114 at4E0, and initializes each of four target/current
levels at4E4/4E8 to5. Its148-byte body matches without hints after minimization.

Palette cycle58B88 increments a halfword counter4EC. On odd wrapped values it
copies32 bytes from one of four successive palette banks to03005120, using
bank=(frame>>1)&3. Its80-byte body preserves redundant mask/copy operations with
five minimized concessions. Full-word intermediate lifetime and halfword wrap
remain explicit; no instruction body or r7 binding is used.

The132-byte layer driver58FF8 is unpinned. It adds camera offsets to03005418's
X/Y, computes truncation-toward-zero tile coordinates from (coordinate-128)/8,
clears the low bit of tileY, and uses screen offsets X-120/Y-80. It forwards all
five arguments to the background effect and then the still-unmatched overlay
renderer. The latter's draft follows four clipped quadrants and paired tile rows;
its control/data interpretation remains under review and receives no credit.

Hex addresses/offsets/flags above omit0x; byte counts and timings are decimal.
No new execution coverage or final in-game identity is claimed.
