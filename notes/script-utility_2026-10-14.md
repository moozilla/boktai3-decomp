# Script utility commands and reference ABI

08056A60–08056D70 is a contiguous cluster of script utilities. Four new exact
functions contribute476 bytes /+0.0197 percentage points, all without matching
hints after minimization. A separately recovered20-byte leaf at56D24 is a new
C definition but gives zero net exact-byte progress: its bytes were previously
credited inside already-C query56D04's52-byte progress extent. The corrected
extents are56D04–56D24 (32) and56D24–56D38 (20), with one additional function
boundary in both build and progress inventories. The leaf is registry key15B3;
its independent BX-LR and literal prove it is not query padding.

A script reference has four descriptor bytes followed by halfword count/index.
The parser0821AAD8 advances the script PC while decoding this reference. Writer
0821B4C8 selects one of three bases by descriptor flags, adds its offset, and adds
the stored index for indexed descriptors before dispatching the typed write.
The first utility writes successive script values until PC reaches zero.

Repeated-call command56A90 reads a argument count/r repeats (both default0),
allocates a VLA of signed32-bit values and builds the established8-byte argument
descriptor: halfword count plus unspecified upper halfword, then values pointer.
Keyword e supplies the nested interpreter's script PC. If d is present, it caches
the data PC, decodes count values for each repeat, advances that cached PC between
values and calls0821AFB4 with the descriptor. Without d, it repeats the call with
the original VLA, preserving the original uninitialized values when count>0.
There are no new validation guards. The compiler creates its own r7 frame pointer;
no register binding is required. Reusing one ordinary counter across outer-loop
and inner countdown lifetimes reproduces its spill to the frame before decoding.

Random query56B64 consumes one sample from the general masked1024-entry stream.
A positive p bound returns Mod(sample>>3,bound); missing/nonpositive p returns the
full unsigned halfword sample. Keys56BC8 returns newly pressed for p/default,
held for s, and the third input halfword for r. The third field's detailed meaning
is not established here. Indexed read56C14 requires r base/i index/s size; absent
keywords return-1. Size0 reads an unsigned byte, size1 reads a signed halfword at
stride2, and other sizes still read a signed halfword but at stride4. Preserve
that unusual final behavior; it is not a32-bit load.

Control setters use030053F4 bit200. The setter optionally stops the song stored
at03005470+3C and latches0300523C bit8. Clearing restarts that stored song when
bit8 was set and clears the latch. The flag's wider meaning remains tentative.
A query returns its Boolean value. The hidden leaf sets030053F4 bit800 and leaves
incoming r0 unchanged, preserved as an explicit input/return contract; a final
utility writes bit800's Boolean state to a decoded script reference. No runtime
coverage or particular script usage is claimed.
