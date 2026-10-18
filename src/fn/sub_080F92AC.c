#define SceneActorControl_ResetRotation        sub_080F92AC
#define SceneActorControl_SetFourEntryPayloads Axe_SetAuxiliarySpriteRenderState
#include "global.h"
#include "rotation_phase.h"
s32 Div(s32, s32);
void Axe_SetAuxiliarySpriteRenderState(struct SceneActorRegistryResetView *, void *, u32);
void sub_080F92AC(struct SceneActorRegistryResetView *actor)
{
    u8 *state = actor->context;
    struct RotatingDrawable *drawable = actor->drawable;
    s32 angle;
    u8 *timer;
    if (SceneActorControl_TakeFlag(&actor->field2B2)) {
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
        actor->field2B0 = 1;
    }
}
