# Tentacle wander-route selection

Tags: #matching #monsters #tentacle #080A8904

`TentacleActor_SelectWanderRoute` matches all 984 bytes in ordinary C.
Typed wander state replaces the raw field accesses. The cardinal mask and
route cursor share one scan owner, and named operations retain the route-cell
reads and candidate advancement. A signed-halfword wrapper around the shared
terrain-width getter preserves the map-pointer lifetime needed by the match.

The stock per-function check reports zero differences. The combined ROM build
prints `build/boktai3.gba: OK`, and the RAM-literal check passes. No register
pins, asm barriers, compiler overrides or chase selectors were added.
