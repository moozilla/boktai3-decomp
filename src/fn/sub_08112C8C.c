#include "global.h"
#include "ram.h"
void ActorMarkerPool_ShowMarker(s32 i)
{
    if (i >= 0) {
        u8 *g = gPanelSpritePool;
        if (g != 0) {
            s32 off = i * 0x60;
            u8 *q = g + 0x74;
            u8 *r;
            q = q + off;
            if (*(u32 *)q != 0) {
                r = g + 0x34;
                r = r + off;
                *(u32 *)r &= ~1;
            }
        }
    }
}
