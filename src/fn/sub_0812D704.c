#include "global.h"
#define SceneParticle_UpdateMotion sub_0812D704
#define SceneParticle_UpdateAlternatingFrame sub_0812D6C4
struct SceneParticle {
    u32 flags; u8 unknown4[0x14];
    u16 x; u8 unknown1a[2]; u16 y; u8 unknown1e[0xa];
    u16 velocityX; u8 unknown2a[2]; u16 velocityY; u8 unknown2e[4];
    u8 state; s8 index; u8 reset, unknown35, timer, phase;
    u16 lifetime; u8 unknown3a[2]; u8 baseFrame, unknown3d; u16 interval;
};
struct SceneParticlePool;
s32 sub_0812D6C4(struct SceneParticlePool *, struct SceneParticle *);
void sub_0812D704(struct SceneParticlePool *pool, struct SceneParticle *particle)
{
    if (particle->reset) { particle->flags &= ~1; particle->reset = 0; }
    particle->x = particle->x + particle->velocityX;
    particle->y = particle->y + particle->velocityY;
    sub_0812D6C4(pool, particle);
    if (particle->lifetime) particle->lifetime--;
    else { particle->state = 2; particle->reset = 1; }
}
