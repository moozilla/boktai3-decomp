# Orbit-positioned background controller

Five newly exact functions add **1,700 bytes / 0.0704 percentage points**.
The complete **08050820–080510B8** subsection is now exact: 2,200 contiguous
bytes across seventeen functions. Four new functions retain minimized hints;
the192-byte initializer needs none. Existing controls and lifecycle functions
now have names and shared types. `OrbitBackground` is a provisional identity;
its observed role is positioning a BG1 image from player/camera coordinates.

The0x38-byte task contains resource18, ID1C, player index1E, enabled1F,
targetXYZ20, positionXYZ28, angle30, signed radius32, interpolation counter34,
camera-hold flag36 and special-mode flag37. It is a singleton behind0200048C.
The resource loader looks up map resourceB12, initializes BG1 and gates its
display-control mask200. The resource pointer is an actual resource lookup;
it is distinct from the integer script IDs found in the preceding actors.

## Recovered behavior

**50910 (216)** handles visibility. In link mode (engine flag800), select a
signed player index at session+18 and inspect the value at session+5C+index.
A zero value enables/reset-orbits; a nonzero value disables. Outside link mode,
global0528 equal1 suppresses display via the special-mode flag; leaving that
mode resets an enabled orbit. The final state controls display mask200.

**509E8 (980)** updates the world position in three modes:

- Camera hold interpolates toward the first three camera halfwords for seven
  updates, then copies them directly and sets the counter to8.
- A player targetActive halfword at4B6 selects its targetXYZ at4A8. It sets
  angle from GameState85E, adding128 modulo256 during transition4, and uses
  the same interpolation/copy sequence.
- Otherwise, smoothly follow the global angle and orbit around the player.
  A near-opposite direction uses an alternate angular adjustment, contracts
  the radius via `(3*radius-300)>>2`, and flips the radius/sign when negative.
  Other directions turn through the shorter arc and grow radius toward300.
  Compute X/Z using the256-entry trig table, with products truncated toward
  zero. Y combines player Y with `(128-playerAF2)*150>>7`.
  A counter above1 interpolates toward the orbit target while counting down;
  otherwise copy the orbit position directly and clear the counter.

Preserve signed shifts, angle masks, halfword reloads and the two distinct
interpolation schedules. The near-opposite radius test narrows to signed16
before comparing. No explicitr7 binding or handwritten instructions were used.

**50DBC (212)** gates on visibility, updates the position, projects XYZ using
signed3/32 scaling, subtracts screen offsets, and calls the existing BG helper
with tile offsets and screen placement. Vertical tile offset is forced even.
The helper's existing name is `BgGfx_LoadTilemapSimple`; its broader API naming
is outside this batch. The local projected vector remains readable C.

**50F40 (192, no hints)** initializes resource/orbit state, places the initial
position on the opposite side of the orbit, runs one tick and installs the
singleton. **51054 (100)** reads player keyword `i` (default0) or link-session
selection, and enabled keyword `m` (default1), then calls the creator.

Controls50820/50840 hold/release camera following with counters0/8. Enable50878
resets a previously disabled orbit; disable5089C clears enabled. The query
returns0 while special mode is active. Reset508F0 copies GameState85E, sets
radius300 and counter0. The singleton clearer deliberately only clears its
pointer; no additional resource destruction was invented.

The combined orbit/area build printed `build/boktai3.gba: OK`; 51 complete
post-promotion extent checks, format/inventory/RAM checks and all twelve data
round trips passed. Aggregate exact progress reaches **24.682%**, up
**0.1726 percentage points** (+4,168 bytes).
