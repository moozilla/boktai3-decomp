
#ifdef NONMATCHING
// fuzzy: 98.5%
// foundry draft (worker best 98.5%); what differs is mostly register allocation and instruction order
#define SceneActorControl_TiltRotation    sub_080F8B38
#define SceneActorControl_RecoverRotation sub_080F8C40
#include "global.h"
#include "rotation_phase.h"
void sub_080F8C40(void);
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
void sub_080F8B38(struct SceneActorRegistryResetView *actor, s32 elapsed)
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
    magnitude = __builtin_abs(angle);
    if (magnitude > 48) {
        if (elapsed > 42)
            RotationPhase_Change(actor, sub_080F8C40, 50);
    } else {
        angle = FoldAngle(32 - state[0xED3]);
        if (angle < 0)
            drawable->angle -= 2;
        else
            drawable->angle += 2;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080F8B38);
#endif
