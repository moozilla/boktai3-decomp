# Inventory lifecycle and neighboring controllers

The contiguous reviewed range **080632AC–08064790** contains 53 functions / 5,348 bytes. All 53 now have exact C. Fourteen newly exact functions add **0.1280 percentage points** (3,092 bytes): 1,288 ordinary-C bytes and 1,804 bytes retaining minimized register concessions. Existing matched helpers receive contracts/names without new credit. The old 97.3% guarded645D4 draft is replaced by exact C. No runtime scene identity or new coverage is claimed. Parent integration owns name promotion and the combined ROM build.

## Inventory controller tail: 632AC–63634

`scene_inventory_lifecycle.h` is a narrow phase view. Text pointers occupy1A88/1A8C/1A90/1A94/1A98, interactionFlags1AA4, mode1AA6, presentation1AA7, category1AA8, clockMode1AAB, page1AB0, pageCount1AB1, itemPageCount1AB2, frame1AEE and action1CF8. The catalog, transition and other controller views overlap these later workspaces; this is not a claim that they are independent objects.

63334 clears the five pointers, selects mode4, resets flags/page fields and starts pageCount at1. Presentation0 calls62DEC,2 calls630FC,1/default calls62F10; these three copy the builder's pageCount to itemPageCount. Presentations3/4 call6300C without that copy. It then configures presentation, initializes the panel, calls the seven-argument helper with(2,5,4,4,4,FFFF,0), plays song118 and requests action631C4. It always returns0, while the factories preserve their observed failure check.

The five factories6347C/634D4/6352C/63584/635DC allocate a1D24-byte task in list11, register632AC/632EC, and select presentation0/1/2/3/4 respectively. Categories are0/1/2/1/1. The numbered names preserve observed configuration values without guessing the corresponding screen labels.

632AC draws numeric HUD status and the clock using clockMode, invokes the current action through the interworking helper, then increments frame and returns0. 632EC removes the sprite at60 and **69 consecutive60-byte widgets** beginningA8 (54 display widgets plus15 comparison widgets), destroys the transition pair at1AF0 and glyph pair at1C14, calls the two external cleanup routines and returns0. Its explicit69-element cleanup view prevents indexing across separately named widget members.

## Scene audio events: 63634–638BC

`scene_event_dispatch.h` recovers the double-buffered receiver: two count bytes at6, followed by two arrays of four pending pointers at8. A command has its dispatch kind at6 and a signed16-bit argument at8. The task embeds this receiver at1C. The current phase is byte24 of the object behind0200047C. Valid protocol values are phases0/1, counts0..4 and command kinds0..9.

6374C reloads the current phase and count around each dispatch, obtains that phase's indexed command, calls08020D54, then routes by kind. All ten handlers consume one pending event after calling their audio helper. Their signed full-word argument loads are preserved even where an underlying SDK wrapper ultimately consumes a16-bit value. Values and bounds originate in the existing event producer; this pass does not add validation.

| Kind | Handler | Observed audio operation |
|---|---|---|
|0|63634|Sound_PlaySong|
|1|63650|0822B330, direct m4aSongNumStart|
|2|6366C|0822AE7C, start/continue tracked song and reset volume|
|3|63688|0822AF78, fade in tracked channel10|
|4|636A4|0822AFE8, temporary fade out channel10|
|5|636C0|0822B058, fade out channel10|
|6|636DC|0822B0C8, start/continue only channel12 songs|
|7|636F8|0822B114, fade in channel12|
|8|63714|0822B180, temporary fade out tracked channel12|
|9|63730|0822B1D8, fade out tracked channel12|

The operation names are supported by the existing C consumers in0822AE7C–0822B330. No channel-to-music/ambience identity is asserted. 6386C registers receiver kind8, and6387C allocates a 50-byte list11 task. Its6385C driver discards the dispatcher's incidental return value and returns0. 63868 is a no-op destructor. The dispatcher deliberately falls off its non-void body to preserve the original return-register behavior; no caller in this cohort consumes that value.

The dispatcher reached 99.2% with one index-shift ordering difference. A 120-second permuter search and a subsequent longer background run did not improve the candidate. Naming an explicit local state load fixed the ordering; depin removed all four temporary hints. The first search completed; the longer search was stopped after the exact full272-byte check passed.

## Paired roaming objects: 638BC–645D4

`roaming_object.h` gives two neighboring144-byte task families the same observed layout: angle25, position28, velocity30, sprite64, graphics90, homeAC, colliderBC, sequence110, sequence resource120, listener124, target angle134, turn timer136, animation138, impulse13A, state13C, flash count13E and state callback140. "RoamingObject" is a provisional description of behavior, not a recovered game object name.

State tables at0824DF8C/0824DF98 each contain three callbacks: wander63970/63FFC, flash63A20/640AC and impulse63A68/640F4. The state setters copy the three-entry table, set state, clear flash count and install the callback. Both drivers call the callback, update animation, and update the actor registration at1C.

Wander approaches target angle in steps of4 using the wrapped difference. When its timer is zero it consumes two entries of the shared1024-entry random table, choosing one of four target angles and the next interval of 120..183. It refreshes the collider at the current position and selects animation7. The intermediate random-counter store is absent in the observed code; the source stores the final counter after obtaining both samples.

Impulse selects animation2, projects its magnitude through the sine table at086149C4 (quarter-turn offset for X, direct angle for Z), truncates signed products toward zero at12 fractional bits, and adds them to velocity30/34. Magnitude decays to three quarters. Once zero, angle reverses by128, target angle follows, listener flag1 clears and state returns to wander. Flash selects palette192 for the first six frames then193, selects animation2 and increments its counter.

Animation folds the heading quadrant into three sequence rows plus a mirror argument, calls Anim_SetSequence, and executes one inline playback step. Shared playback updates sprite frame/mirror flags, elapsed/index, packed frame weight and duration. It retains observed repeated packed-command reads. The helper was minimized together with its consumer; the two 316-byte TUs retain two local hints plus 13 shared helper hints. No explicit r7 binding is used.

Position initialization63CAC/64338 parses keyword70 into three packed coordinates (or zero), maps signed X/Z cell coordinates to a terrain index, checks a dynamic override, and derives terrain height from the first cell byte. Low nibble supplies the base height; high-nibble types1/2 subtract fractional Z/X respectively. It registers actor kind7 at1C. Packed-coordinate stores leave padding untouched; only initialized coordinate halfwords are read through the local16-bit view. Both 256-byte initializers match without register hints.

## Attachment removal boundary: 645D4–64790

`actor_attachment_slot.h` is a separate narrow slot prefix coordinated with the next worker. It covers flags31/32, status[3]36, secondaryStatus[3]39, signed slotId3D, kind3F, signed timer40, thresholds[3]42, active48, signed counters[3]50, source5C and sprite60. The next worker's signed counter comparisons establish their signed type. This is a distinct family from the144-byte roaming objects.

645D4 acts once per flag32 bit0, after confirming a source exists and its16-bit region key2FA equals GameState.region5A0. Source category2B5==15 suppresses it. Source state98==4 playsEE; state1 playsFF unless slot kind3; state3 calls08077848(source,3). Successful actions set the notification flag. The region-key interpretation is provisional, supported by the existing GameState field and exact equality.

6466C waits for timer expiration. If removal status36 bit0 is set, it clears all three status/secondary/counter/threshold entries, words24/4C and flags31; marks sprite hidden; clears the slot's bit in owner+A1C; removes the sprite; clears active and writes slotId=-1. It returns1 for removal. The active-slot protocol supplies a valid nonnegative bit index before this operation. While timer>10 and removal is pending, it jitters sprite X by a random value in[-10,9], then notifies the source and decrements timer. Other paths return0.

Concessions were minimized: steer 2 hints, impulse 3, animation 2 local + 13 shared, source notification 2 and removal 18. The remaining low-register and offsetof-style address temporaries retain the compiler's observed address lifetimes. They contain no instruction bodies. Full source validity and exact extents were reviewed; final checker records are in build/lifecycle-final.json in the isolated worker checkout.

Final validation: all **39 / 39 changed TUs** matched their full reviewed extents (4,212 bytes) after formatting. The other fourteen existing TUs in this range are untouched. No worker full build was run; the parent validates the combined batch.
