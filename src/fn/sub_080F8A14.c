#include "global.h"
#include "scene_actor_control_phase.h"

struct RotatingDrawable {
    u32 flags;
    u8 unknown4[2];
    u8 angle;
};
void sub_080FBA38(struct SceneActorAnimationView *);
void Sound_PlaySong(u32);

static inline s32 NormalizeAngle(s32 angle)
{
    s32 result = angle;
    if (angle > 128)
        result -= 256;
    else if (angle < -128)
        result += 256;
    return result;
}

void SceneActorControl_SettleRotation(struct SceneActorAnimationView *actor, s32 elapsed)
{
    u8 *state = (u8 *)actor->link;
    struct RotatingDrawable *drawable = actor->drawable;
    s32 angle;
    u32 previous;
    if (SceneActorControl_TakeFlag(&actor->directionalEntryFlag)) {
        drawable->flags |= 2;
        state[0xA36] = 12;
        state[0xEAF] = 30;
    }
    angle = NormalizeAngle(drawable->angle);
    sub_080FBA38(actor);
    previous = drawable->angle;
    drawable->angle = previous - state[0xA36];
    if ((s32)previous < ((struct RotatingDrawable *)actor->drawable)->angle)
        Sound_PlaySong(0x169);
    if (elapsed > 29) {
        s32 magnitude = angle;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude <= 12) {
            drawable->angle = 0;
            actor->unknown2B0 = 1;
        }
    }
}
