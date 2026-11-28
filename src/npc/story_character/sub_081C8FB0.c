#include "global.h"
#include "npc/story_character/state_actor.h"
#include "player/player.h"

s32 Math_Atan2(s32 x, s32 z); // include/objects/directional_controller.h

static inline s32 near(struct StateActor *p, struct StateActorPos *a, struct StateActorPos *b)
{
    s32 limit = p->overlayDrawable[0].horizontalRange;
    s32 x, y, z;
    x = b->x - a->x;
    if (x < 0)
        x = -x;
    if (x < limit) {
        y = b->y - a->y;
        if (y < 0)
            y = -y;
        if (y <= 255) {
            z = b->z - a->z;
            if (z < 0)
                z = -z;
            if (z < limit)
                return 1;
        }
    }
    return 0;
}

s32 sub_081C8FB0(struct StateActorTargetContext *ctx, struct StateActor *p, s32 checkAngle)
{
    s32 facing;
    if (ctx->target != 0)
        facing = (u8)ctx->targetPlayer->unk2D;
    else
        facing = 0;
    if (near(p, &p->position, ctx->target)) {
        if (checkAngle != 0) {
            struct StateActorPos *a = &p->position;
            struct StateActorPos *target = ctx->target;
            s32 dx = a->x - target->x;
            s32 dz = a->z - target->z;
            u16 diff1;
            u8 diff2;
            s32 d1, d2, behind;
            diff1 = facing - ((s8)Math_Atan2(dx, dz));
            behind = p->angle + 0x80;
            diff2 = facing - behind;
            d1 = (s8)diff1;
            if (d1 < 0)
                d1 = -d1;
            if (d1 > 0x3f)
                return 0;
            d2 = (s8)diff2;
            if (d2 < 0)
                d2 = -d2;
            if (d2 > 0x1f)
                return 0;
        }
        return 1;
    }
    return 0;
}
