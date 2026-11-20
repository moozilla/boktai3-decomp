#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "monster_particle_random.h"

#include "fading_particle_group.h"
extern const s16 gUnk_086149C4[];
void *FadingParticleManager_Create(u32, u32);
s32 Mod(s32, s32);
void sub_08217E40(void *, u32);
u32 Anim_SetSequence(void *, void *, u32, u32, u32);
static inline u16 FadingParticles_NextRandom(u32 *r, const u16 *table)
{
    *r = (*r + 1) & 1023;
    return table[*r];
}
s32 FadingParticleGroup_Spawn(u32 count, struct ContactPosition *position, s32 life, s32 lifeRange,
                              s32 speed, s32 speedRange, s32 angle, s32 angleRange, s32 rise,
                              s32 riseRange)
{
    s32 i, j;
    u32 *rng;
    const u16 *table;
    const s16 *sine;
    if (gFadingParticleManager == NULL && FadingParticleManager_Create(0, 0) == NULL)
        return -1;
    for (i = 0; i < 6; i++) {
        struct FadingParticleGroup *g =
            &((struct FadingParticleManager *)gFadingParticleManager)->groups[i];
        rng = &gRandomIndex;
        if (g->active == 0) {
            union {
                struct FadingParticleGroup *group;
                union ParticleRandomTableSource random;
            } input;
            g->active = 1;
            g->count = count;
            if ((u8)count > 8)
                g->count = 8;
            j = 0;
            input.group = g;
            if (j < input.group->count) {
                struct DisplaySequenceState *anim;
                struct SpriteDrawable *sprite;
                struct FadingParticle *p;
                input.random.halves[0] = 0xB400;
                input.random.halves[1] = 0x0203;
                table = ParticleRandom_GetTable(&input.random);
                anim = &g->particles[0].motion.anim;
                sprite = &g->particles[0].motion.sprite;
                p = &g->particles[0];
                sine = gUnk_086149C4;
                do {
                    s32 a;
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
                    Anim_SetSequence(
                        anim, ((struct FadingParticleManager *)gFadingParticleManager)->animations,
                        3, 0, 0);
                    anim =
                        (struct DisplaySequenceState *)((u8 *)anim + sizeof(struct FadingParticle));
                    sprite =
                        (struct SpriteDrawable *)((u8 *)sprite + sizeof(struct FadingParticle));
                    p++;
                    j++;
                } while (j < g->count);
            }
            return 0;
        }
    }
    return -1;
}
