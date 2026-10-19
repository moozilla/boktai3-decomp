#include "global.h"
#define SceneParticle_UpdatePool     sub_0812D8EC
#define gSceneParticleStateCallbacks gUnk_08610808
struct SceneParticle {
    u8 unknown[0x32];
    u8 state;
    u8 unknown33[0xd];
};
struct SceneParticlePool {
    u8 task[0x1c];
    struct SceneParticle particles[26];
    volatile u32 occupied;
};
extern void *const gUnk_08610808[];
s32 sub_08249240(void *, void *, void *);
static inline u8 SceneParticle_IsOccupied(volatile u32 *flags, s32 index)
{
    if (*flags & (1 << index))
        return 1;
    return 0;
}
s32 sub_0812D8EC(struct SceneParticlePool *pool)
{
    struct SceneParticle *particle = pool->particles;
    s32 i;
    u8 *state;
    if (*(u32 *)((u8 *)pool + 0x69c)) {
        volatile u32 *flags;
        i = 0;
        flags = &pool->occupied;
        state = &particle->state;
        for (; i <= 25; state += 0x40, particle++, i++) {
            if (SceneParticle_IsOccupied(flags, i))
                sub_08249240(pool, particle, gUnk_08610808[*state]);
        }
    }
    return 0;
}
