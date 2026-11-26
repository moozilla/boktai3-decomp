# Source-port testbench and fixture findings

Tags: #tooling #scripts #actors #menu

The testbench drives the normal headless source port. It assembles a small
GCL registry packet and hands it to the game's Script_RunBlock (0821AFB4).
Actor allocation evidence comes from the game scheduler, the room registry
and individual slot allocators. A named creator can yield a task, a
registry actor, a slot or an existing singleton. Two native-only fixtures
exercise a scene particle and a scene projectile. See the [testbench
README](../tools/testbench/README.md).

## How the fixtures changed

- The first arena (2744/57 at story 14) loaded into the level-up tutorial
  and suppressed movement. The Sealed Prison corridor at 2776/7 at story
  25 has no ambient registry enemy and takes player input. The Ghoul
  example proves that a Gradius hit lowers enemy HP and that Ghoul contact
  lowers player HP.
- Mode Menu (0BFE) and Game Start (15DE) stalled in their save-loading
  routines when called from the host VBlank import. Link session creation
  (1745) stalled after linking its priority task, during serial setup.
  All three ran when the command moved to the next TaskSystem_RunFrame
  (0821A054) entry, the same timing the daemon uses for queued procedures.
- Standalone link display fixtures require a session with a valid menu
  callback. The initial zeroed session trapped calling address zero in
  SessionMenu_InvokeCallback (0810BB0C). The fixture now supplies the
  existing Video_OnFrameNoOp callback and leaves peer negotiation outside
  the display case.
- Direct Solar Bank (7F5E) injection beside a live dungeon HUD received
  SIGBUS during sub_08000430 (08000430), after the bank and the message task
  had updated. Loading its real scene (2147) creates the bank from the
  normal script and agrees with the original ROM replay.
- The first Actor Group (F1F9) case received SIGBUS after creation in the
  tutorial arena. The standalone allocator and update loop ran in the
  current arena. The earlier fault has no isolated source reproducer.
- The Display Track (8337) fixture needs a free entry. The default arena
  has an existing capacity-three track manager with three entries. Scene
  2803 at entrance 57 lets the tool create the normal manager first and
  observe the new track it adds.
- Tile Animation (7BBC) and Terrain Overlay (E28B) require the resource
  keywords that their library scripts normally fill from arguments. The
  fixtures use values from observed Sealed Prison calls. Numeric Overlay
  (95F8) had no observed registry call and needs a palette resource. Its
  standalone fixture uses the title palette.
- Scene Objects (E02A) needs a parent pool (D9AE). A type-6 recipe from a
  foreign scene reached SceneObjects_Update (08005498) with callback
  0x999B and trapped. The arena case uses a normal type-0 pushable box.
  The original type-6 scene conditions remain a separate reproduction
  target.

The checksummed battery fixture replays in the original ROM at the same
input ticks, with the same RTC and raw sensor. ROM comparison samples the
scene, story, life and player position. Cases with native injections record
why the input-only movie cannot reproduce them.

The [pending Registry 1745 patch](../tools/testbench/pending_src_fixes/registry-1745-return.patch)
addresses a separate missing return in the creator (0810B914). The guarded
candidate matched the original ROM unit. The link fixture itself uses the
observed priority-task link and works with the current source.
