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
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
void sub_0800A80C(struct Manager *, struct Effect *);
static inline u32 randomValue(s32 reverse)
{
    u32 offset;
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    offset = gUnk_03005308 * 2;
    if (reverse) {
        register u16 *table asm("r2") = gUnk_0203B400;
        return *(u16 *)(offset + (u32)table);
    }
    return *(u16 *)(offset + (u32)gUnk_0203B400);
}
static inline s32 scale(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        value >>= 5;
    else
        value = -((-value) >> 5);
    return value;
}
static inline s32 scaleHeight(s32 input)
{
    s32 value = input * 3;
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline void project(struct Vec *out, struct Vec *in)
{
    register s32 x asm("r4");
    s32 z, h, y;
    x = in->x;
    z = in->z;
    out->x = scale(x - z);
    {
        register s32 sum asm("r1") = x + z;
        asm("" : : "r"(sum));
        h = scale(sum);
    }
    y = scaleHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
#define EffectParticle_UpdateDriftBeforeHoming sub_0800B788
void EffectParticle_UpdateDriftBeforeHoming(struct Manager *manager, struct Effect *effect,
                                            s32 index, s32 phase, u32 phaseMask,
                                            struct Context *ctx)
{
    struct Vec projected;
    struct Particle *p = &effect->particles[index];
    struct Vec *origin = &p->origin;
    struct Sprite *sprite = &p->sprite, *o;
    register u8 *angle asm("r6") = &p->angle;
    struct Anim *a;
    const u16 *entry;
    register u16 n asm("r0");
    s32 difference;
    if ((index & 1) == phase) {
        difference =
            sub_082215E4(origin->x - sprite->position.x, origin->z - sprite->position.z) - p->angle;
        if (p->drift < 0) {
            if ((difference < 0 ? -difference : difference) <= 4)
                *angle = p->angle + difference;
            else
                *angle = p->angle + 4;
            p->drift++;
            if (!p->drift)
                p->drift = (randomValue(0) & 63) + 64;
        } else {
            if ((difference < 0 ? -difference : difference) <= 4)
                *angle = p->angle - difference;
            else
                *angle = p->angle - 4;
            p->drift--;
            if (!p->drift)
                p->drift = -(randomValue(1) & 63) - 64;
        }
        p->velocity.x = (gUnk_086149C4[(*angle + 64) & 255] * p->speed) >> 12;
        p->velocity.z = (gUnk_086149C4[*angle] * p->speed) >> 12;
    } else if (!effect->timer) {
        project(&projected, &p->sprite.position);
        {
            u32 mask;
            register u32 m asm("r1") = 4;
            u32 paused;
            mask = m;
            paused = gUnk_02000560 & m;
            if (!paused && ctx->lower.x <= projected.x && ctx->upper.x >= projected.x &&
                ctx->lower.y <= projected.y && ctx->upper.y >= projected.y) {
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
        s32 height = ctx->world.y - sprite->position.y;
        if (height > 16)
            sprite->position.y += 16;
        else if (height < -16)
            sprite->position.y -= 16;
        else
            sprite->position.y = ctx->world.y;
    }
    sprite->position.z = sprite->position.z + p->velocity.z;
    o = &p->sprite;
    {
        struct Graphics *graphics = manager->graphics;
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
        register u32 left asm("r1") = (u8)(mask & input);
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
    if (!p->lifetime) {
        p->state = 2;
        p->speed = 15;
        p->lifetime = 10;
        p->timer = (randomValue(0) & 31) + 60;
    } else
        p->lifetime--;
}
