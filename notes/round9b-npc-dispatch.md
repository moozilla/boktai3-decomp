# Scene NPC command dispatch

Tags: #matching #scene-actor #commands

SceneNpc_DispatchCommands at 081D01C8 remains guarded at its 1,552-byte ROM extent. Commands 17 and 18 read a signed halfword speed. Command 15 clears the halfword at actor offset 0x2E before the palette transfer.

The stock comparison reports 98.3% fuzzy. In command 8, one direction mask is loaded before the message flags. The ROM also copies its mask before combining it with the shifted angle. This changes an alignment halfword and several branch distances.

A differential execution compared all 69 command values under both dispatch phases, both attachment states and two signed message speeds. The 552 paired executions produced identical actor memory and external call arguments.
