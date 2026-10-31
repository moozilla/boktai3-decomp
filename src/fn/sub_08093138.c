#include "global.h"
#include "contact_actor.h"

#include "scene_actor_reset.h"
#include "engine.h"
#include "ram.h"

struct BeeCurveContext {
    u8 unknown0[0x430];
    u16 time;
    u8 unknown432[2];
    s16 curve;
    u8 unknown436[0x450 - 0x436];
    struct ContactPosition output;
    u8 unknown458[0x470 - 0x458];
    struct ContactPosition start, end;
};
static inline s32 Bee_TruncateFixed10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}
static inline void Bee_SubtractPositions(struct ContactPosition *out, struct ContactPosition *a,
                                         struct ContactPosition *b)
{
    out->x = b->x - a->x;
    out->y = b->y - a->y;
    out->z = b->z - a->z;
}
static inline void Bee_InterpolatePosition(struct ContactPosition *out,
                                           struct ContactPosition *start,
                                           struct ContactPosition *end, s32 initialScale, s32 t,
                                           s32 finalScale, s32 speed)
{
    struct ContactPosition delta, tangent;
    s32 square, cube, twiceCube, thriceSquare, sway, final;
    s32 scaled, value;
    Bee_SubtractPositions(&delta, start, end);
    ZeroFill8(&tangent);
    square = Bee_TruncateFixed10(t * t);
    cube = Bee_TruncateFixed10(square * t);
    twiceCube = cube * 2;
    thriceSquare = square * 3;
    sway = (cube - 2 * square + t) * speed;
    final = (cube - square) * finalScale;
    scaled = ((twiceCube - thriceSquare + 1) * tangent.x) * initialScale;
    value = (thriceSquare - twiceCube) * delta.x;
    value = scaled + value + sway + final;
    out->x = start->x + Bee_TruncateFixed10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.y) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.y;
    value = scaled + value + sway + final;
    out->y = start->y + Bee_TruncateFixed10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.z) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.z;
    value = scaled + value + sway + final;
    out->z = start->z + Bee_TruncateFixed10(value);
}
static inline u32 Bee_RandomByte(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    index *= 2;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index) & 255;
}

u8 Bee_AdvanceMovementCurve(struct SceneActorRegistryResetView *actor)
{
    struct BeeCurveContext *ctx = actor->context;
    struct ContactPosition start, end;
    start = ctx->start;
    end = ctx->end;
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
        *(u32 *)&end = mask & *(u32 *)&end;
    }
    Bee_InterpolatePosition(&ctx->output, &start, &end, 1, ctx->time, 0, ctx->curve);
    if (ctx->time > 1023) {
        ctx->time = 0;
        return 1;
    }
    ctx->time += (s16)Bee_RandomByte() + 256;
    if (ctx->time > 1024)
        ctx->time = 1024;
    return 0;
}
