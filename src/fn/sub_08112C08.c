#include "global.h"
#include "ram.h"
void Sprite_Remove(void *);
void ActorMarkerPool_RemoveMarker(u32 i)
{
    u8 *g = gPanelSpritePool;
    if (g != 0 && i <= 7) {
        u32 off = i * 0x60;
        u8 *q = g + off;
        if (q[0x38] != 0) {
            u8 *r = g + (off + 0x18);
            u8 *t;
            u32 z;
            Sprite_Remove(r + 0x1c);
            t = g + 0x74;
            t = t + off;
            z = 0;
            *(u32 *)t = z;
        }
    }
}
