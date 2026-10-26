#include "global.h"
#include "ram.h"
struct E {
    u8 f[4];
    u8 a;
    u8 g[0x5c - 5];
};
void Sprite_Remove(void *);
void Hresvelgr_DestroyEffectPool(u8 *p)
{
    struct E *e = (struct E *)(p + 0x5c);
    s32 i = 0x1f;
    do {
        if (e->a != 0)
            Sprite_Remove(e);
        e++;
        i--;
    } while (i >= 0);
    gMotionEntryManager = 0;
}
