#include "hresvelgr_travel.h"

s32 Callback_InvokeActor(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Hresvelgr_UpdateTravel(struct HresvelgrTravelActor *);
void Hresvelgr_UpdateAlternatingMotion(struct HresvelgrTravelActor *actor, s32 ticks)
{
    Hresvelgr_SetFlags(actor, 0x600048);
    if (actor->changed) {
        actor->changed = 0;
        SceneActors_StartUnmirroredAnimation(actor, &actor->animation, &actor->animationData, 1, 1,
                                             4);
        actor->side = (actor->side + 1) & 1;
        Hresvelgr_RefreshTarget(actor);
    }
    Hresvelgr_SetFlags(actor, 0x200000);
    if (actor->pulse)
        Sound_PlaySong(0x3d9);
    if (!(u8)Callback_InvokeActor(actor, actor->callback)) {
        if (Hresvelgr_UpdateTravel(actor) || ticks > 0x80)
            actor->finished = 1;
    }
}
