// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct Vec {
    s16 x, y, z, pad;
};
struct Sprite {
    u32 flags;
    u8 pad4[4];
    u16 tile;
    u8 width, height;
    u8 padc[12];
    struct Vec position;
    u8 tail[8];
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Motion {
    struct Vec velocity;
    struct Sprite sprite;
    struct Anim anim;
};
struct Particle {
    u8 active, lifetime;
    s16 speed;
    struct Motion motion;
};
struct Group {
    u8 active, count, pad[2];
    struct Particle particles[8];
};
struct Manager {
    u8 pad[28];
    u8 *resource;
    void *animations;
    struct Group groups[6];
};
extern struct Manager *gFadingParticleManager;
extern u32 gRandomIndex;
extern const u16 gUnk_0203B400[];
extern const s16 gUnk_086149C4[];
void *FadingParticleManager_Create(u32, u32);
s32 Mod(s32, s32);
void sub_08217E40(void *, u32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
static inline u16 FadingParticles_NextRandom(u32 *r, const u16 *table)
{
    *r = (*r + 1) & 1023;
    return table[*r];
}
s32 FadingParticleGroup_Spawn(u32 count, struct Vec *position, s32 life, s32 lifeRange, s32 speed,
                              s32 speedRange, s32 angle, s32 angleRange, s32 rise, s32 riseRange)
{
    s32 i, j;
    u32 *rng;
    register const u16 *table asm("r6");
    if (gFadingParticleManager == NULL && FadingParticleManager_Create(0, 0) == NULL)
        return -1;
    for (i = 0; i < 6; i++) {
        struct Group *g = &gFadingParticleManager->groups[i];
        rng = &gRandomIndex;
        if (g->active == 0) {
            g->active = 1;
            g->count = count;
            if ((u8)count > 8)
                g->count = 8;
            j = 0;
            if (j < g->count) {
                struct Anim *anim;
                struct Sprite *sprite;
                struct Particle *p;
                table = gUnk_0203B400;
                anim = &g->particles[0].motion.anim;
                sprite = &g->particles[0].motion.sprite;
                p = &g->particles[0];
                do {
                    s32 a;
                    register const s16 *sine asm("r2");
                    p->active = 1;
                    if (lifeRange == 0)
                        p->lifetime = life;
                    else
                        p->lifetime = life + Mod(FadingParticles_NextRandom(rng, table), lifeRange);
                    if (speedRange == 0)
                        p->speed = speed;
                    else
                        p->speed = speed + Mod(FadingParticles_NextRandom(rng, table), speedRange);
                    if (angleRange == 0)
                        a = angle;
                    else
                        a = angle + Mod(FadingParticles_NextRandom(rng, table), angleRange);
                    sine = gUnk_086149C4;

                    p->motion.velocity.x = sine[(a + 64) & 255];
                    p->motion.velocity.z = sine[a & 255];
                    if (riseRange == 0)
                        p->motion.velocity.y = rise;
                    else
                        p->motion.velocity.y =
                            rise + Mod(FadingParticles_NextRandom(rng, table), riseRange);
                    p->motion.sprite.position = *position;
                    sub_08217E40(sprite, 0);
                    p->motion.sprite.flags &= ~1;
                    Anim_SetSequence(anim, gFadingParticleManager->animations, 3, 0, 0);
                    anim = (struct Anim *)((u8 *)anim + sizeof(struct Particle));
                    sprite = (struct Sprite *)((u8 *)sprite + sizeof(struct Particle));
                    p++;
                    j++;
                } while (j < g->count);
            }
            return 0;
        }
    }
    return -1;
}
