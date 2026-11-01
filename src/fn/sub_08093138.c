#include "global.h"
#include "contact_actor.h"
#include "movement_curve.h"

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
    MovementCurve_InterpolatePosition(&ctx->output, &start, &end, 1, ctx->time, 0, ctx->curve);
    if (ctx->time > 1023) {
        ctx->time = 0;
        return 1;
    }
    ctx->time += (s16)MovementCurve_RandomByte() + 256;
    if (ctx->time > 1024)
        ctx->time = 1024;
    return 0;
}
