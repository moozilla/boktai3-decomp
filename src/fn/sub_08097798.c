#include "global.h"
#include "contact_actor.h"
#include "movement_curve.h"

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
    MovementCurve_InterpolatePosition(&ctx->output, &start, &end, 1, ctx->time, 0, ctx->curve);
    if (ctx->time > 1023)
        return 1;
    ctx->time += 128;
    return 0;
}
