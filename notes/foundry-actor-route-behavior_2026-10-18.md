# Actor route and behavior foundry pass

Tags: #matching #foundry #scene-actor #route

All six requested functions matched with plain C, without register pins or
assembly. Sources remain in `wip/foundry/`; per-function reports are in
`build/foundry/`. No production integration, commits, or full build.

- 08130BD4: `SceneActorRoute_UpdateTimedPhase`, three checks. Consumes the
  actor's entry flag, installs callback 5F4, initializes signed countdown 5C6
  from the route step duration, ticks it, and conditionally advances the route.
- 08130E60: `SceneActorRoute_UpdateWaitPhase`, two checks. Restores callback
  22C, then either changes behavior or compares the postincremented route
  elapsed byte with step duration before advancing. The duration callee
  0821E7E4 reads `cursor->entry[4]`; named provisionally
  `SceneActorRoute_GetStepDuration`. Its declaration needs a full signed
  return for this comparison; the context's u16 return adds truncation.
- 08131BC4: `SceneActorBehavior_DispatchOrEnterState2`, one check. Global
  flags redirect the actor to state 2, phase 0, behavior 1; otherwise calls
  behavior-indexed callback table 28C. The flags' broader meanings are unknown.
- 08131C48: `SceneActorBehavior_DispatchAlternateOrEnterState2`, one check.
  Same dispatch policy with table 288.
- 08131CCC: `SceneActorBehavior_InstallCallbackTables`, one check. Installs
  ten ROM tables at 27C–2A0. Roles beyond observed behavior/phase lookups
  remain represented by offsets.
- 08132898: `SceneActorBehavior_DispatchFlaggedCategory`, one check. Requires
  enabled == 1, flags2CC bit 1 clear, and field2D8 bit 20 set. Category 9
  calls 08132748 and returns zero; categories 11–13 call 08132490 and return
  one; other categories call 0813214C and return one. Callees not renamed
  without sufficient semantic evidence.

Shared `scene_actor_reset.h` additions replace padding only: callback 26C,
callback tables 27C–294 and 29C–2A0, callback 5F4. Existing offsets stay fixed.
`foundry.py hdrcheck` verified all nine directly including matched sources
following every layout edit. Inline predicates preserve mask-before-load
instruction order. Provisional names have low confidence.
