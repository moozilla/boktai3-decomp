#include "global.h"

#ifdef NONMATCHING
// Remaining: Control and state-store constants have different allocation and derivation.
// fuzzy: 82.6%
#include "scene_actor_reaction.h"
#include "scene_actor_control_phase.h"

s32 Mod(s32, s32);
void SceneActorControl_ResetCollisionFlags(struct SceneActorRegistryResetView *);
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *callback, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = callback;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u8 state, u8 phase,
                            u8 behavior)
{
    actor->state2A8 = state;
    actor->phase2A9 = phase;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}

void Lun_UpdateWaitingAnimation(void);
void Lun_BeginMoveToPosition(void *, void *);
extern u8 *gUnk_02000580;
// confidence=low: restores pursuit after the lift response, or returns to waiting when pursuit expires.
#define Lun_ResumeAfterLift sub_0809B7FC
void Lun_ResumeAfterLift(struct SceneActorRegistryResetView *actor, s32 frame)
{
    u32 zero = 0;
    if (SceneActorControl_TakeFlag(&actor->field2B2))
        SceneActorControl_ConsumeLiftFlagPenalty(actor);
    actor->flags2D6 = 0x10 | actor->flags2D6;
    actor->flags2D0 |= 0x100000;
    if (frame > 30) {
        u8 *context = actor->context;
        u16 *remaining;
        u32 value;
        SceneActorControl_ResetCollisionFlags(actor);
        remaining = (u16 *)(context + 0x444);
        value = *remaining;
        if (value) {
            actor->flags2D4 = 0x100 | actor->flags2D4;
            *remaining = *(u16 *)(context + 0x442);
            Lun_BeginMoveToPosition(actor, gUnk_02000580 + 0x30);
            SetState(actor, 0, 2, 1);
        } else {
            SetControl(actor, Lun_UpdateWaitingAnimation, 10);
            SetState(actor, 0, 1, 1);
            actor->direction = actor->fieldE9;
            actor->field2C2 = 0x40;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0809B7FC);
#endif
