#include "global.h"
struct SceneParticle {
    u8 unknown[0x32];
    u8 state;
    s8 index;
    u8 reset, unknown35, timer, phase;
    u8 unknown38[4];
    u8 baseFrame, unknown3d;
    u16 interval;
};
struct SceneParticlePool {
    u8 unknown[0x18];
    void *resource;
    struct SceneParticle particles[26];
    volatile u32 occupied;
};
void sub_08217EEC(void *, void *, u32);
s32 SceneParticle_UpdateAlternatingFrame(struct SceneParticlePool *pool,
                                         struct SceneParticle *particle)
{
    if (particle->timer == 0) {
        particle->timer = particle->interval;
        particle->phase = (particle->phase + 1) & 1;
        sub_08217EEC(particle, pool->resource, particle->baseFrame + particle->phase);
    } else
        particle->timer--;
    return 0;
}
