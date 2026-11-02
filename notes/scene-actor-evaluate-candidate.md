# SceneActorContacts_EvaluateCandidate

Matched all 2,800 bytes at 0x08072F4C in pin-free C. The player-path call passes
the contact node's object directly. PRE then bases the sample-position
expression on the second call's load, putting the three address spills in
ROM order at sp+0x90, sp+0x94 and sp+0x98.

Assigning the local difference vector component by component reproduces the
first path's zero in r4. The typed actor copy is established on entry to the
inlined evaluation. Keeping the map width in a local for each vertical neighbor
test gives the second actor copy r7 across the facing call.
