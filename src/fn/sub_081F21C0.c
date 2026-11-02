#include "sprite_trail.h"
struct SpriteTrail *MotionEffectPool_FindFreeEntry(struct SpriteTrailPool *p, s32 *out)
{
    s32 i = 0;
    u32 one = 1;
    u32 f = p->activeMask;
    struct SpriteTrail *q = p->entries;
    struct SpriteTrail *r;
    do {
        if (!((one << i) & f)) {
            *out = i;
            r = q;
            goto end;
        }
        q++;
        i++;
    } while (i <= 3);
    r = 0;
end:
    return r;
}
