#include "scene_actor_control_helpers.h"
#include "movement_curve.h"

s32 SceneActorControl_AdvancePositionCurve(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorControlMotionState *ctx = SceneActorControl_GetMotionState(actor);
    struct ContactPosition start, end;
    start = ctx->vectors[0];
    end = ctx->vectors[1];
    {
        u32 mask = 0xFFFF;
        *(u32 *)&start = mask & *(u32 *)&start;
        *(u32 *)&end = mask & *(u32 *)&end;
    }
    MovementCurve_InterpolatePosition(&ctx->curvePosition, &start, &end, 1, ctx->field3E0, 0,
                                      ctx->sway);
    if (ctx->field3E0 > 1023) {
        ctx->field3E0 = 0;
        return 1;
    }
    ctx->field3E0 += (s16)MovementCurve_RandomByte() + 256;
    if (ctx->field3E0 > 1024)
        ctx->field3E0 = 1024;
    return 0;
}
