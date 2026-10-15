# Actor timer and pending-flag update helpers

The contiguous pair0807A62C–0807A8CC contains two complete callbacks,232 and440
bytes. Both newly authored bodies pass full-extent exact checks. Their combined
672 bytes add **+0.0278 percentage points**, all in Register-pinned. Hint removal
reduced the first callback22→12 and the dispatcher47→16; neither binds r7 or
contains instruction bodies. Names describe static behavior, without coverage
claims. The adjacent A8CC–C11C animation/motion and behavior update bodies remain
separate work.

A62C clears word304 before its availability test. Actual callers must supply a
valid actor even though the inlined predicate retains a null case. Flags2D4&40,
flags2D0&20000, or health<=0 suppress subsequent work; the callback returns1 on
every path. Otherwise it invokes78700 to refresh collider activation. Flags2D4
bit2000 clear byte5CA. Without that bit, nonzero halfword2B8 is decremented and
its pre-decrement bit2 requests flags2D6 bit20; byte5CA decrements when nonzero.
The halfword wraps according to its unsigned storage width. A nonnull contact
node45C is tested by72CB0; an inactive node invokes cleanup callback618 through
the BX-r1 trampoline0824923C and is cleared. This path does not add a null test
for the cleanup callback, matching the original contract.

A714 uses the same availability conditions, returning0 for rejected actors,
zero pending flags, or flags with no handled request. Handled requests return1.
The priority is4,2,800,1,80,100; only the first applies. The resulting flag masks
are respectivelyF678,F67C,F67E,FE7E,FE7F,FE7F. These masks clear additional pending
bits together; they are not just acknowledgments of the tested bit.

Bit2 writes state2A8=3, phase2A9=0, behavior2AA=0, counter2AC=0, fields2B1/2B2=1,
field2B0=0, kind2B5=7, control21C=the saved pointer5DC, and field2C8=0. It clears
flags114 bit1 and word104, then copies signed halfwordA8 to adjacent halfwordAA.
The narrow update view models these adjacent halfwords as a two-element array
so the observed pointer increment stays inside its C array object. Bit800 calls
7CA44. Bits80 and100 choose optional callbacks23C and5EC; the dispatcher tests
the selected callback before invoking the same trampoline. Bits4 and1 only
apply their masks. Existing registry field names and the game-facing purpose of
these numeric states remain provisional.

Parent integration owns name promotion, combined full-ROM validation and RAM
literal auditing; working-branch fast proofs are retained under
build/actor-update-A62C-final.json and build/actor-update-A714-final.json.
