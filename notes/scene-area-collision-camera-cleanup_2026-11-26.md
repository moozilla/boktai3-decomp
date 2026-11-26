# Scene area, collision, lifecycle, and camera cleanup

Tags: #matching #types #phase3 #scene #camera

All changed translation units remain byte-identical. Exact source coverage
remains 100%, a change of zero percentage points. These six objects have
nine fewer report lines and nine fewer charged constant uses, with zero
linter errors.

| Object | Report lines | Charged uses |
| --- | ---: | ---: |
| Areas | 13 → 11 | 13 → 11 |
| Standalone actor units | 12 → 9 | 11 → 8 |
| Collision | 10 → 10 | 10 → 10 |
| Lifecycle | 10 → 8 | 11 → 9 |
| Route sprite actor | 8 → 7 | 9 → 8 |
| Camera | 8 → 7 | 8 → 7 |

- Area effect callbacks read typed positions and clear or reveal sprites
  with indexed entries at 0804E87C and 0804E8D8. Both area setups use the
  declared manager. The typed containment scan inserts an extra member
  base before loading the entry bounds, so its private entry view stays.
- The five emitters in the 0811EE3C unit use one emitter record and the
  actor embedded at owner +0x18. The low variant's timer overlaps the last
  halfword of the effect profile. The standalone route utility shares the
  existing route check, and the ambient particle cursor advances directly.
- The collider unit (0821FDC8) uses ContactCollider and ContactPosition.
  The scene body position offset at +0x34 is an eight-byte ContactPosition,
  verified in all 640 header dependents. Stimulus posting (0822B6C8) uses
  indexed slots, and exclusion clearing (0822BAA0) takes its endpoints
  from the ID array. A direct zero in posting moves past the flags mask.
  Removing the one-pass schedule loop changes allocation among the
  quota, delay, mask and enabled count.
- The lifecycle pause checks share Engine_PauseBits and use the declared
  engine and initialization words. The update at 082250FC uses the native
  task, GameState and input fields. Direct pause writes would change the
  placement of the mask and word address after sound dispatch.
- The route sprite planner (0806D438) uses the player position, route point
  table and direct start height. The pool uses the declared 0x1C-byte
  graphics type, verified in all 19 header dependents. The native waypoint
  array expression changes allocation around the curve generator.
- The camera task at 082275D8 allocates 0xA4 bytes. The controller header
  now describes the common record. Its two halfword pairs represent X/Z
  limits in the centered mode and difference/sum in the diagonal mode.
  The 16 header dependents and the tracking blend unit match. The native
  reset (08226228) clears the position mode directly. Arithmetic division
  and a factored blend change the instruction shape and register choices.
