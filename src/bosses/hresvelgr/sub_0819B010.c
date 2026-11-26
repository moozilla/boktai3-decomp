#include "global.h"
#include "objects/dungeon_prop/orbit_motion.h"
/* Math_BlendWithTableFactor: Blends endpoints using a capped table index and returns the target for a zero table entry */

extern const s16 gUnk_08611D24[];

s32 Math_BlendWithTableFactor(s32 from, s32 to, s32 t)
{
    if (t > 15)
        t = 15;
    t = gUnk_08611D24[t];
    if (t == 0)
        return to;
    return (to - from) * scale8(t) + from;
}
