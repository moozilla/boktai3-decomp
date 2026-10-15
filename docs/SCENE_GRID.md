# Scene-grid position helpers (0806E34C onward)

These helpers use an eight-column, ten-row signed-byte table at 08605BC8. Its
local origin maps to terrain cell (5, 8): a local `(column,row)` becomes cell
`column + 5 + (row + 8) * width`. Names are provisional; no game-screen identity
or runtime coverage is claimed. The terrain width is read as signed 16-bit.
The expected input domain has a loaded terrain map with positive width and
valid board coordinates; signed step counts select a direction and distance.

The independent 388-byte sampler 6E34C reads a source world position, converts
its signed X/Z coordinates to a terrain cell (invalid coordinates select zero),
and draws two successive RNG samples within the square radius. It tries one
candidate and succeeds only when its board entry equals the source terrain's
height nibble. Success writes the candidate terrain-cell center; failure leaves
the output unchanged. The second incoming argument is unused. It neither
retries nor clamps the sampled board coordinate; callers must keep its sample
window within the scene table's domain.

`6E4D0` splits two cell indices by terrain width and writes their column and
row deltas. Its 104-byte body is ordinary exact C. `6E538` resolves an endpoint
using sentinel -255: if both indices are absent, it fills the second position
from default world cell (10, 12); if only the second is absent, it fills the first
from the absolute first index and returns 1; otherwise it fills the first from
the absolute second index. Other paths return -1. Position X/Z select cell
centers (`coordinate*256+128`), and Y is the height nibble times 256. Each height
lookup first consults 0821D64C(index,1), falling back to the terrain grid.

`6E690` and `6E7CC` scan rows and columns respectively, stopping at the requested
distance, the board edge, or a different table entry. Zero steps return -255.
They convert the last accepted local coordinate to a terrain index. If that
index equals the blocked index, they check a table entry two cells farther in
the requested direction and return its negated terrain index; failure returns
-255. This signed-index encoding is preserved without guessing its gameplay
meaning. Their 316/308-byte bodies match complete reviewed extents. Integer
cursors preserve unused final strides without constructing out-of-range C
pointers; dereferences remain guarded by the board bounds.

`6E900` scans terrain in one of four cardinal directions, selected by the
rounded angle. Each step reads the height nibble and then the flag word through
separate dynamic-cell lookups, falling back to the map's cell properties.
Flag `0x80` stops traversal. The scan remembers an accepted cell at the starting
height and handles height transitions before returning that cell or the last
visited cell. Its 624-byte extent is exact with nine register hints. The map's
boundary flags supply the stopping condition; the function does not add a
separate coordinate-bounds check.

A separate 916-byte route resolver begins 6EB70, with its own save/stack prologue
and pop/bx return before 6EF04. This boundary was previously hidden inside the
unmatched 6E900 span; separating it adds no byte denominator and no duplicate
credit. The retained resolver remains assembly-backed pending recovery.

The root grid cohort adds 2,084 exact bytes: **+0.0863 percentage points**,
comprising 104 ordinary bytes (0.0043 points) and 1,980 register-pinned bytes
(0.0820 points). Each full reviewed span passed
its per-function checker and final combined-image check; the complete ROM
build and RAM-literal audit passed.
