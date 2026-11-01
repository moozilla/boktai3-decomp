#include "global.h"
#include "contact_actor.h"
#include "movement_curve.h"

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
    MovementCurve_InterpolatePosition(&ctx->output, &start, &end, 1, ctx->time, 0, ctx->curve);
    if (ctx->time > 1023)
        return 1;
    ctx->time += 128;
    return 0;
}
