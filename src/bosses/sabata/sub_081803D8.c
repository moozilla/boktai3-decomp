#include "bosses/sabata/sabata_actor_state.h"
#include "bosses/state_actor_action_entry.h"

void *SceneActors_GetBySlot(u32 slot);

static inline void BossTask_SetStep(struct SabataActorState *p, u32 step)
{
    p->requestMode = step;
}

/* Registry routine 4EC9: cycles the singleton in slot 2 through steps 0-2
   (+0x4EA), entering state 8, 10 or 3, then clears +0xB3, +0x4EB, +0x4E8. */
void Sabata_AdvanceStepFromScript(void)
{
    struct StateActor *actor = SceneActors_GetBySlot(2);
    struct SabataActorState *p = (struct SabataActorState *)actor;
    if (p) {
        if (p->requestMode == 0) {
            BossTask_SetStep(p, 1);
            SceneActorState_StartAction(actor, 0, 0, 8);
        } else if (p->requestMode == 1) {
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
        p->requestPhase = 0;
        p->request = 0;
    }
}
