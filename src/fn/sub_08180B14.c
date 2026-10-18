#include "global.h"
#define SceneCombatActor_InitBouncingParticle sub_08180B14
#define SceneParticle_UpdateBounce            sub_08180A88
#define SceneParticle_HideSprite              sub_08180AFC
#define gSceneCombatParticleFrameBases        gUnk_0824E6A4
struct ParticleVector {
    s32 x : 16, y : 16, z : 16;
    u32 : 16;
};
struct BouncingParticleSprite {
    u32 flags;
    u8 unknown4[2], phase;
    u8 unknown7[9];
    u16 frame;
    u8 unknown12[0xa];
    struct ParticleVector position;
    u8 unknown24[8];
};
struct BouncingSceneParticle {
    void (*update)(void *), (*destroy)(void *);
    struct BouncingParticleSprite sprite;
    struct ParticleVector velocity;
    s16 floor;
    u8 bounced;
};
struct SceneCombatActor {
    u8 unknown0[0xd6];
    u16 floor;
    u8 unknownd8[0x318 - 0xd8];
    u8 particleAnimation[0x1c];
    u8 unknown334[0x390 - 0x334];
    struct BouncingSceneParticle particle;
    u8 unknown3d0[0xb38 - 0x3d0];
    u8 kind, heading;
};
void Sprite_Init(void *, void *, u32);
void sub_08180A88(void *);
void sub_08180AFC(void *);
extern const u8 gUnk_0824E6A4[];
static inline void SetParticleCallbacks(struct BouncingSceneParticle *p, void (*update)(void *),
                                        void (*destroy)(void *))
{
    p->update = update;
    p->destroy = destroy;
}
void sub_08180B14(struct SceneCombatActor *p)
{
    struct BouncingParticleSprite *sprite = &p->particle.sprite;
    s32 frame, direction;
    struct BouncingSceneParticle *particle;
    Sprite_Init(sprite, p->particleAnimation, 0);
    sprite->flags = 0x202;
    SetParticleCallbacks(&p->particle, sub_08180A88, sub_08180AFC);
    p->particle.bounced = 0;
    p->particle.floor = p->floor;
    frame = gUnk_0824E6A4[p->kind];
    direction = ((p->heading + 0x60) >> 6) & 3;
    switch (direction) {
        case 0:
            sprite->frame = frame;
            break;
        case 1:
            sprite->frame = frame + 1;
            break;
        case 2:
            sprite->frame = frame;
            sprite->flags |= 4;
            break;
        case 3:
            sprite->frame = frame + 1;
            sprite->flags |= 4;
            break;
    }
}
