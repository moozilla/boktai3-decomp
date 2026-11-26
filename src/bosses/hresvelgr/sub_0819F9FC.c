#include "engine/shared/global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"
#include "engine/shared/scene_facing.h"
void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
void Hresvelgr_EmitFacingMotion(struct HresvelgrTravelActor *);
s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_UpdateFacingActionPhase(struct HresvelgrTravelActor *actor)
{
    struct BossActionState *action = &actor->action;
    if (actor->changed) {
        actor->changed = 0;
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
        actor->facing = actor->targetHeading;
        actor->facing4 = SceneFacing_FourWay(actor->facing);
        actor->facing8 = SceneFacing_EightWay(actor->facing);
    }
    Hresvelgr_SetFlags(actor, 0x600040);
    actor->flags |= 0x2000000;
    switch (action->state) {
        case 0:
            if (action->frame == 0) {
                Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 19, 1, 0);
                (action->frame)++;
                Sound_PlaySong(0x3E2);
                Sound_PlaySong(0x83);
            }
            if (actor->animationCompletions > 4)
                ActionState_AdvanceState(action);
            Hresvelgr_EmitFacingMotion(actor);
            break;
        case 1:
            actor->finished = 1;
            break;
    }
    Callback_InvokeActor(actor, actor->callback);
}
