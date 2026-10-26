#include "global.h"
#include "ram.h"
void Sprite_Remove(u8 *);
u32 SpritePool_Destroy(u8 *p)
{
    u8 *e = p + 0x18;
    s32 i = 0xf;
    do {
        if (e[0x10] != 0)
            Sprite_Remove(e + 0xc);
        i--;
        e += 0xb0;
    } while (i >= 0);
    gSpritePoolManager = 0;
    return 0;
}
