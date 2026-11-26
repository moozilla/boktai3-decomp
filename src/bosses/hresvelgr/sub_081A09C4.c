#include "bosses/hresvelgr/hresvelgr_travel.h"

void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
void OffsetActor_StepTowardTarget(struct HresvelgrTravelActor *);
void Sound_PlaySong(u32);
/* Returns TRUE once the actor has reached the landing height. */
static inline s32 Hresvelgr_Descend(struct HresvelgrTravelActor *actor)
{
    Hresvelgr_SetFlags(actor, 0x200000);
    actor->flags |= 12;
    if (actor->changed) {
        actor->changed = 0;
        Hresvelgr_RefreshTarget(actor);
        actor->position = actor->target;
        actor->position.y = 0x2000;
        Hresvelgr_SetAnimation(actor, &actor->animation, &actor->animationData, 0, 1, 0);
        Anim_SetFrameDurationScale(&actor->animation, 128);
    }
    if (actor->pulse)
        Sound_PlaySong(0x3d9);
    OffsetActor_StepTowardTarget(actor);
    if (actor->position.y > 0x340)
        return 0;
    actor->position.y = 0x340;
    actor->finished = 1;
    return 1;
}
void Hresvelgr_PlayDescent(void *p, int tick)
{
    struct HresvelgrTravelActor *actor = p;
    if (!Hresvelgr_Descend(p) && !(tick & 31))
        Hresvelgr_RefreshTarget(actor);
}
