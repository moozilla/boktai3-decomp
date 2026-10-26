#include "global.h"
#include "ram.h"
struct S {
    u8 f[0x18];
    u32 p;
    u8 g[0x69c - 0x1c];
    u32 c;
};
u32 AnimData_FindEntry(u32);
s32 ParticleEffectPool_Init(struct S *s)
{
    s->p = AnimData_FindEntry(0x1c1e);
    s->c = 0;
    gSceneParticlePool = s;
    return 0;
}
