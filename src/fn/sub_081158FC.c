#include "global.h"
#include "ram.h"
void Sprite_Remove(void *);
u32 Collectible_DestroyManager(u8 *p)
{
    u8 *e = p + 0x34;
    s32 i = 0x2f;
    do {
        Sprite_Remove(e);
        e += 0xF8;
        i--;
    } while (i >= 0);
    {
        u32 z = 0;
        gActorParticlePool = z;
    }
    return 0;
}
