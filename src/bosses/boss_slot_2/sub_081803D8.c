#include "engine/shared/global.h"
#include "engine/shared/state_actor_action_entry.h"

struct BossTask {
    struct StateActor base;
    u8 unk348[0x4E8 - 0x348];
    u8 field4E8;
    u8 unk4E9;
    u8 step, field4EB;
};
void *SceneActors_GetBySlot(u32 slot);

static inline void BossTask_SetStep(struct BossTask *p, u32 step)
{
    p->step = step;
}

/* Registry routine 4EC9: cycles the singleton in slot 2 through steps 0-2
   (+0x4EA), entering state 8, 10 or 3, then clears +0xB3, +0x4EB, +0x4E8. */
void BossSlot2_AdvanceStepFromScript(void)
{
    struct StateActor *actor = SceneActors_GetBySlot(2);
    struct BossTask *p = (struct BossTask *)actor;
    if (p) {
        if (p->step == 0) {
            BossTask_SetStep(p, 1);
            SceneActorState_StartAction(actor, 0, 0, 8);
        } else if (p->step == 1) {
            BossTask_SetStep(p, 2);
            SceneActorState_StartAction(actor, 0, 0, 10);
        } else {
            BossTask_SetStep(p, 0);
            SceneActorState_StartAction(actor, 0, 0, 3);
        }
        {
            u8 cleared = 0;
            actor->locked = cleared;
        }
        p->field4EB = 0;
        p->field4E8 = 0;
    }
}
