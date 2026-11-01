# Scene actor contact candidates

Tags: #matching #scene-actors 08072F4C

The guarded draft of the 2,800-byte contact search reaches 98.6%. Both inlined
copies use the same four-way adjacency check as SceneActor_FindDirectionalCell.
The player path reloads the contact node's object after testing the candidate.
A wrapped negative heading adds 256, and the range test preserves the original
signed half-distance rounding.

Remaining differences concern a zero held across the facing call, the lifetime
of the actor pointer in the second inline copy and three permuted stack slots.
