#include "global.h"
#include "ram.h"
s32 ScriptedParticle_InitPool(u8 *s)
{
    u8 *p;
    s32 i;
    u32 m;
    u32 t;
    gScriptedParticlePool = s;
    m = 0xff;
    p = s + 0x88;
    i = 0x2f;
    do {
        t = *p;
        t = t | m;
        *p = t;
        p += 0x70;
        i--;
    } while (i >= 0);
    return 0;
}
