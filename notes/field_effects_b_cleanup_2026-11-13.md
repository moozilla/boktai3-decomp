# Field effects B cleanup, 2026-11-13

Tags: #actors #particles #animation #matching

The worker consolidated twenty units in ROM order and repaired the unit-merging
writer so it does not stage files through Git. Each source unit matches the stock
compiler. This is source cleanup: exact-match coverage has zero percentage-point
gain. The coordinator must run the combined ROM build at integration.

The final unit, ship-wheel presentation (081C482C–081C542C), combines forty files
and forty functions into `src/shipwheel_presentation.c`. Its shared header recovers
three tasks: the wheel with embedded palette transition, the scripted movement/fade
controller, and the message-triggered falling sprite. The unit matches 3,072 bytes.
`ContactSwarmGraphics.paletteIndex` occupies existing padding at offset 6; the
layout and size remain unchanged. All 688 transitive source consumers match after this header change;
the interrupted check was resumed from its last completed result. A direct write through `wheel->graphics.paletteData` preserves the
original owner-relative store; writing through the cached graphics pointer chooses
a different base register. Five formerly unnamed routines now have descriptive
names. The hidden-state callback uses its function symbol instead of a numeric
Thumb address. Existing packed endpoint coordinates preserve word-copy behavior.

Earlier units recovered shared emitter, burst, follower, directional sprite,
convergence, particle pool, HUD presentation, scene volume, palette effect,
scene effect and queue, link effect, positioned effect, scripted scene object,
tether presentation, sparkle emitter, contact impact, and scene-transition types.
Each was submitted separately through the worker commit runner. Shared-header
changes were checked against their transitive source consumers; external callers
changed by symbol promotion were checked too. The detailed per-unit evidence is
in `build/REPORT.md` and `build/LOG.md` in the worker checkout.

The large scene-animation unit (08190C98–0819792C) was set aside without retaining
source changes. Seven of fourteen tested front functions match the shared Nidhoggr
layout; seven collider paths change base-plus-index address evaluation order. The
existing controller also needs joint recovery of overlapping collider/group-health
views. Next attempt: member-derived collider cursors, then unify those views.

Known existing artifacts retained after measured alternatives failed: the burst
and scene-volume countdown gotos; scene-effect queue projection macro and queue
gotos; the scripted scene object's unused four-byte local needed for its stack
reservation; palette blending's inner do-while(0); and contact-impact growth's
gotos. No new concession was introduced. Contact-impact projection's statement
expression and sparkle allocation's gotos were removed successfully.

The run ended at the user's stop instruction. No frost-emitter source changes
were started. Continue at 081D2D34–081D31D8, then follow the remaining ROM-ordered
queue in `build/cx/units.json`; revisit the large scene-animation unit afterward.
