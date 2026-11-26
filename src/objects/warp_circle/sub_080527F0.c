#include "objects/warp_circle/warp_circle.h"
void Sound_PlaySong(u32);
void WarpCircle_ExtendSegments(struct WarpCircle *o)
{
    u16 *t = &o->frame;
    if (*t == 0)
        Sound_PlaySong(0x160);
    if (o->height <= 6) {
        o->height++;
        WarpCircle_UpdateSegments(o);
    } else if (*t > 0x26) {
        WarpCircle_SetUpdate(o, WarpCircle_WaitForParticles);
    }
    o->frame++;
}
