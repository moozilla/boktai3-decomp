#include "global.h"
#include "rotation_phase.h"

// MATCHING_CONCESSION: register-pinning
void SceneActorControl_RecoverRotation(void);
static inline s32 FoldAngle(s32 a)
{
    if (a > 128)
        return a - 256;
    if (a < -128)
        return a + 256;
    return a;
}
static inline s32 Magnitude(s32 a)
{
    s32 result = a;
    if (result < 0)
        result = -result;
    return result;
}
void SceneActorControl_TiltRotation(struct SceneActorRegistryResetView *actor, s32 elapsed)
{
    u8 *state = actor->context;
    struct RotatingDrawable *drawable = actor->drawable;
    s32 angle, magnitude;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
        u8 saved;
        drawable->flags |= 2;
        saved = actor->fieldE9;
        state[0xED3] = saved;
    }
    angle = FoldAngle(drawable->angle);
    magnitude = angle;
    asm("" : "+r"(magnitude));
    if (magnitude < 0)
        magnitude = -magnitude;
    if (magnitude > 48) {
        if (elapsed > 42)
            RotationPhase_Change(actor, SceneActorControl_RecoverRotation, 50);
    } else {
        angle = FoldAngle(32 - state[0xED3]);
        if (angle < 0)
            drawable->angle -= 2;
        else
            drawable->angle += 2;
    }
}
