#include "global.h"
/* Golem_BeginSlideState: on entry hands control to the slide state (sub_080C725C, kind 0x29), faces the stored heading and sets the context push speed to 0x200; after 30 frames or once the control finishes, reports the pending context event and returns to the idle control (sub_080C68F0, kind 0x27) in phase 2/3 (provisional name) */
#define Golem_BeginSlideState sub_080CEB48
#include "scene_actor_reset.h"

struct Unk080CEB48Context {
    u8 unknown0[0x634];
    u16 pendingEvent;
    u8 unknown636[0x682 - 0x636];
    u16 pushSpeed;
};
struct Unk080CEB48Body {
    u8 unknown0[9];
    u8 direction;
};

void sub_080C725C(void);
void sub_080C68F0(void);

static inline u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return TRUE;
    }
    return FALSE;
}
static inline void BeginControl(struct SceneActorRegistryResetView *actor, void *control, u8 kind)
{
    u32 zero;
    actor->field2B2 = 1;
    zero = 0;
    actor->field2B0 = zero;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = zero;
    {
        u32 *flags = &actor->flags114;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->word104 = zero;
}
static inline void SetPhase(struct SceneActorRegistryResetView *actor, u32 state, u32 phase,
                            u32 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void sub_080CEB48(struct SceneActorRegistryResetView *actor)
{
    struct Unk080CEB48Context *context = actor->context;
    struct Unk080CEB48Body *body = (struct Unk080CEB48Body *)((u8 *)actor + 0x50);
    if (TakeFlag(&actor->field2B1)) {
        BeginControl(actor, sub_080C725C, 0x29);
        actor->field2B0 = 0;
        body->direction = actor->fieldE9;
        context->pushSpeed = 0x200;
    }
    if ((s32)actor->counter2AC > 0x1E || actor->field2B0) {
        if (context->pendingEvent) {
            actor->field304 = context->pendingEvent;
            Callback_InvokeActor(actor, actor->update26C);
        }
        BeginControl(actor, sub_080C68F0, 0x27);
        SetPhase(actor, 0, 2, 3);
    } else {
        actor->counter2AC++;
    }
}
