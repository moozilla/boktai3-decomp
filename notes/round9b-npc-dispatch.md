# Scene NPC command dispatch

Tags: #matching #scene-actor #commands

SceneNpc_DispatchCommands matches all 1,552 bytes. Command 8 merges the low two message bits into the actor flags and reuses its direction mask for four-way facing. Masking both halfwords in the upper half of a word gives agbcc the same two-address Thumb AND as the ROM. The compiler removes the shifts.

Commands 17 and 18 read a signed halfword speed. Command 15 clears the halfword at actor offset 0x2E before the palette transfer.
