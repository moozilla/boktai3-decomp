#include "global.h"
#include "ram.h"
#define SceneParticle_Allocate   SceneMovingParticle_AllocateEntry_0812D670
#define SceneParticle_CreatePool ParticleEffectPool_GetOrCreate
struct SceneParticleVector {
    u32 words[2];
};
struct SceneParticle {
    u32 flags;
    u8 unknown4[11], mode;
    u8 unknown10[8];
    struct SceneParticleVector position;
    u8 unknown20[8];
    struct SceneParticleVector velocity;
    u16 elapsed;
    u8 state;
    s8 index;
    u8 reset, delay, timer, phase;
    u16 lifetime;
    u8 unknown3a[2];
    u8 baseFrame, unknown3d;
    s16 interval;
};
struct SceneParticlePool {
    u8 task[0x18];
    void *resource;
    struct SceneParticle particles[26];
    u32 occupied;
};
struct SceneParticlePool *ParticleEffectPool_GetOrCreate(void);
struct SceneParticle *SceneMovingParticle_AllocateEntry_0812D670(struct SceneParticlePool *);
void ParticleSprite_Initialize(struct SceneParticle *, void *, u32);
void Effect_SetOrigin(struct SceneParticle *, s32, s32);
void SpriteDrawable_SelectTileFrame(struct SceneParticle *, void *, u32);
void Effect_SetPalette(struct SceneParticle *, u32);
struct SceneParticle *SceneParticle_Create(struct SceneParticleVector *position,
                                           struct SceneParticleVector *velocity, u32 lifetime,
                                           u32 kind, u32 mode)
{
    struct SceneParticlePool *pool = gSceneParticlePool;
    struct SceneParticle *p;
    u32 frame;
    if (!pool) {
        pool = ParticleEffectPool_GetOrCreate();
        if (!pool)
            return 0;
    }
    p = SceneMovingParticle_AllocateEntry_0812D670(pool);
    if (!p)
        return 0;
    p->state = 1;
    p->elapsed = 0;
    p->reset = 1;
    p->velocity = *velocity;
    p->lifetime = lifetime;
    p->delay = 30;
    p->timer = 0;
    switch (kind) {
        case 1:
            frame = 1;
            break;
        case 2:
            frame = 2;
            break;
        case 3:
            frame = 3;
            break;
        case 4:
            frame = 4;
            break;
        case 5:
            frame = 5;
            break;
        case 6:
            frame = 6;
            break;
        case 7:
            frame = 7;
            break;
        case 0:
        default:
            frame = 0;
            break;
    }
    p->phase = 0;
    p->baseFrame = frame << 1;
    p->interval = 3;
    p->timer = p->interval;
    p->position = *position;
    ParticleSprite_Initialize(p, pool->resource, 0);
    Effect_SetOrigin(p, -4, -4);
    SpriteDrawable_SelectTileFrame(p, pool->resource, p->baseFrame);
    Effect_SetPalette(p, 1);
    p->flags |= 1;
    p->mode = mode;
    return p;
}
