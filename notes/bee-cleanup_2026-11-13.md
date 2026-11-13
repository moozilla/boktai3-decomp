# Bee cleanup draft

Tags: #monsters #cleanup #bee #volatile

The completed 08091E54–08095574 unit now lives in `src/bee.c`, with its shared types in `include/bee.h`. All 45 input functions and eight reference-only rename consumers match; the merged unit matches all 14112 bytes. Four anonymous routines now have provisional Bee names. The old source-only draft patch has been removed because the completed source supersedes it.

The recovered 0x490-byte Bee context contains flags at 3D4, initial height/previous ground at 3D8/3DA, behavior cooldown and saved facing at 3DC/3DF, nine points at 3E0, waypoint and cooldown bytes at 428–42B, duration at 42C, curve time/phase/type at 430–437, remaining duration at 43A, anchor ID/speed/pointer at 43C/43E/440, a saved motion subject at 444 with curve output at 450, endpoints at 470/478, motion at 480 and original position at 488. Existing engine component types preserve the layout. These field interpretations remain provisional.

The final blocker was the private clear operation in Bee_FollowWanderRoute. Its local volatile word duplicates the existing `ZeroFill8` helper in `global.h`. Calling that shared helper matches all 1288 bytes and removes the duplicated private implementation. The shared helper's existing volatile implementation was not changed; this cleanup adds no volatile object or other concession. The separate scanner change now recognizes the vu*/vs* aliases, including the existing helper itself.

The curve builder must form the indexed base before taking the points member address. Direct indexing changes 19 instructions (six apart from register choice). A small documented adaptation uses the array element's `sizeof` for the stride and then the real `points` field, with no hard-coded struct offset. The anchor follower accepts direct indexing. The return-to-player transition uses the shared control helper, unused private helpers were removed, and other identical helpers are deduplicated by the merge.

The code-byte matching percentage is unchanged (0.0 percentage-point gain); this is source organization and type cleanup. No full ROM build was run. The coordinator validates the combined build and regenerates concession/progress totals after integration.
