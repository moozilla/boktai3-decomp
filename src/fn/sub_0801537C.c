#include "global.h"
#include "ram.h"
void Sprite_Remove(u8 *);
u32 AnimatedParticleManager_Destroy(u8 *s)
{
    u8 *a = s + 0x60;
    u8 *b = s + 0x24;
    s32 i = 0x1f;
    do {
        if (*b != 0)
            Sprite_Remove(a);
        a += 0x78;
        b += 0x78;
        i--;
    } while (i >= 0);
    {
        u32 z = 0;
        gAnimatedParticleManager = z;
    }
    return 0;
}
