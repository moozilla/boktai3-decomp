#include "objects/warp_circle/warp_circle.h"
void Sound_PlaySong(u32);
void WarpCircle_ExtendForScript(struct WarpCircle *p)
{
    u16 *c = &p->height;
    if (*c <= 6) {
        *c += 1;
        WarpCircle_UpdateSegments(p);
    } else {
        u16 *d = &p->frame;
        u32 v = *d + 1;
        *d = v;
        if ((u16)v > 0x1f) {
            Sound_PlaySong(0x15f);
            WarpCircle_SetUpdate(p, WarpCircle_WaitForScriptParticles);
        }
    }
}
