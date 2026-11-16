#include "global.h"
#include "axe.h"
#include "rotation_phase.h"

void Sound_PlaySong(u32);
s32 Mod(s32, s32);
void Axe_EmitMotionTrail(struct ChaseActor *actor);
void sub_080FC094(struct ChaseActor *actor, s32 count);
void SceneActorControl_FinishRotation(struct ChaseActor *actor, s32 elapsed);
static inline s32 FoldRotation(s32 a)
{
    if (a > 128)
        return a - 256;
    if (a < -128)
        return a + 256;
    return a;
}
void SceneActorControl_RecoverRotation(struct ChaseActor *actor, s32 elapsed)
{
    struct Axe *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    s32 angle;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        drawable->flags |= 2;
        state->acceleration = 4;
        Sound_PlaySong(0x1E5);
    }
    angle = NormalizeAngle(32 - state->savedAngle);
    if (angle < 0) {
        u32 step = state->acceleration;
        {
            u32 cur = drawable->angle;
            step = step + cur;
            drawable->angle = step;
        }
    } else
        drawable->angle -= state->acceleration;
    if (Mod(elapsed, 4) == 0)
        state->acceleration += 4;
    angle = NormalizeAngle(drawable->angle);
    Axe_EmitMotionTrail(actor);
    {
        s32 magnitude = angle;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude > 96) {
            RotationPhase_Change(actor, SceneActorControl_FinishRotation, 51);
            state->flagsE4C = RotationPhase_Mask(&state->flagsE4C, ~0x200);
            state->parameterEB2 = 32;
            sub_080FC094(actor, 8);
        }
    }
}
