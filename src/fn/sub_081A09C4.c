#include "hresvelgr_travel.h"

// MATCHING_CONCESSION: register-pinning
void ActionState_Reset(u8 *);
void ActionState_AdvanceState(u8 *);
void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
void OffsetActor_StepTowardTarget(struct HresvelgrTravelActor *);
void Sound_PlaySong(u32);
void Hresvelgr_PlayDescent(void *p, int tick)
{
    struct HresvelgrTravelActor *actor = p;
    asm("" : "+r"(actor));
    {
        u32 mask = 0x200000;
        actor->flags |= mask;
        actor->flags |= 12;
    }
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_RefreshTarget(actor);
        actor->position = actor->target;
        actor->position.y = 0x2000;
        Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData, 0, 1, 0);
        Anim_SetFrameDurationScale(actor->animation, 128);
    }
    if (actor->pulse)
        Sound_PlaySong(0x3d9);
    OffsetActor_StepTowardTarget(actor);
    if (actor->position.y <= 0x340) {
        actor->position.y = 0x340;
        actor->finished = 1;
    } else if (!(tick & 31))
        Hresvelgr_RefreshTarget(p);
}
