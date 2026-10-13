#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Sprite {
    u32 flags;
    u8 pad4[4];
    u16 tile;
    u8 width, height;
    u8 rest[12];
    struct Vec position;
    u8 tail[8];
};
struct Anim {
    const u16 *data;
    u8 flags, count, factor, rate;
    u16 frame, unused, scale, timer;
};
struct Particle {
    u8 state, speed, angle;
    s8 drift;
    u16 timer, lifetime;
    struct Vec velocity, origin;
    struct Anim anim;
    struct Sprite sprite;
};
struct Effect {
    u8 active, count, type, kind, soundDelay, pad;
    u16 timer;
    struct Particle particles[4];
};
struct Graphics {
    u32 pad;
    u16 base;
};
struct Manager {
    u8 pad[24];
    u32 mode, counter;
    struct Graphics *graphics;
    void *animation;
    struct Effect effects[6];
    void *actor;
};
struct Context {
    struct Vec world, lower, upper, actorLower, actorUpper;
};
extern s16 gUnk_086149C4[];
extern u32 gUnk_02000560;
struct AnimId {
    u16 id, pad;
};
extern struct AnimId gUnk_08604A30[][3];
extern u32 gUnk_08604A60[];
extern void (*gUnk_08604A54[])(void *, u32);
s32 sub_082215E4(s32, s32);
void sub_08220C8C(void *, void *, u32, u32, u32);
void sub_0822B3BC(u32);
#define EffectParticle_UpdateHoming sub_0800BE24
void EffectParticle_UpdateHoming(struct Manager *manager, struct Effect *effect, s32 index,
                                 s32 phase, u32 phaseMask, struct Context *context)
{
    register struct Context *ctx asm("r10") = context;
    struct Particle *p = &effect->particles[index];
    struct Sprite *sprite = &p->sprite, *o;
    struct Anim *a;
    const u16 *entry;
    register u16 n asm("r0");
    s32 dx, dy, dz;
    if ((index & 1) == phase) {
        {
            register struct Context *c asm("r2") = ctx;
            dx = c->world.x - sprite->position.x;
        }
        {
            register struct Context *c asm("r3") = ctx;
            dy = c->world.y - sprite->position.y + 128;
            dz = c->world.z - sprite->position.z;
        }
        p->angle = sub_082215E4(dx, dz);
        p->velocity.x = (gUnk_086149C4[(p->angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[p->angle] * p->speed) >> 12;
        {
            u32 mask;
            register u32 m asm("r3") = 4;
            asm("" : : "r"(m));
            mask = m;
            if (!(gUnk_02000560 & m) && !effect->timer && (dy < 0 ? -dy : dy) <= 129 &&
                dx * dx + dz * dz < 400) {
                p->state = mask;
                sub_08220C8C(&p->anim, manager->animation,
                             gUnk_08604A30[manager->mode][effect->type].id, 0, 0);
                if (!(gUnk_02000560 & mask)) {
                    sub_0822B3BC(gUnk_08604A60[effect->type]);
                    gUnk_08604A54[effect->type](manager->actor, effect->kind);
                }
                return;
            }
        }
    }
    sprite->position.x = sprite->position.x + p->velocity.x;
    {
        s32 height;
        register struct Context *c asm("r2") = ctx;
        asm("" : : "r"(c));
        height = c->world.y - sprite->position.y;
        if (height > 16)
            sprite->position.y += 16;
        else if (height < -16)
            sprite->position.y -= 16;
        else {
            register struct Context *c asm("r3") = ctx;
            asm("" : : "r"(c));
            sprite->position.y = c->world.y;
        }
    }
    {
        u16 velocity = p->velocity.z;
        u16 position = sprite->position.z;
        asm("" : : "r"(velocity), "r"(position));
        sprite->position.z = velocity + position;
    }
    o = &p->sprite;
    {
        register struct Manager *m asm("r2") = manager;
        struct Graphics *graphics;
        asm("" : : "r"(m));
        graphics = m->graphics;
        a = &p->anim;
        entry = a->data + a->frame;
        {
            u32 factor = *entry >> 6;
            o->tile = (o->width >> 3) * (o->height >> 3) * factor + graphics->base;
        }
    }
    {
        u32 input = a->flags;
        register u32 mask asm("r3") = 1;
        register u32 left asm("r1") = (u8)(input & mask);
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
            o->flags |= 8;
        else
            o->flags &= ~8;
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
        a->rate = ((*entry & 15) * a->scale) >> 6;
    }
    p->timer--;
    if (!p->timer) {
        p->state = 3;
        p->lifetime = 15;
    }
}
