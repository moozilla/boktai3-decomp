#include "global.h"

struct ParticleVector {
    s32 x : 16, y : 16, z : 16;
    u32 : 16;
};
struct BouncingParticleSprite {
    u32 flags;
    u8 unk_04[12];
    u16 frame;
    u8 unk_12[10];
    struct ParticleVector position;
    u8 unk_24[8];
};
struct BouncingSceneParticle {
    void (*update)(void *), (*destroy)(void *);
    struct BouncingParticleSprite sprite;
    struct ParticleVector velocity;
    s16 floor;
    u8 bounced;
};
struct ParticleAnimation {
    u8 unk_00[0x1c];
};
struct ActorMarker {
    u8 unk_00[0xd6];
    u16 floor;
    u8 unk_D8[0x2e0 - 0xd8];
    struct ParticleAnimation animations[2];
    u8 unk_318[0x410 - 0x318];
    struct BouncingSceneParticle particles[9];
    u8 unk_650[0xb38 - 0x650];
    u8 kind, direction;
};
void Sprite_Init(void *, void *, u32);
void SceneParticle_UpdateBounce(void *);
void BouncingEffect_RemoveSprite(void *);
extern const u8 gUnk_0824E6D4[];
extern const u8 gUnk_0824E6E6[];

static inline void SetParticleFrame(struct BouncingParticleSprite *sprite, u32 frame)
{
    sprite->frame = frame;
}

static inline void SetParticleCallbacks(struct BouncingSceneParticle *particle,
                                        void (*update)(void *), void (*destroy)(void *))
{
    particle->update = update;
    particle->destroy = destroy;
}

void ActorMarker_InitBouncingParticles(struct ActorMarker *p)
{
    s32 flip = 0;
    s32 side = 0;
    s32 i;
    s32 direction = ((p->direction + 0x60) >> 6) & 3;
    switch (direction) {
        case 0:
            side = 1;
            break;
        case 1:
            side = 0;
            break;
        case 2:
            side = 0;
            flip = 1;
            break;
        case 3:
            side = 1;
            flip = 1;
            break;
    }
    /* Each direction selects one of the two nine-frame groups. */
    for (i = 0; i < 9; i++) {
        u32 frame = gUnk_0824E6D4[i + side * 9];
        u32 animation;
        switch (p->kind) {
            case 0:
            case 1:
            case 2:
            case 4:
            case 6:
            case 8:
            case 10:
            case 11:
            case 12:
            case 14:
            default: {
                const u8 *table = gUnk_0824E6E6;
                table += 9;
                animation = table[i];
                break;
            }
            case 3:
            case 5:
            case 7:
            case 9:
            case 13:
            case 15:
                animation = gUnk_0824E6E6[i];
                break;
        }
        Sprite_Init(&p->particles[i].sprite, &p->animations[animation], 0);
        SetParticleFrame(&p->particles[i].sprite, frame);
        p->particles[i].sprite.flags = 2;
        if (flip)
            p->particles[i].sprite.flags = 6;
        if (1 & i)
            p->particles[i].sprite.flags |= 0x200;
        else
            p->particles[i].sprite.flags |= 0x400;
        SetParticleCallbacks(&p->particles[i], SceneParticle_UpdateBounce,
                             BouncingEffect_RemoveSprite);
        p->particles[i].bounced = 0;
        p->particles[i].floor = p->floor;
    }
}
