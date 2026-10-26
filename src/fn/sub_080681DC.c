#include "scene_particle_burst.h"
#include "ram.h"
static inline u8 IsUsed(u32 *used, u32 index)
{
    u32 one = 1;
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
        u32 zero = 0;
        gSceneParticleBurstPool = (void *)zero;
    }
    return 0;
}
