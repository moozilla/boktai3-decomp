# Monsters unit cleanup

Tags: #monsters #cleanup #ghoul #slime

The concession-free ranges 08085D58–08086C1C and 08089168–0808E208 now live in `src/ghoul_states.c` and `src/ghoul.c`. The intervening range is outside this worker's queue, so the units remain separate contiguous runs.

`include/ghoul.h` describes the full 0x678-byte Ghoul allocation, including the delayed script word and byte counters at 624–627, disc flags at 628, captured targets at 62C/634, cooldown/count at 63C/63E, disc kind at 640, route and recovery delays at 648/64A, bounds at 64C and route position at 670. Interpretations remain provisional. The canonical scene actor reset view extends beyond the Ghoul allocation for other species, so the Ghoul type currently repeats its common prefix while reusing the existing component types. No shared engine header was changed.

Collision callbacks use `ContactCollider`. Recovery and control callbacks use shared actor, drawable, sequence and delayed-response types. The three action controllers at 0808CE50, 0808CF54 and 0808D058 now use the shared state/control helpers instead of padding structs and offset bookkeeping. Disc helpers are shared in `include/ghoul_disc.h`.

Three unnamed routines now have provisional names: `Ghoul_UpdateAimedDiscControl` (0808A664), `Ghoul_SelectRandomRoutePoint` (0808AED8), and `Ghoul_SelectRetreatRoutePoint` (0808AF38). The last rename also updates the two references in 08088C24; that otherwise untouched unit still matches.

Two code-generation details determined the final forms. The terrain-height query in 0808AF38 retains its branch structure: a structured if/else changes six instructions even though the typed terrain accesses match. In 0808CC48, accessing the initial captured target through an inline accessor preserves the original address formation; a direct member expression reuses the body pointer and changes the function by four bytes. Both final forms use ordinary C and typed fields.

All 37 inputs to the second merge were checked after cleanup, along with the first merged unit and 08088C24: 39 MATCH results. The final merged units independently match their complete ranges. The code-byte matching percentage and concession-free percentage are unchanged (0.0 percentage-point gain); this is source organization and type cleanup. The coordinator must run the combined ROM build at integration.

Slime 0808E208–08091E54 now lives in `src/slime.c` (48 functions). `include/slime.h` recovers its complete 0x40C-byte allocation: enrage/launch counters, previous ground height, half-health threshold, saved movement subject, split-script operands, wait timer and flags. It reuses the engine collider, motion-subject, tracking and animation types. Four anonymous callbacks have provisional Slime names. The player-follow transition uses shared state helpers, and palette setup, callback tables, message dispatch and shadow operations use fields.

The saved movement subject starts at 3E0 and holds its position at 3EC; passing this real subobject preserves the original address base. An inline helper returning both palette field pointers preserves the death-debris register allocation. The damage-flags read takes a field pointer, preventing a changed address reuse across the script calls. Structured versions of the hidden-state predicate and route-selection exit changed the output; those small existing goto forms remain, with comments. No shared engine header changed. All 48 inputs matched after type recovery; the final merged unit matches all 15436 bytes. Cleanup changes matching coverage by 0.0 percentage points.
