#include "global.h"
#include "contact_actor.h"

struct LunCurveContext {
    u8 pad[0x3E0];
    u16 time;
    u8 pad3E2[2];
    s16 curve;
    u8 pad3E6[0xE];
    struct ContactPosition output;
    u8 pad3FC[0x18];
    struct ContactPosition start, end;
};
struct LunCurveActor {
    u8 pad[0x3D0];
    struct LunCurveContext *context;
};
static inline s32 Shift10(s32 value)
{
    return value >= 0 ? value >> 10 : -((-value) >> 10);
}
static inline void Subtract(struct ContactPosition *out, struct ContactPosition *a,
                            struct ContactPosition *b)
{
    out->x = b->x - a->x;
    out->y = b->y - a->y;
    out->z = b->z - a->z;
}
static inline void Interpolate(struct ContactPosition *out, struct ContactPosition *start,
                               struct ContactPosition *end, s32 initialScale, s32 t, s32 finalScale,
                               s32 speed)
{
    struct ContactPosition delta, tangent;
    s32 square, cube, twiceCube, thriceSquare, sway, final;
    s32 scaled, value;
    Subtract(&delta, start, end);
    ZeroFill8(&tangent);
    square = Shift10(t * t);
    cube = Shift10(square * t);
    twiceCube = cube * 2;
    thriceSquare = square * 3;
    sway = (cube - 2 * square + t) * speed;
    final = (cube - square) * finalScale;
    scaled = ((twiceCube - thriceSquare + 1) * tangent.x) * initialScale;
    value = (thriceSquare - twiceCube) * delta.x;
    value = scaled + value + sway + final;
    out->x = start->x + Shift10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.y) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.y;
    value = scaled + value + sway + final;
    out->y = start->y + Shift10(value);
    scaled = ((twiceCube - thriceSquare + 1) * tangent.z) * initialScale;
    value = (-twiceCube + thriceSquare) * delta.z;
    value = scaled + value + sway + final;
    out->z = start->z + Shift10(value);
}
u8 sub_0809CCF4(struct LunCurveActor *actor)
{
    struct LunCurveContext *ctx = actor->context;
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
    Interpolate(&ctx->output, &start, &end, 1, ctx->time, 0, ctx->curve);
    if (ctx->time > 1023)
        return 1;
    ctx->time += 128;
    return 0;
}
