# Scene actor target and contact control

Root's contiguous E9D0–8005C section links registry targets, contact ownership,
route control and animation responses. Names remain tentative; static analysis
here does not claim runtime coverage.

F710 resolves a signed-positive pending ID at304. It calls F4B8 to reset the
contact response, clears group414 bit2, sets2D6 bit80 and clears100, then sets
EB bit10. It looks up the low16 bits of the pending ID in the contact registry,
stores the resulting object atDC, and requires that object to exist before
reading its position at+0C. Signed target X/Z shifted by8 select a terrain cell
with map-bounds fallback0; this halfword is stored at2F8, distinct from the
terrainCell field2F4 used by other phase views. Wrapped16-bit XYZ differences
from actor position5C feed Math_Atan2(X,Z), whose low byte becomes direction59.
The vector pad is never read as a scalar. It returns1 after this path. Without
a positive ID, EB bit2 gates clearing EB bit10, and it returns0.

All268 bytes match with six minimized register/lifetime hints. The adjoining
F4B8 is exact over600 bytes with14 minimized hints: it releases owned contact45C, resets command460 and
route control, installs FC90/substate16, resets actor state3/1/0, clears contact
flags, releases attachment C72C, and records the same capped per-player counters
as789EC. It requires the current target object for the per-player counter lookup. Counters
cap at9999 using the observed signed-halfword comparisons, matching789EC.
F4B8 plusF710 recover868 exact bytes (+0.0359 percentage points).
