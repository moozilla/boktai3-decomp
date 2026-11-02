# Defeat burst at 08080494

Tags: #scene-actors #matching #compiler

`SceneActorReaction_BeginDefeatBurst` matches all 980 bytes with stock agbcc.
The six sparks draw consecutive random counts and use the same range, height,
kind and priority. The function then handles the category sound, optional wide
burst and script, bestiary update and return to idle.

The earlier guarded version set the zero phase before the optional bestiary
call. The ROM first sets r5 to zero at `080807A8`, immediately before the
halfword timer store from r3. A literal zero phase gives the right instruction
order. Register allocation for the earlier sparks determines which register
the compiler gives that zero.

The sample table and the category pointer used by the bestiary check share a
local union. In the allocation trace, that variable takes r4 and displaces both
the random cursor address in the spark block and the phase zero in the idle
block. The cursor then reloads from its literal at each sample and the phase
zero gets r5. Passing the three signed halfword spark settings through the
inline emitter keeps priority 1 in r8, kind 2 in r9 and height 20 in sl.
