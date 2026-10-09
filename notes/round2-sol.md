# Sol production round 2

Baseline: `e2546f66536ae28998b5836ebb936a99ce47c9a0`.
Branch: `codex/endurance-r2-sol`. Exclusive assigned range and eight root reservations respected.

40 new exact functions, 3,736 emitted bytes. 21 independently adapted seeds (three permuter assisted), seven manually translated family variants, 12 automatic template ports. All additions are individual matching commits; no baseline rewrites.

89 logged candidate checks: 40 matches, 44 mismatches, five tool errors. The errors were checks overlapping the worker's ELF link; they were retried after the full build completed. Five separate 60-second permuter trials: 08210C5C, 0817040C and 081E0D58 solved; 08170EE8 and 08190E8C did not improve. Individual check transcripts and archived candidate sources, actual Unix timestamps, automatic port records and exact ordered commit manifest remain in private `build/round2/`.

Matching elapsed: 23.57 minutes (includes full-build waiting). Full builds 1 through 9 each printed `build/boktai3.gba: OK`. Final three-script shift check passed all screenshots in newgame_intro, save_boot and save_menus_field. It uses SHIFT=0x10000 and the prepared read-only root harness executable. Make unexpectedly regenerated shared gen/code_sym.s and gen/data.s because shared gen/code.s had a newer 19:30:49 timestamp; the parent was notified. Future worker tests should invoke the shift scripts directly to avoid Make prerequisite regeneration.

Patterns: allocator null guards must fall through to retain r0 where original assembly does; decoded allocation constants need factor/shift/kind indexing after the first argument copy. 08210C5C's r2/r3 swap vanished when its conditional and counter update were enclosed in a do-while-zero scope. 0817040C matched after removing the cached key local and testing global reads directly. 081E0D58 matched with an explicit s16 flag temporary before its shared callback branch.

Retained candidates: `build/round2/large-best.c` is the 081E8374 animation family seed (552 versus original 580 bytes; seven family members), not the root statistics-counter family. `wip/round2/sub_08190E8C.c` has only two pointer-add operand differences in 96 bytes; its family includes 081913EC. `wip/round2/sub_08170EE8.c` retains the 108-byte register-allocation near-match. `wip/round2/sub_081CBCBC.c` retains the uncollapsed-switch-table problem. Other failed candidates stay under `wip/round2/`; no nonmatching C is tracked.

Ordered matching commits:

- `bdbb0fda3efccf619989af3aef8c4e7b894c4e8f` match sub_081D0BD0
- `ecede998bd136ef4d45a690c13c30b9ddee4305f` match sub_08170040
- `c711e0dffe94e42536b99ed0ff85b2b08a1a324c` match sub_08171274
- `164e74cf6dd4d243607e2fdf54e5b85d19677430` match sub_0817147C
- `e9ecd57335f5abb45e0e65d924a17479e6f05f9f` match sub_081708CC
- `f94c4a3b906a594a7a84397d79e30d850e594b74` match sub_081700A8
- `5d92ef09e95c5b871c4e9fbacbedd3a20392587a` match sub_081739A8
- `b7a88ce0a1dcab44890a04ee09f4a603e34eda11` match sub_08173928
- `6c04ff8b21f61a1a829379deeabfbbfbfa202efb` match sub_08173A14
- `c1b4ca580d60e79b9f1a29a689515182cbfb43af` match sub_08173390
- `714c61a8fb3e629edf8177b825d0c4b1dd5581d6` match sub_08202608
- `2690db20b145b7ef43f1178cb8c77d4ce6b4d668` match sub_0820361C
- `bfb5246bcfa88c4e67ba26b8bcda3c053aee3ba7` match sub_08204704
- `83c2cc0e687d58b7b725b770fadf0a282aee49f3` match sub_08205900
- `e47a22dce792365380e8ea3adae45c1c9d9ca2a7` match sub_08206470
- `97cc890ce2914df5fdb1f1218102ba4196b5d4f8` match sub_08207F68
- `1966b9aa4467d91c749fe1be88f6ba798d78df14` match sub_08208FD8
- `9f0440cab17103200de4110ab10e6975cadb2654` match sub_0820BCE0
- `9efe501e777cb6c805d39029746c1d2351600049` match sub_0820D1F8
- `8fb612d3cf047f9c2e41c2624d815a395f7ee7ee` match sub_0820EBA8
- `6d47c9e2d1d8f1f8e98cdd557aa5835e308fead5` match sub_081DE078
- `ec9bc14169b9c67e375fccb3d49ea5540df240e5` match sub_0820B258
- `9589a96a72cd9fbe2ccd09b8ed492ef7fce71323` match sub_08228F9C
- `9e4a8d28b51092cb91f2251e1e9528b72f55e59e` match sub_081CAD28
- `92c5b80e6ae97974039b3c66556a771dd70c0a2d` match sub_081CADD8
- `3603381406d658066237c0080b01be2ad2b672cf` match sub_081FBB38
- `7ae841c33c7c78dd4c0590d2921009d6c7627504` match sub_081FC1FC
- `877fdabe48b1a6b9e22c5866fc7f7f6205bc1aa8` match sub_081FCFA8
- `2fe2295e4889697984e8f5954690b90af28c92e8` match sub_081FFE20
- `172e1ba32e71951ec716a4eb6cdb5f03f85f1448` match sub_081E00E8
- `b95716856c0fe62f4701c458c5614931153f5330` Match sub_0820FD10
- `9d9d8fbb3f04aa8179837d8e35f0180e5e90d985` Match sub_082109D8
- `2abaac3f77cf6b61b311a5c36bec487341eab79d` Match sub_08228FF0
- `60a598afa2bf35fc29903a80392a4e8cb8f52c74` Match sub_0820F298
- `85ee38187811cc78d0cc34f268e32bc9235a7fa2` Match sub_08210C5C
- `b9f0007e9b4958c91974d62f43c0aa79d6cb1822` Match sub_0821211C
- `9f4092d71ac0de2050089d1fc4fb743d354e84ac` Match sub_08213294
- `c72aff1896c05d99df97162ce8c1edbbc68d18b6` Match sub_0817040C
- `31ca727fe6e1dee9c4ba8743cc15f8db2d75d5ab` Match sub_081E0D58
- `9dd399606c9d9f07aab2659a91dd7345f1d41e85` Match sub_081EB864
