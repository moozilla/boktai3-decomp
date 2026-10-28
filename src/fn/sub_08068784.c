// MATCHING_CONCESSION: register-pinning
#include "scene_particle_pool.h"
#include "ram.h"
extern void *gUnk_08605B5C[];
static inline u8 Occupied(const u32 *mask, u32 bit)
{
    if (*mask & bit)
        return TRUE;
    return FALSE;
}
s32 SceneParticlePool_Update(struct SceneParticlePool *input)
{
    struct SceneParticlePool *pool = input;
    struct SceneParticleSlot *slot;
    s32 i;
    register u32 maskOffset asm("r1") = (u32) & ((struct SceneParticlePool *)0)->used;
    register u32 *guard asm("r0");
    asm("" : "+r"(maskOffset));
    guard = (void *)((u32)pool + maskOffset);
    if (*guard) {
        u32 *used;
        void **table;
        i = 0;
        {
            u32 offset = (u32) & ((struct SceneParticlePool *)0)->used;
            used = (void *)((u32)pool + offset);
        }
        {
            register u32 offset asm("r1") = (u32) & ((struct SceneParticlePool *)0)->slots;
            asm("" : "+r"(offset));
            slot = (void *)((u32)pool + offset);
        }
        table = gUnk_08605B5C;
        for (; i < 8; slot++, i++) {
            if (Occupied(used, 1u << i))
                sub_08249240(pool, slot, table[slot->state]);
        }
    }
    return 0;
}
