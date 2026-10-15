// MATCHING_CONCESSION: register-pinning
#include "scene_particle_burst.h"
#define SceneParticleBurst_Destroy sub_080681DC
extern struct SceneParticleBurstPool *gUnk_02000130;
static inline u8 IsUsed(u32 *used, u32 index)
{
    register u32 one asm("r1") = 1;
    if (*used & (one << index))
        return 1;
    return 0;
}
s32 SceneParticleBurst_Destroy(struct SceneParticleBurstPool *p)
{
    s32 i = 0;
    u32 *used = &p->used;
    struct SceneParticleBurstSlot *s = p->slots;
    for (; i < 32; s++, i++) {
        if (IsUsed(used, i))
            Effect_Remove((u8 *)p + (s->index * 0x28 + 0x20));
    }
    {
        register u32 zero asm("r1") = 0;
        gUnk_02000130 = (void *)zero;
    }
    return 0;
}
