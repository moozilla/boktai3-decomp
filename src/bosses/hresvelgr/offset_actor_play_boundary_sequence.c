#include "engine/shared/global.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Actor_SetHeadingVelocity(struct HresvelgrTravelActor *, u32);

static inline u8 OutsideBoundary(struct HresvelgrTravelActor *actor)
{
    u8 outside = 0;
    if (actor->position.x > 0x2000 || actor->position.z > 0x2000)
        outside = 1;
    return outside;
}

void OffsetActor_PlayBoundarySequence(struct HresvelgrTravelActor *actor)
{
    struct BossActionState *state = &actor->action;
    if (actor->changed) {
        actor->changed = 0;
        ActionState_Reset(state);
    }
    Hresvelgr_SetFlags(actor, 0x20000);
    switch (state->state) {
        case 0:
            if (state->frame == 0) {
                Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 2, 1, 0);
                state->frame = 1;
            }
            Actor_SetHeadingVelocity(actor, 0x20);
            if (OutsideBoundary(actor))
                ActionState_SetState(state, 1);
            break;
        case 1:
            actor->finished = 1;
            break;
    }
}
