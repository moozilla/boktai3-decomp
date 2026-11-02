#include "global.h"
#include "state_actor.h"
#include "player.h"

s32 Math_Atan2(s32 x, s32 z); // include/directional_interaction.h

static inline s32 near(struct StateActor *p, struct StateActorPos *a, struct StateActorPos *b)
{
    s32 limit;
    s32 x;
    s32 result;
    limit = p->overlayDrawable[0].horizontalRange;
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

s32 sub_081C8FB0(struct StateActorTargetContext *ctx, struct StateActor *p, s32 checkAngle)
{
    s32 facing;
    s32 result;
    if (ctx->target != 0)
        facing = (u8)ctx->targetPlayer->unk2D;
    else
        facing = 0;
    if (!near(p, &p->position, ctx->target))
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
