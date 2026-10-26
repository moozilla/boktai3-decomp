#include "global.h"
#include "ram.h"
void Sprite_Remove(void *);
u32 TransientSpritePool_Destroy(u8 *s)
{
    u8 *p = s + 0x18;
    s32 i = 0xf;
    do {
        if (p[8] != 0)
            Sprite_Remove(p + 4);
        i--;
        p += 0x4c;
    } while (i >= 0);
    return gTransientSpritePool = 0;
}
