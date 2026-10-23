#include "global.h"
#include "rotation_phase.h"

void Sound_PlaySong(u32);
s32 Mod(s32, s32);
void Axe_EmitMotionTrail(struct SceneActorRegistryResetView *);
void sub_080FC094(struct SceneActorRegistryResetView *, s32);
void SceneActorControl_FinishRotation(void);
struct RotationControlState {
    u8 unknown0[0xE4C];
    u32 flags;
    u8 unknownE50[0x5E];
    u8 acceleration;
    u8 unknownEAF[3];
    u8 parameter;
    u8 unknownEB3[0x20];
    u8 savedAngle;
};
static inline s32 FoldRotation(s32 a)
{
    if (a > 128)
        return a - 256;
    if (a < -128)
        return a + 256;
    return a;
}
void SceneActorControl_RecoverRotation(struct SceneActorRegistryResetView *actor, s32 elapsed)
{
    struct RotationControlState *state = actor->context;
    struct RotatingDrawable *drawable = actor->drawable;
    s32 angle;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
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
            state->flags = RotationPhase_Mask(&state->flags, ~0x200);
            state->parameter = 32;
            sub_080FC094(actor, 8);
        }
    }
}
