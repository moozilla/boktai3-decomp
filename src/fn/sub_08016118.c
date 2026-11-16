#include "fading_particle_group.h"
#include "ram.h"
#include "display_sequence_inline.h"
void Anim_SetDurationScale(void *, int);
void Effect_Remove(void *);
static inline int scale(int a, int b)
{
    int v = a * b;
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline u32 frame_factor(u16 v)
{
    return DisplaySequence_MaskCommand(v, 15);
}
int FadingParticleGroup_Update(struct FadingParticleGroup *g)
{
    int alive = 0, i = 0;
    if (i < g->count) {
        u8 *life = &g->particles[0].lifetime;
        struct DisplaySequenceState *anim = &g->particles[0].motion.anim;
        s16 *speed = &g->particles[0].speed;
        struct FadingParticleMotion *p = &g->particles[0].motion;
        do {
            if (*life) {
                struct DisplaySequenceState *a;
                const u16 *entry;
                const struct SpriteEffectTileResource *resource;
                u32 frame;
                alive++;
                p->sprite.position.x += scale(p->velocity.x, *speed);
                p->sprite.position.y += p->velocity.y;
                p->sprite.position.z += scale(p->velocity.z, *speed);
                p = (struct FadingParticleMotion *)((u32)p + (p->anim.flags >> 8));
                p->velocity.y -= 2;
                Anim_SetDurationScale(anim, 16);
                resource = ((struct FadingParticleManager *)gFadingParticleManager)->graphics;
                a = anim;
                entry = p->anim.commands + p->anim.index;
                frame = *entry >> 6;
                p->sprite.tile =
                    resource->tileBase + (p->sprite.width >> 3) * (p->sprite.height >> 3) * frame;
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = a->flags;
                    bit = 1;
                    first = (u8)(control.raw & bit);
                    if (first != (((*entry & 0x30) >> 4) & bit))
                        p->sprite.flags |= 4;
                    else
                        p->sprite.flags &= ~4;
                }
                {
                    union DisplaySequenceControl control;
                    u32 bit, first;
                    control.raw = a->flags;
                    bit = 2;
                    first = (u8)(control.raw & bit);
                    if (first != (((*entry & 0x30) >> 4) & bit))
                        p->sprite.flags |= 8;
                    else
                        p->sprite.flags &= ~8;
                }
                if (++a->elapsed >= a->duration) {
                    a->elapsed = 0;
                    if (a->flags & 4) {
                        if (a->index == 0) {
                            if (a->flags & 8) {
                                a->elapsed = a->duration;
                            } else {
                                a->index = a->frameCount - 1;
                            }
                        } else {
                            a->index--;
                        }
                    } else {
                        if (++a->index >= a->frameCount) {
                            if ((u8)(a->flags & 8)) {
                                a->elapsed = a->duration;
                                a->index = a->frameCount - 1;
                            } else {
                                a->index = 0;
                            }
                        }
                    }
                    entry = a->commands + a->index;
                    a->duration = ((int)frame_factor(*entry) * a->speed) >> 6;
                }
                {
                    u32 raw = *life;
                    u32 compare;
                    u32 time;
                    compare = raw;
                    if ((s32)raw < 0)
                        raw = 0;
                    time = raw;
                    if (compare > 8) {
                        if (p->anim.index == 5)
                            p->anim.index = 0;
                    } else if (compare == 8)
                        p->anim.index = 0;
                    raw = time - 1;
                    *life = raw;
                    if ((u8)raw == 255)
                        p->sprite.flags |= 1;
                }
            } else
                p->sprite.flags |= 1;
            life += 68;
            anim = (void *)((u8 *)anim + 68);
            speed = (void *)((u8 *)speed + 68);
            p = (void *)((u8 *)p + 68);
            i++;
        } while (i < g->count);
    }
    if (!alive) {
        g->active = 0;
        for (i = 0; i < g->count; i++)
            Effect_Remove(&g->particles[i].motion.sprite);
        g->count = 0;
    }
    return 0;
}
