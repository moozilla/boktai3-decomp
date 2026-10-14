# Encoded records and password callers, 08044C80–080454C0

This contiguous subsection contains two 24-byte record encoders and decoders,
six-bit packing helpers, seeded masking and four-row display formatting.
Static callers confirm the connection to the message-menu and password systems;
there are no new emulator execution claims. The alternate decoder is used by
NameEntry_DecodeAndValidate in src/password.c after version/checksum validation.
The first encoder and row formatter feed MessageMenu_Initialize.

This batch adds six exact functions and **0.088 percentage points** (2,136 bytes):
paired contact-swarm spawn 416; record XOR 100; first encoder 620/decoder 228;
alternate encoder 544/decoder 228. Four record encoders/decoders are entirely
unpinned. The existing packing helpers, LCG and formatter gain names/types with
no new credit. The count-limited paired spawn remains a guarded 98.6% draft;
its only three differing bytes select r0 rather than r2 for the speed-byte load.

## Record framing

Each record holds 144 bits in 24 bytes whose low six bits are used. Packing
writes least-significant bits first, using a scalar bit-index cursor. A write
ORs bits into the buffer rather than clearing a zero bit. Writers saturate an
unsigned oversized value to the requested bit count; count zero returns early.
Readers advance the cursor and reconstruct the value with the same bit order.
The old pointer-to-pointer cursor prototypes represented integer bit positions;
the exact packing implementations now use u32 pointers.

Bits 0–15 hold the checksum, bits 16–17 are reserved zero, bits 18–20 contain
value 1, and bits 21–23 select a random mask skip from 0–7. Payload starts at bit
24. Both field selections use 80 bits, followed by five eight-bit name bytes,
ending at bit 144. Short names are zero padded. The decoder writes eleven u32
fields followed by the five name bytes, which aligns with PasswordEntry's
44-byte decoded area and adjacent name storage.

Both encoders sample the random table with ((counter+1)&1023), take bits 8–10
for the skip, and pass the complete record to the same mask helper. Name scanning
checks both bytes of each source character and stores its second byte. This
matches the game's paired glyph strings; the early zero checks remain explicit.

| Field source | First format width | Alternate width |
|---|---:|---:|
| gUnk_030053F8 +18 (meaning tentative) | 6 | 6 |
| GameState +660, +662 | absent | 3, 4 |
| +538, signed halfword shifted right 3 | 8 | 8 |
| +664 | 7 | 7 |
| +75A | 9 | absent |
| +7B4 | 7 | 6 |
| +75F, +75E | 1, 6 | absent |
| Reserved zero | absent | 6 |
| +520, signed word shifted right 12 | 10 | 10 |
| +51C, signed word shifted right 8 | 8 | 8 |
| +614 divided by 216000 | 6 | 6 |
| +536, signed halfword | 12 | 16 |

The first encoder explicitly clamps most scaled fields using unsigned comparisons,
so negative results saturate to the field maximum. The alternate encoder delegates
the bound to WriteBits. Decoders restore the shifts on +538, +520 and +51C.
Other gameplay meanings remain unnamed; include/game_state.h now records the
observed scalar types while preserving the established offsets.

## Masking, checksums and display

EncodedRecord_NextMaskWord advances a u32 seed with multiplier 0x6262C05D and
increment 1. XorRange starts its accumulator at 0xFFFF, XORs in skip+4 generated
words, then masks bytes [start,end) with that accumulator and the supplied mask.
After each byte it XORs in another generated word. Both record formats apply it
only to bytes 4–23 with mask 63. Seeds are 0x8C159 and 0xB8E6E respectively.
The first checksum wrapper uses polynomial 0x8005, the alternate 0x1021; each
calls the MSB-first checksum core with initial 0xFFFF and final complement.
Checksums cover bytes 3–23 after masking, then are written into bits 0–15.

The display formatter converts codes through the caller's glyph-pair table into
four rows at 256-byte strides. Codes greater than 44 are wrapped in existing
<WEIGHT> and </WEIGHT> tags from 08605934/08605940. The password caller strips
tags before converting glyph pairs back to codes.

## Matching evidence and remaining work

Independent entries 080450D8 and 080451BC were hidden in the first encoder's
retained tail. Their own prologues, returns and literal pools establish reviewed
228/544-byte extents; build/progress metadata splits them without editing gen/.
Hidden entry aliases remain file-local where gen/ has no recognized function
label for promotion. The gUnk_08045276 split inside the alternate encoder is retained
as a relative label, not a separate function boundary.

Natural shared types and exact control flow matched both encoders/decoders.
A temporary arithmetic-shift barrier in the first encoder was removed by depin;
an ordinary signed randomWord local alone preserves the original instruction.
XorRange reduced ten hints to four. Paired spawn reduced 57 to 24 hints; removing
redundant barriers restored a short conditional branch and corrected reload
spill rotation. No handwritten instructions or r7 bindings were introduced.
The nearby count-limited spawn retains three register-byte differences after
bounded barrier, parameter and load-width experiments; its 24 retained hints
compile to the same checked candidate bytes as the reviewed close baseline.

Next contiguous subsection begins at 080454C0: a collider callback, sixteen-slot
motion pool, manager/factory, spawn wrapper and parameter setters. Its tentative
0x13DC allocation and 0x13C slot stride follow the factory and initialization loop.
