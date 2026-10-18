# Foundry scene-actor phase run, 2026-10-18

Tags: #matching #scene-actor #foundry

Worked in order through 080E41F8, 080E4330, 080E55AC, 080E6848,
080E696C and 080E6A34. Drafts remain under `wip/foundry/`; per-function
results and final verification logs are under `build/foundry/`. No sources
were promoted or committed and no combined build was run.

080E696C matches exactly. The other drafts remain fuzzy: 080E41F8 75.2%,
080E4330 75.9%, 080E55AC 70.1%, 080E6848 66.4%, 080E6A34 86.7%.
The final function has core distance one: the compiler coalesces the ROM's
separate zero values. Earlier drafts retain differences in offset generation,
argument preparation and registers; their final logs identify each region.
No register pins or assembly statements were used.

The shared `SceneActorRegistryResetView` already describes the actor. Added
halfword fields at B8/C0/C8, word D4, and tentative context animation fields at
624/65C/678/69A/69C/6A7 by replacing unknown storage without moving offsets.
Every header edit passed `foundry.py hdrcheck`: all nine existing consumers
still match. Other pre-existing header edits were preserved.

Static evidence: 080E41F8 and 080E4330 initialize context+624 with different
actor animation parameters, clear context+6A7, and save different frame values
on completion. 080DF298 tests context halfwords 698/69A for zero and is named
provisionally SceneActor_IsAnimationFinished in the 080E55AC draft. 080E55AC
restores entryTarget224 and advances to state 3/phase 1 on completion;
080E696C restores entryTarget228 and advances to behavior 15. 080E6848 clears
movement/control flags and restores savedTarget. 080E6A34 sets context+65C bit
0, selects callback 080E766C, and advances to behavior 2. The final update call
in these movement phases reads actor+270, rather than callback248.
