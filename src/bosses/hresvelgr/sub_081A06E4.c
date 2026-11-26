#include "engine/shared/global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void SceneMover_SetSideTarget(void *);
s32 SceneMover_IsNearTarget(void *, s32);
void OffsetActor_StepTowardTarget(void *);
void Sound_PlaySong(u32);

void Hresvelgr_PlaySideTargetSequence(struct HresvelgrTravelActor *actor)
{
    struct BossActionState *state = &actor->action;
    Hresvelgr_SetFlags(actor, 0x200000);
    if ((u8)Callback_InvokeActor(actor, actor->callback))
        return;
    if (actor->changed) {
        actor->changed = 0;
        ActionState_Reset(state);
        ActionState_SetState(state, 0);
    }
    Hresvelgr_SetFlags(actor, 0x200004);
    if (actor->pulse)
        Sound_PlaySong(0x3d9);
    switch (state->state) {
        case 0:
            if (state->frame == 0) {
                Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 0, 1, 0);
                SceneMover_SetSideTarget(actor);
                state->frame = 1;
            }
            if (SceneMover_IsNearTarget(actor, 4))
                ActionState_AdvanceState(state);
            else
                OffsetActor_StepTowardTarget(actor);
            break;
        case 1:
            actor->finished = 1;
            break;
    }
}
