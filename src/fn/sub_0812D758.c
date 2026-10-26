#include "global.h"
#include "ram.h"
struct SceneParticle {
    u32 flags;
    u8 unknown4[0x14];
    u16 x;
    u8 unknown1a[2];
    u16 y;
    u8 unknown1e[0xa];
    u16 velocityX;
    u8 unknown2a[2];
    u16 velocityY;
    u8 unknown2e[2];
    u16 elapsed;
    u8 state;
    s8 index;
    u8 reset, unknown35, timer, phase;
    u16 lifetime;
    u8 unknown3a[2];
    u8 baseFrame, unknown3d;
    u16 interval;
};
struct SceneParticlePool {
    u8 unknown[0x1c];
    struct SceneParticle particles[26];
    u32 occupied;
};
void Effect_Remove(void *);
void SceneParticle_Remove(void *unused, struct SceneParticle *particle)
{
    if (particle->reset)
        particle->reset = 0;
    {
        struct SceneParticlePool *pool = gSceneParticlePool;
        s32 index = particle->index;
        pool->occupied &= ~(1 << index);
    }
    particle->state = 0;
    Effect_Remove(particle);
    particle->index = -1;
    particle->elapsed = 0;
    particle->lifetime = 0;
}
