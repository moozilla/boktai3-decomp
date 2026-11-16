#include "global.h"
#include "golem.h"
/* Golem_BeginSlideState: on entry hands control to the slide state (Golem_UpdateSlideImpact, kind 0x29), faces the stored heading and sets the context push speed to 0x200; after 30 frames or once the control finishes, reports the pending context event and returns to the idle control (Golem_UpdateChargeStop, kind 0x27) in phase 2/3 (provisional name) */
#include "scene_actor_reset.h"

struct Unk080CEB48Body {
    u8 unknown0[9];
    u8 direction;
};

void Golem_UpdateSlideImpact(void);
void Golem_UpdateChargeStop(void);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
static inline void BeginControl(struct ChaseActor *actor, void *control, u8 kind)
{
    u32 zero;
    actor->base.field2B2 = 1;
    zero = 0;
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}
static inline void SetPhase(struct ChaseActor *actor, u32 state, u32 phase, u32 behavior)
{
    actor->base.state2A8 = state;
    actor->base.phase2A9 = phase;
    actor->base.behavior2AA = behavior;
    actor->base.counter2AC = 0;
    actor->base.field2B1 = 1;
}

void Golem_BeginSlideState(struct ChaseActor *actor)
{
    struct Golem *context = actor->base.context;
    struct Unk080CEB48Body *body = (struct Unk080CEB48Body *)((u8 *)actor + 0x50);
    if (TakeFlag(&actor->base.field2B1)) {
        BeginControl(actor, Golem_UpdateSlideImpact, 0x29);
        actor->base.stepDone = 0;
        body->direction = actor->base.tracking.heading;
        context->pushSpeed = 0x200;
    }
    if ((s32)actor->base.counter2AC > 0x1E || actor->base.stepDone) {
        if (context->pendingEvent) {
            actor->base.counter304 = context->pendingEvent;
            Callback_InvokeActor(actor, actor->base.targetQuery);
        }
        BeginControl(actor, Golem_UpdateChargeStop, 0x27);
        SetPhase(actor, 0, 2, 3);
    } else {
        actor->base.counter2AC++;
    }
}
