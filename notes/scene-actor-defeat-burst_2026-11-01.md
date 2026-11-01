# Defeat burst

Tags: #scene-actors #matching #compiler

`08080494` SceneActorReaction_BeginDefeatBurst is still guarded. Stock agbcc produces the correct
980-byte extent and a 99.0% fuzzy score. One instruction is out of place. Reproduce it with
`python3 tools/scope/scope.py src/fn/sub_08080494.c`.

The six spark calls use the existing particle sample and origin capture idioms. The random table
gets r4, the origin r5, the spread r6 and the vertical range sl. A function-lifetime cursor
rematerializes from its literal at each sample. The group setup and script arguments match too.

The state-entry byte stores keep the zero phase in r5 while the halfword timer is cleared from r3.
The draft sets r5 at `08080750` before the optional bestiary call. The ROM sets it at `080807A8`,
immediately before the timer store. The scope diff pairs every other instruction after accounting
for the resulting PC-relative shifts. Replacing the phase argument with literal zero changes
local allocation: the halfword zero starts out in r4.
