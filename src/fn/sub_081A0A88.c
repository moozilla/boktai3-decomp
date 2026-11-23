#include "global.h"
#include "hresvelgr_travel.h"

s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_GetTravelTarget(struct HresvelgrTravelActor *, struct SceneVec *);
s32 ColliderActor_SetTargetPosition(struct HresvelgrTravelActor *, struct SceneVec *);
void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
int Hresvelgr_UpdateTravel(struct HresvelgrTravelActor *);

void Hresvelgr_PlayTargetedTravel(struct HresvelgrTravelActor *p)
{
    struct SceneVec target;
    if (p->changed) {
        p->changed = 0;
        Hresvelgr_GetTravelTarget(p, &target);
        ColliderActor_SetTargetPosition(p, &target);
        Hresvelgr_SetAnimation(p, &p->animation, &p->animationData, 0, 1, 0);
    }
    Hresvelgr_SetFlags(p, 0x200040);
    if (!(u8)Callback_InvokeActor(p, p->callback)) {
        if (p->pulse)
            Sound_PlaySong(0x3d9);
        if (Hresvelgr_UpdateTravel(p) || p->tick > 128)
            p->finished = 1;
    }
}
