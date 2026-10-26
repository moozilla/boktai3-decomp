#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
};
extern struct Manager *gFadingParticleManager;
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
    register u32 value asm("r1") = v;
    u32 mask = 15;
    return mask & value;
}
int FadingParticleGroup_Update(struct Group *g)
{
    int alive = 0, i = 0;
    if (i < g->count) {
        u8 *life = &g->particles[0].lifetime;
        struct Anim *anim = &g->particles[0].motion.anim;
        s16 *speed = &g->particles[0].speed;
        struct Motion *p = &g->particles[0].motion;
        do {
            if (*life) {
                struct Anim *a;
                const u16 *entry;
                register u16 n asm("r0");
                u8 *resource;
                u32 frame;
                alive++;
                p->sprite.position.x += scale(p->velocity.x, *speed);
                p->sprite.position.y += p->velocity.y;
                p->sprite.position.z += scale(p->velocity.z, *speed);
                asm("" : : : "memory");
                p->velocity.y -= 2;
                Anim_SetDurationScale(anim, 16);
                resource = gFadingParticleManager->resource;
                a = anim;
                entry = p->anim.data + p->anim.frame;
                frame = *entry >> 6;
                p->sprite.tile = *(u16 *)(resource + 4) +
                                 (p->sprite.width >> 3) * (p->sprite.height >> 3) * frame;
                {
                    u32 input = a->flags;
                    u32 left = 1;
                    u32 mask = 1;
                    left &= input;
                    if (left != (((*entry & 0x30) >> 4) & mask))
                        p->sprite.flags |= 4;
                    else
                        p->sprite.flags &= ~4;
                }
                {
                    register u32 input asm("r0") = a->flags;
                    u32 mask = 2;
                    u32 left = (u8)(mask & input);
                    if (left != (((*entry & 0x30) >> 4) & mask))
                        p->sprite.flags |= 8;
                    else
                        p->sprite.flags &= ~8;
                }
                n = ++a->timer;
                if (n >= a->rate) {
                    a->timer = 0;
                    if (a->flags & 4) {
                        if (a->frame == 0) {
                            if (a->flags & 8) {
                                a->timer = a->rate;
                            } else {
                                a->frame = a->count - 1;
                            }
                        } else {
                            a->frame--;
                        }
                    } else {
                        if (++a->frame >= a->count) {
                            if ((u8)(a->flags & 8)) {
                                a->timer = a->rate;
                                a->frame = a->count - 1;
                            } else {
                                a->frame = 0;
                            }
                        }
                    }
                    entry = a->data + a->frame;
                    a->rate = ((int)frame_factor(*entry) * a->scale) >> 6;
                }
                {
                    register u32 raw asm("r0") = *life;
                    u32 compare;
                    u32 time;
                    compare = raw;
                    time = raw;
                    if (compare > 8) {
                        if (p->anim.frame == 5)
                            p->anim.frame = 0;
                    } else if (compare == 8)
                        p->anim.frame = 0;
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
