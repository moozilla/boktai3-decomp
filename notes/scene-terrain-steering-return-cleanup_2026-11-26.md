# Terrain traps, steering, and attachment return

Tags: #matching #types #phase3 #scene #terrain

The final terrain, steering, and attachment pass recovered seven report
lines and six charged constant uses. The checked source stays 100% exact,
a change of zero percentage points.

- Terrain trap registration and removal use the map's declared trap
  arrays. Contact tracking and dispatch share a checked 0x30-byte
  TrapProbe with its position and ten serials (0821DE30 and 0821E040).
  The probe header's two dependents match. The native status field in
  Terrain_ApplyBlocked replaces the parked mask and output zero in
  the 3180-byte 0821F15C unit.
- NavMesh_FindRoute's two counts start at one because each region list
  is seeded with its containing region. The gather macro increments
  both counts. The linter counts eight initial-value reads because the
  assignments inside the macro are opaque to it. The inline gather
  trial changes global register allocation and branch layout. At
  0821E5C4, a plain split between the route definition and point count
  makes the bank and definition contend at equal allocation priority.
- The steering collider and fixed travel origin now use ContactPosition
  directly (0806BBD0 and 0806CF4C). Settled slots use an indexed state
  loop (0806CF18). The typed pool base and direct destruction cursor
  change the address or branch sequences in their remaining callbacks.
- The room query (0806F140) keeps its original test after direct forms
  were tried. The nearest variant materializes the player status mask
  after the flag word load. The ROM loads the mask first.
- The attachment return follow-up (0807CA44) uses the declared chase
  group, home route, contact and delayed-response record and reuses
  the terrain height and route operations. The by-address route read
  still preserves the ROM's allocation. Sharing the two coordinate
  calls changes branch and value allocation. The complete 828-byte
  unit matches its ROM extent.
