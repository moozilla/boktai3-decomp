#include "global.h"

void Sprite_Remove(u8 *);

u32 Explosion_Destroy(u8 *p)
{
    Sprite_Remove(p + 0x18);
    return 0;
}
