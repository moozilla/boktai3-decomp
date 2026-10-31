#include "global.h"
#include "contact_actor.h"

#include "scene_actor_reset.h"

struct GhostCurveContext {
    u8 unknown0[0x5B4];
    u16 time;
    u8 unknown5B6[2];
    s16 curve;
    u8 unknown5BA[0x5D4 - 0x5BA];
    struct ContactPosition output;
    u8 unknown5DC[0x5EC - 0x5DC];
    struct ContactPosition start, end;
};
static inline s32 Ghost_TruncateFixed10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}
static inline void Ghost_SubtractPositions(struct ContactPosition *out, struct ContactPosition *a,
                                           struct ContactPosition *b)
{
    out->x = b->x - a->x;
    out->y = b->y - a->y;
    out->z = b->z - a->z;
}
static inline void Ghost_InterpolatePosition(struct ContactPosition *out,
                                             struct ContactPosition *start,
                                             struct ContactPosition *end, s32 initialScale, s32 t,
                                             s32 finalScale, s32 speed)
{
    struct ContactPosition delta, tangent;
    s32 square, cube, twiceCube, thriceSquare, sway, final;
    s32 scaled, value;
    Ghost_SubtractPositions(&delta, start, end);
    ZeroFill8(&tangent);
    square = Ghost_TruncateFixed10(t * t);
    cube = Ghost_TruncateFixed10(square * t);
    twiceCube = cube * 2;
    thriceSquare = square * 3;
    sway = (cube - 2 * square + t) * speed;
    final = (cube - square) * finalScale;
    scaled = ((twiceCube - thriceSquare + 1) * tangent.x) * initialScale;
    value = (thriceSquare - twiceCube) * delta.x;
    value = scaled + value + sway + final;
    out->x = start->x + Ghost_TruncateFixed10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.y) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.y;
    value = scaled + value + sway + final;
    out->y = start->y + Ghost_TruncateFixed10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.z) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.z;
    value = scaled + value + sway + final;
    out->z = start->z + Ghost_TruncateFixed10(value);
}
u8 sub_08097798(struct SceneActorRegistryResetView *actor)
{
    struct GhostCurveContext *ctx = actor->context;
    struct ContactPosition start, end;
    start = ctx->start;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
    }
    end = ctx->end;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&end = mask & *(u32 *)&end;
    }
    Ghost_InterpolatePosition(&ctx->output, &start, &end, 1, ctx->time, 0, ctx->curve);
    if (ctx->time > 1023)
        return 1;
    ctx->time += 128;
    return 0;
}
