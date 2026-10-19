#define SceneActorControl_SelectHoverPhase sub_080FB07C
#include "global.h"
#include "scene_actor_reset.h"
struct HoverControlContext {
    u8 unknown0[0xE48];
    s32 healthLimit;
    u8 unknownE4C[0x68];
    u16 healthThreshold;
    u8 unknownEB6[2];
    u16 counters[4];
};
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u32 phase, u32 behavior, u32 zero)
{
    actor->state2A8 = zero;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = zero;
    actor->field2B1 = 1;
}
s32 sub_080FB07C(struct SceneActorRegistryResetView *actor)
{
    struct HoverControlContext *context = actor->context;
    u32 zero;
    u32 phase, behavior;
    if (actor->health <= context->healthLimit) {
        zero = context->counters[3];
        if (!zero) {
            phase = 2;
            behavior = 8;
            goto select;
        }
    }
    if (!context->counters[0] && actor->distanceF0 <= 0xFFFE) {
        s32 direction = actor->fieldE9;
        s32 difference = direction - 160;
        s32 absolute;
        if (difference > 128)
            difference = direction - 416;
        else if (difference < -128)
            difference = direction + 96;
        absolute = difference;
        if (absolute < 0)
            absolute = -absolute;
        if (absolute <= 41) {
            SetPhase(actor, 2, 0, 0);
        } else {
            SetPhase(actor, 2, 2, 0);
        }
        return 1;
    }
    if (context->healthThreshold >= actor->health) {
        zero = context->counters[1];
        if (!zero) {
            phase = 2;
            behavior = 4;
            goto select;
        }
        zero = context->counters[2];
        if (!zero) {
            phase = 2;
            behavior = 6;
            goto select;
        }
    }
    return 0;
select:
    SetPhase(actor, phase, behavior, zero);
    return 1;
}
