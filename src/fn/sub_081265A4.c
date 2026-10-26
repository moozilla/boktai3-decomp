#include "global.h"
#include "ram.h"
void Sprite_Remove(u8 *);
u32 ScriptedParticle_DestroyPool(u8 *p)
{
    u8 *r = p + 0x88;
    u8 *q = p + 0x1c;
    s32 n = 0x2f;
    u32 z;
    do {
        if (*(s8 *)r >= 0)
            Sprite_Remove(q);
        r += 0x70;
        q += 0x70;
        n--;
    } while (n >= 0);
    z = 0;
    gScriptedParticlePool = z;
    return z;
}
