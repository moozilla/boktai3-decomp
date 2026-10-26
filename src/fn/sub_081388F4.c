#include "global.h"
#include "ram.h"
struct E {
    u8 f[0x4];
    u8 b;
    u8 pad[0x5b];
};
void Sprite_Remove(void *);
s32 PositionEffectDisplay_Destroy(u8 *s)
{
    s32 i;
    u32 z;
    struct E *p = (struct E *)(s + 0x34);
    i = 0x3;
    do {
        if (p->b != 0)
            Sprite_Remove(p);
        p++;
    } while (--i >= 0);
    z = 0;
    gSpriteSlots = z;
    return 0;
}
