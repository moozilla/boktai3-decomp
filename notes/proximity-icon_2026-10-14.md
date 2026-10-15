# Player-proximity keyframe icon

The tentative ProximityIcon cohort spans08057678–08057960: five functions /744
bytes. Three new bodies add668 bytes /0.0277 percentage points without hints;
the destructor and creator (76 bytes) receive typed contracts without credit.

Each0x84-byte list9 task has type18, unsigned radius1A, a60-byte keyframe
animation1C and origin7C. Initialization copies and relocates resourceCB05/82F1
without a null guard, preserving the original behavior. Script p gives origin
XYZ(defaultzero), o gives animation offsetXYZ(defaultzero), I radius(default384),
t type(defaultzero), and R priority(default1, truncated tobyte). Type0 selects
frame d(default0), type1 selects d+4, other types select frame0. Animation position
is offset+origin with halfword truncation; origin itself remains unchanged.
Constructor mode0, argument0 and scale60 are retained. Packed bitfields for the
temporary offset and final position reproduce the original unspecified padding
and register operations; ordinary structs changed those operations. No register
binding or empty barrier is needed.

The proximity predicate requires the local player pointer, then checks absolute
X and Z differences against radius, inclusive. Y is ignored: this is a square
in the X/Z plane, not a circle. Update shows the icon only when control flag200
is clear and proximity holds; otherwise it hides via animation flag1. Initial
visibility follows the same rules. No completion callback or singleton is used.
Destruction unlinks the animation. Creation retains its negative-init failure
branch although the recovered initializer returns zero on its ordinary paths.

All five complete reviewed extents match. Identity and runtime usage remain
unestablished; no new execution coverage is claimed.
