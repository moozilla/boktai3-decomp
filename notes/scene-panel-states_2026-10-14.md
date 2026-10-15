# Scene panel state and background controls (tentative)

The08058130–080586C0 subsection is26 exact functions /1,424 bytes. One new
200-byte background initializer adds0.0083 percentage points without hints.
The25 already-C helpers (1,224 bytes), including three previously recorded hidden
entry thunks, receive typed contracts without new progress credit.
All26 reviewed extents match without register concessions. This subsection is
part of a larger presentation object; it does not establish its in-game name.

The shared partial view has frame22E, eight halfword channels230, state callback
250, background-map pointer4D4, palette pointer4D8, palette modeC10, presentation
mode1328, resource selector133A, transition-complete word1428 and channel-dirty
halfword1430. Channel writes set the dirty flag. Changing state clears frame.
Each timed state increments a halfword and compares its wrapped value, retaining
the original overflow behavior.

Observed channel2 sequences are: forward3→4→5, reverse4→3→2 (eight ticks per
step), and6→7→8→9→10→11→2 (six ticks per step). The long sequence clears the
completion word on entry and sets it on its final step. Immediate entry thunks
select2 or5 and clear the callback. Pulse entry selects12, then eight ticks to13,
38 to12, eight to2,38 to12, and repeats. State names describe those channel values;
they do not claim a particular animation or game event.

Palette mode mapping is0→8,1/2→6,3→4,4→2,7/8→10,9/10→12,otherwise0. The loader
selects resource92B3/313A+14+offset*32 and copies64 bytes to palette shadow03004FC0.
The new initializer selects a C091 resource through table08605A88[resourceIndex],
stores its resource pointer at4D4, draws target3 with a single zero
index and configures layer1. It clears display-shadow bit200, calls08215A74(1,0),
and writes BLDCNT=3C42, BLDALPHA=0810, BLDY=0. It then takes32 bytes from
92B3/F308+194 into shadow03005140 and stores that pointer at4D8. Missing-resource
behavior and unspecified arguments are preserved. SetPresentationMode writes30
at1328. Broader ownership and later consumers remain under investigation.

No execution coverage is claimed. Hexadecimal addresses/offsets/flags above
are written without0x for compactness; byte counts and timing are decimal.
