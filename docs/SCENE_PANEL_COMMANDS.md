# ScenePanel message and display helpers

Reviewed range: **0805C680–0805C8D8**, nine functions, 600 bytes. Two newly recovered functions add **304 exact bytes (+0.0126 percentage points)**; seven existing C functions account for 296 bytes and receive typed contracts/names. Every full reviewed extent matches without register pins or empty asm. C754 is an existing hidden 12-byte leaf already represented separately; it earns no new credit. C8D8 begins the next driver and is excluded.

`scene_panel_commands.h` contains an additional partial object view. The actor ID is at 18 and its message receiver begins at 17B0. Receiver fields observed here and in 08020CD4/08020D54/08020D68 are actor ID0, category2, pending trigger3, two bank counts6/7 and banked message pointers8 (two groups of four). The global message manager's bank selector is at24. Message opcode is byte6 and this consumer's mode argument is halfword8. These are views of shared message machinery, not a claim that every message has the same payload.

| Address | Name | Bytes | Contract |
|---|---|---:|---|
| 0805C680 | ScenePanel_DispatchCommands | 168 | Visit the selected bank. Prepare the receiver's pending trigger from each packet, then handle opcode0 by truncating its mode argument into savedModeC11, or opcode1 by selecting the 080581A0 presentation callback. Both recognized cases request pending-trigger consumption with expected value1. Unknown opcodes receive neither action nor consumption. |
| 0805C728 | ScenePanel_UnregisterReceiver | 20 | Remove the embedded receiver through 08020D08; ignore its result. |
| 0805C73C | ScenePanel_RegisterReceiver | 24 | Register actorId18 with category9; ignore registration status. |
| 0805C754 | ScenePanel_ClearGlobal | 12 | Clear the global panel pointer. |
| 0805C760 | ScenePanel_IsPresent | 20 | Return exactly0 or1 according to the global pointer. |
| 0805C774 | ScenePanel_SelectMode | 64 | If flagC0F is zero, truncate game-state signed halfword876 into modeC10; otherwise use savedModeC11. |
| 0805C7B4 | ScenePanel_CountActiveSubjects | 48 | Count state4 among the subjectCount1E pointers; assumes those counted slots are nonnull. |
| 0805C7E4 | ScenePanel_UpdateApproachSprite | 136 | Show sprite518 only for nonzero modeC10 and activeCount1F, in state10 or14. Set frame1 and uniform random scale72–87; otherwise set its hidden flag. |
| 0805C86C | ScenePanel_SubmitPlayerMidpoint | 108 | Submit packed X/Z midpoints of the player and origin500, with playerY+150, to 08225F88. The callee's broader purpose remains tentative. |

The dispatch loop reloads the global bank selector and receiver count after calls. It does not snapshot the entire batch. The pending helper only copies nonzero packet trigger bytes, and consumption only fires when the receiver byte matches1; calling it does not prove that a trigger fired.

The adjoining driver C8D8 stores the count helper's result into activeCount1F. `skipMainSprite` remains an earlier tentative name for C0F: this consumer proves that it also selects saved versus game-state mode, so its full meaning is broader than sprite visibility. The approach sprite view adds only observed fields: flags0, scaleX8/scaleY9, and animation frame10. Position arithmetic preserves the packed vector's word writes and unused bits; no claim is made about its padding contents.
