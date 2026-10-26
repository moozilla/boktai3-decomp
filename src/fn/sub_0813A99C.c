#include "global.h"
#include "ram.h"
void ActorMarker_ShowSlot(s32 i0)
{
    s32 i = i0;
    u8 *b;
    u8 *q;
    if (i >= 0) {
        b = gCompactSpritePool;
        if (b != 0) {
            i *= 0x64;
            q = b + 0x78;
            if (*(u32 *)(q + i) != 0) {
                u8 *r = b + 0x34;
                u32 *p = (u32 *)(r + i);
                *p = *p & ~1;
            }
        }
    }
}
