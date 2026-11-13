# Bee cleanup draft

Tags: #monsters #cleanup #bee #volatile

The source-only patch `bee-cleanup-draft_2026-11-13.patch` preserves the unlanded cleanup of 08091E54–08095574. Apply it from the repository root only when resuming the work. The normal sources remain their original matching versions.

The recovered 0x490-byte Bee context contains flags at 3D4, initial height/previous ground at 3D8/3DA, behavior cooldown and saved facing at 3DC/3DF, nine points at 3E0, waypoint and cooldown bytes at 428–42B, duration at 42C, curve time/phase/type at 430–437, remaining duration at 43A, anchor ID/speed/pointer at 43C/43E/440, a saved motion subject at 444 with curve output at 450, endpoints at 470/478, motion at 480 and original position at 488. Existing engine component types preserve the layout. These field interpretations remain provisional.

Forty-four functions matched after type recovery and prototype fixes. The remaining route follower at 08093810 had a `vu32 zero` local, which the concession scanner did not recognize. Reinstating that existing volatile in the recovered draft produces an exact 1288-byte match. A plain word produces 1272 bytes and changes allocation widely. Block scopes, an inline clear helper with its own local, a one-word array, and three scratch-record layouts did not recover the match; the closest-sized record produced 1292 bytes with one extra spill. No volatile or new concession was landed.

The preserved patch also includes a direct waypoint indexing experiment in 0809345C that differs in 19 instructions (six apart from register choice). Its prior typed form, using the field-derived address in two steps, matches. The analogous direct-index cleanup in 080941F4 matches. Remaining work: remove the volatile from 08093810, recover ordinary indexing in 0809345C, replace address-bearing temporary helper names with descriptive names, name four anonymous functions, recheck every input and the merged unit, and only then land the unit.
