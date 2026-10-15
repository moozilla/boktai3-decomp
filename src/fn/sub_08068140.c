// MATCHING_CONCESSION: register-pinning
#include "scene_particle_burst.h"
#define SceneParticleBurst_Update sub_08068140
extern u32 gUnk_030053F4, gUnk_030053F0, gUnk_0300523C;
extern void *gUnk_08605B50[];
static inline u8 IsUsed(struct SceneParticleBurstPool *p, u32 index)
{
    if (p->used & (1U << index))
        return 1;
    return 0;
}
s32 SceneParticleBurst_Update(struct SceneParticleBurstPool *input)
{
    register struct SceneParticleBurstPool *p asm("r6") = input;
    if (p->used) {
        s32 i = 0;
        u32 pauseMask = 0x200;
        struct SceneParticleBurstSlot *slot;
        slot = p->slots;
        for (; i < 32; slot++, i++) {
            if (IsUsed(p, i)) {
                struct SceneParticleBurstSlot *current = slot;
                if (((gUnk_030053F4 | gUnk_030053F0) & pauseMask) ||
                    ((gUnk_030053F4 & 0x800) && (gUnk_0300523C & 7))) {
                    if (slot->flags & 1)
                        continue;
                }
                sub_08249240(p, current, gUnk_08605B50[current->state]);
            }
        }
    }
    return 0;
}
