#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "state_actor.h"

// fuzzy: 97.4%
// fix.py best version (reprinted C)
// foundry draft (worker best 82.4%); what differs is mostly register allocation and instruction order
s32 Math_Atan2(s32 x, s32 z); // include/directional_interaction.h

struct Context {
    u8 pad[0x434];
    struct StateActorPos *target;
    struct StateActor *targetActor;
};

static inline s32 near(struct StateActor *p, struct StateActorPos *b)
{
    struct StateActorPos *a = &p->position;
    s32 limit;
    s32 x;
    s32 result;
    limit = *(s32 *)((u8 *)p + 0x228);
    x = b->x - a->x;
    if (x < 0)
        x = -x;
    if (x >= limit)
        goto fail;
    {
        s32 y = b->y - a->y;
        if (y < 0)
            y = -y;
        if (y > 255)
            goto fail;
    }
    {
        s32 z = b->z - a->z;
        if (z < 0)
            z = -z;
        result = 1;
        if (z < limit)
            goto done;
    }
fail:
    result = 0;
done:
    return result;
}

s32 sub_081C8FB0(struct Context *ctx, struct StateActor *p, s32 checkAngle)
{
    s32 facing;
    s32 result;
    if (ctx->target != 0)
        facing = ((u8 *)ctx->targetActor)[0x2d];
    else
        facing = 0;
    if (!near(p, ctx->target))
        goto fail;
    if (checkAngle == 0)
        goto success;
    {
        struct StateActorPos *a = &p->position;
        struct StateActorPos *target = ctx->target;
        s32 dx = a->x - target->x;
        s32 dz = a->z - target->z;
        u16 diff1;
        u8 diff2u;
        s32 d1;
        s32 d2;
        diff1 = facing - ((s8)Math_Atan2(dx, dz));
        {
            s32 angle = p->angle + 0x80;
            asm("" : "+r"(angle));
            diff2u = facing - angle;
        }
        d1 = (s8)diff1;
        if (d1 < 0)
            d1 = -d1;
        if (d1 > 0x3f)
            goto fail;
        d2 = (s8)diff2u;
        if (d2 < 0)
            d2 = -d2;
        if (d2 > 0x1f)
            goto fail;
    }
success:
    result = 1;

    goto done;
fail:
    result = 0;

done:
    return result;
}
