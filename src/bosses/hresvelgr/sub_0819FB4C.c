#include "engine/shared/global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void OffsetActor_StepTowardTarget(struct HresvelgrTravelActor *);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);

static inline void SetTarget(struct SceneVec *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void OffsetActor_PlayTargetSequence(struct HresvelgrTravelActor *object)
{
    struct HresvelgrTravelActor *actor = object;
    struct BossActionState *state = &actor->action;
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 0, 1, 0);
        ActionState_Reset(state);
    }
    Hresvelgr_SetFlags(actor, 0x600040);
    if (actor->pulse)
        Sound_PlaySong(0x3d9);
    switch (state->state) {
        case 0:
            if (state->frame > 0x80)
                ActionState_AdvanceState(state);
            else {
                struct SceneVec *origin = actor->origin;
                if (!actor->side)
                    SetTarget(&actor->target, 0x400, origin->y, origin->z);
                else
                    SetTarget(&actor->target, origin->x, origin->y, 0x400);
                OffsetActor_StepTowardTarget(object);
                state->frame++;
            }
            break;
        case 1:
            actor->finished = 1;
            break;
    }
    Callback_InvokeActor(actor, actor->callback);
}
