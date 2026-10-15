# Staged keyframe icon display

The tentative StageIconDisplay cohort is08055B74–0805606C: sixteen functions,
1,272 bytes. Seven new exact functions add896 bytes /+0.0371 percentage points;
nine existing helpers account for376 bytes and receive no extra credit. Shared
layout is stage_icon_display.h. Static contracts do not establish an in-game
identity or new runtime coverage.

The singleton0200011C owns a1D4-byte task on list4, a32-byte resource header18,
main keyframe icon38, and three60-byte indicators98/F8/158. Tail fields are script
completion mode1B8, option1B9 (stored but not consumed in this cohort), state
frame1BA, kill request1BC, signed revealed count1C0, native completion callback1C4,
opaque argument1C8, integer script ID1CC and update callback1D0. The native
completion thunk passes both the opaque argument in r0 and display in r1.

Resource initialization looks up CB05/1289, copies/relocates its header and uses
an8-byte packed XYZ parameter whose trailing halfword remains unspecified.
Variant0 builds main frame48/mode48 and three frame49/mode49 indicators at X0/8/16.
Variant1 builds only main frame51/mode48; other variants build main frame48/mode48.
The update selector chooses a90-tick delay for variant1 and reveal state otherwise.
These fallback cases are retained without inventing an unsupported variant guard.

Reveal plays sound706 on its first tick, increments frame and waits until61.
It then shows the next indicator with sound540, increments revealed count and
resets frame to1. Once all three have been revealed it loads main frame50, hides
all indicators, installs shake state and notifies completion. Shake plays707,
jitters icon X/Y with two RNG samples per tick for frames0..7, clears X/Y at8,
and requests deletion when incremented frame exceeds29. Delay plays1260 on its
first tick and notifies/deletes when frame exceeds89. Kill request bypasses the
callback. Destruction always unlinks the main icon and unlinks only registered
indicators before clearing the singleton.

Native creation takes variant, option, completion callback and argument. Script
creation sets script-completion mode and reads m variant(default0), f option
(default0), p completion script ID(default0). Both initialize resource/state and
publish the singleton. The native initializer also resets revealed count;
script creation relies on the task allocation's zeroed memory.

The initializers' epilogues preserve incidental r0 produced by the void state
selector, whose pop-r0 return loads its return address. Generic creators compare
that value with zero. Empty output constraints preserve this observed ABI without
claiming a semantic success/count result or using an incompatible call prototype.
These are temporary matching concessions. RNG uses03005304/086141BC, distinct
from the general03005308/0203B400 random stream.

All16 full reviewed extents passed in scratch after hint minimization. New bytes
are156 unpinned and740 with minimized concessions. In resource initialization,
one ordinary scratch is reused as a stack-vector address and then zero for loop
arguments; the compiler naturally allocates r7. There is no explicit r7 binding.
