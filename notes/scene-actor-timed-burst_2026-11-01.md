# Timed reaction burst

Tags: #scene-actor #matching #compiler

`08081C98` SceneActorReaction_UpdateTimedBurst matches all 1,260 bytes without
register pins. The earlier draft already reproduced the 0x88-byte frame and
its instruction sequence. Eighteen spilled field addresses occupied the wrong
stack slots. Materializing each of the three flag predicates as a byte result
and reading the actor's byte direction through a typed getter gives PRE 534
input instructions instead of 510. Its 267-bucket expression table then
assigns the addresses in the ROM's order.
