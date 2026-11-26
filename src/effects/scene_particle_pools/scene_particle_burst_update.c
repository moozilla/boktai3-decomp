#include "engine/shared/scene_particle_burst.h"
#include "engine/shared/render.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "engine/shared/engine_pause.h"
extern void *gUnk_08605B50[];
static inline u8 IsUsed(struct SceneParticleBurstPool *p, u32 index)
{
    if (p->used & (1U << index))
        return 1;
    return 0;
}
s32 SceneParticleBurst_Update(struct SceneParticleBurstPool *input)
{
    struct SceneParticleBurstPool *p = input;
    if ((*(&p))->used) {
        s32 i = 0;
        u32 pauseMask = 0x200;
        struct SceneParticleBurstSlot *slot;
        slot = p->slots;
        for (; i < 32; slot++, i++) {
            if (IsUsed(p, i)) {
                struct SceneParticleBurstSlot *current = slot;
                if (Engine_PauseBits(pauseMask) ||
                    ((gEngineFlags & 0x800) && (gEnginePauseFlags & 7))) {
                    if (slot->flags & 1) {
                        continue;
                    }
                }
                sub_08249240(p, current, gUnk_08605B50[current->state]);
            }
        }
    }
    return 0;
}
