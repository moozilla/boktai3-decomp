#include "global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_UpdateRecoveryPhase(struct HresvelgrTravelActor *actor)
{
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 16, 1, 0);
        Anim_SetFrameDurationScale(&actor->animation, 160);
        actor->phase = 0;
        actor->phaseFrame = 0;
    }
    Hresvelgr_SetFlags(actor, 0x200040);
    if ((u8)Callback_InvokeActor(actor, actor->callback) == 0) {
        switch (actor->phase) {
            case 0:
                if (actor->animationCompletions > 3) {
                    actor->phase = 1;
                    actor->phaseFrame = 0;
                    Anim_SetFrameDurationScale(&actor->animation, 64);
                }
                break;
            case 1:
                actor->finished = 1;
                break;
        }
    }
}
