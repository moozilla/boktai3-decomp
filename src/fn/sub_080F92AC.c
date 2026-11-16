#define SceneActorControl_SetFourEntryPayloads Axe_SetAuxiliarySpriteRenderState
#include "global.h"
#include "rotation_phase.h"
s32 Div(s32, s32);
void Axe_SetAuxiliarySpriteRenderState(struct S080FB958 *s, u32 x, u32 y);
void SceneActorControl_ResetRotation(struct ChaseActor *actor)
{
    u8 *state = actor->base.context;
    struct RotatingDrawable *drawable = actor->base.drawable;
    s32 angle;
    u8 *timer;
    if (SceneActorControl_TakeFlag(&actor->base.field2B2)) {
        drawable->flags |= 2;
        state[0xEAF] = 30;
    }
    angle = NormalizeAngle(drawable->angle);
    if (angle > 0) {
        s32 step;
        timer = &state[0xEAF];
        step = Div(angle, *timer);
        drawable->angle -= step;
    } else {
        s32 magnitude = angle;
        s32 step;
        if (magnitude < 0)
            magnitude = -magnitude;
        timer = &state[0xEAF];
        step = Div(magnitude, *timer);
        drawable->angle += step;
    }
    --*timer;
    if (state[0xEAF] == 0) {
        Axe_SetAuxiliarySpriteRenderState(actor, state + 0x888, 0x13C);
        actor->base.stepDone = 1;
    }
}
