# Scene effect schedule (tentative)

The singleton at `0200019C` owns a `0x19C`-byte task containing twelve 32-byte
entries and a mask at `198`. Each entry contains position and offset vectors,
two signed parameters, ID, flags, kind, style, a signed slot index and an effect
pointer. Allocation scans the mask and records the selected index. Creation
uses task list 10, installs tick/destruction callbacks, and forwards an argument
to initialization; initialization currently ignores that argument, initializes
the effect subsystem and publishes the singleton. Destruction/reset clear it.

Ticking requires a nonempty mask, clear `400` in both global control words,
and clear bit 4 in `02000560`. It then scans all twelve entries; there is no
per-entry occupied-bit test in this loop. A nonnull effect with negative signed
state at `13E` is recreated. Unless control flag `800` is set, both signed
parameters are scaled by `Div((10 - gameState.unk876) * value, 10)`. The effect
constructor receives position, ID, scaled parameter, kind, style, -1, offset
X/Y/Z, the other scaled parameter and flags. Keep these provisional parameter
names until callers or runtime evidence establish their units.

The script removal helper reads keyword `n` with default -1. A nonnegative ID
is looked up among occupied entries; on a hit, it clears the entry's mask bit
and all 32 bytes of the entry. It does not call an effect destructor here.

The script creation function at `0806F5C0` remains assembly-backed. Its private
412-byte semantic draft reads position (`p`), offset (`a`), style and random
range (`f`), ID (`n`), kind (`d`), scaling (`t`) and a normalized flag (`s`).
Missing fields retain their previous entry values. The current candidate differs
by two bytes and has a frozen, bounded permuter search; it contributes no exact
credit. Mutations that discard initialization are not acceptable matches.

New exact code totals 412 bytes: 260-byte ticking retains two initialized
empty-asm lifetime barriers; 80-byte creation and 72-byte removal are ordinary C.
The existing allocation, ID lookup, initialization and singleton cleanup helpers
have typed shared views without additional matching credit.
