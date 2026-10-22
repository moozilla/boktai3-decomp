#include "global.h"
// MATCHING_CONCESSION: register-pinning

void Sprite_Remove(void *);
void Collider_Remove(void *);

void Armor6_DestroySpriteColliders(u8 *parts)
{
    u8 *collider = parts + 0x9C;
    u8 *part = parts;
    s32 i = 2;
    do {
        Sprite_Remove(part);
        Collider_Remove(collider);
        {
            register u32 stride asm("r0") = 0x1E4;
            collider += stride;
            part += stride;
        }
    } while (--i >= 0);
}
