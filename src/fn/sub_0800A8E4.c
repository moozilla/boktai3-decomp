#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Vec {
    s16 x, y, z, pad;
};
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Actor {
    u8 pad0[48];
    struct Vec position;
    u8 pad38[0x36c];
    u8 facing;
};
struct Effect {
    u8 active, unused, type, pad3, soundDelay, pad5;
    u16 timer;
    u8 particles[320];
};
struct Manager {
    u8 task[28];
    u32 counter;
    u8 pad20[8];
    struct Effect effects[6];
    struct Actor *actor;
};
struct Context {
    struct Vec world, lower, upper, actorLower, actorUpper;
};
extern u32 gUnk_030053F4, gUnk_030053F0;
extern u32 gUnk_08604A6C[];
void EffectManager_UpdateParticles(struct Manager *, struct Effect *, struct Context *);
void sub_0822B3BC(u32);
static inline s32 scale(s32 input)
{
    register s32 value asm("r0") = input * 3;
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
    s32 x, z, h, y;
    asm("" : : "r"(out), "r"(in));
    x = in->x;
    z = in->z;
    out->x = scale(x - z);
    h = scale(x + z);
    y = scaleHeight(in->y);
    out->y = h - y;
    out->z = h + y;
}
static inline void projectActor(struct Vec *out, struct Vec *in)
{
    register s32 x asm("r3");
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
static inline s32 continuing(s32 i)
{
    register s32 v asm("r0") = i;
    asm("" : : "r"(v));
    return v >= 0;
}
s32 EffectManager_UpdateAll(struct Manager *manager)
{
    struct Context context;
    struct Context *ctx;
    struct PackedVec actorWorld;
    struct Vec actorScreen;
    struct Effect *p, *view;
    s32 i;
    u32 mask = 512;
    u32 flags;
    flags = gUnk_030053F4 | gUnk_030053F0;
    mask &= flags;
    if (mask) {
        flags &= 256;
        if (!flags)
            goto done;
    }
    actorWorld.x = manager->actor->position.x;
    actorWorld.y = manager->actor->position.y + 128;
    actorWorld.z = manager->actor->position.z;
    projectActor(&actorScreen, (struct Vec *)&actorWorld);
    ctx = &context;
    p = manager->effects;
    view = p;
    i = 5;
    do {
        if (view->active) {
            if (view->type == 2) {
                ctx->world.x = ((struct Vec *)&actorWorld)->x;
                ctx->world.y = ((struct Vec *)&actorWorld)->y;
                ctx->world.z = ((struct Vec *)&actorWorld)->z;
            } else if (manager->actor->facing == 0) {
                ctx->world.x = manager->actor->position.x + 24;
                ctx->world.y = manager->actor->position.y + 350;
                ctx->world.z = manager->actor->position.z - 24;
            } else {
                ctx->world.x = manager->actor->position.x - 32;
                ctx->world.y = manager->actor->position.y + 320;
                ctx->world.z = manager->actor->position.z + 32;
            }
            project(&context.lower, &context.world);
            context.upper = context.lower;
            ctx->lower.x -= 5;
            ctx->lower.y -= 5;
            ctx->upper.x += 5;
            ctx->upper.y += 5;
            context.actorLower = actorScreen;
            context.actorUpper = actorScreen;
            ctx->actorLower.x -= 8;
            ctx->actorLower.y -= 20;
            ctx->actorUpper.x += 8;
            ctx->actorUpper.y += 20;
            EffectManager_UpdateParticles(manager, p, &context);
            if (view->soundDelay) {
                view->soundDelay--;
                if (!view->soundDelay) {
                    u32 type = view->type;
                    sub_0822B3BC(gUnk_08604A6C[type]);
                }
            }
            if (view->timer)
                view->timer--;
        }
        {
            register u32 stride asm("r1") = sizeof(*p);
            register s32 step asm("r2");
            view = (struct Effect *)((u8 *)view + stride);
            p = (struct Effect *)((u8 *)p + stride);
            step = -1;
            asm("" : : "r"(step));
            i += step;
        }
    } while (continuing(i));
    {
        struct Manager *end = manager;
        end->counter++;
    }
done:
    return 0;
}
