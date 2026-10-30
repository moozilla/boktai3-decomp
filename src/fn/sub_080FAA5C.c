#include "global.h"
#include "contact_actor.h"
#include "scene_actor_state.h"

struct AxeCurveContext {
    u8 unknown0[0xEA8];
    u16 time;
    u8 unknownEAA[2];
    s16 curve;
    u8 unknownEAE[0xF04 - 0xEAE];
    struct ContactPosition start, end;
};
struct AxeMoveNode {
    u8 unknown0[12];
    struct ContactPosition position;
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
u8 sub_080FAA5C(struct SceneActorRegistryResetView *actor)
{
    struct AxeCurveContext *ctx = actor->context;
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
    Interpolate(&((struct AxeMoveNode *)actor->group.route)->position, &start, &end, 1, ctx->time,
                0, ctx->curve);
    if (ctx->time > 1023)
        return 1;
    ctx->time += 128;
    return 0;
}
