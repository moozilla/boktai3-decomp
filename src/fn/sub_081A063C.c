#include "hresvelgr_travel.h"
s32 Callback_InvokeActor(void *, void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void Sound_PlaySong(u32);
s32 Hresvelgr_UpdateTravel(struct HresvelgrTravelActor *);
void Hresvelgr_UpdateAlternatingMotion(struct HresvelgrTravelActor *actor, s32 ticks)
{
    u8 *p = (u8 *)actor;
    u32 mask = 0x600048;
    u32 *flags = &actor->flags;
    *flags |= mask;
    if (actor->changed) {
        actor->changed = 0;
        SceneActors_StartUnmirroredAnimation(p, actor->animation, actor->animationData, 1, 1, 4);
        actor->side = (actor->side + 1) & 1;
        Hresvelgr_RefreshTarget(actor);
    }
    {
        u32 mask = 0x200000;
        *flags |= mask;
    }
    if (actor->pulse)
        Sound_PlaySong(0x3d9);
    if (!(u8)Callback_InvokeActor(p, actor->callback)) {
        if (Hresvelgr_UpdateTravel(actor) || ticks > 0x80)
            actor->finished = 1;
    }
}
