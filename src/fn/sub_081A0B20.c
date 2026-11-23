#include "global.h"
#include "hresvelgr_travel.h"
s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_GetTravelTarget(struct HresvelgrTravelActor *, struct SceneVec *);
s32 ColliderActor_SetTargetPosition(struct HresvelgrTravelActor *, struct SceneVec *);
void Hresvelgr_SetAnimation(struct HresvelgrTravelActor *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
int Hresvelgr_UpdateTravel(struct HresvelgrTravelActor *);
int Path_SnapToNearestPoint(struct SceneVec *, struct SceneVec *, int);
void Hresvelgr_PlayNearestPointTravel(struct HresvelgrTravelActor *body)
{
    struct HresvelgrTravelActor *actor = body;
    struct SceneVec target, nearest;
    Hresvelgr_SetFlags(body, 0x200040);
    if (body->changed) {
        body->changed = 0;
        Hresvelgr_GetTravelTarget(body, &target);
        if (Path_SnapToNearestPoint(&nearest, &target, 0) < 0) {
            body->finished = 1;
            return;
        }
        ColliderActor_SetTargetPosition(body, &nearest);
        Hresvelgr_SetAnimation(body, &body->animation, &body->animationData, 0, 1, 0);
    }
    if (!(u8)Callback_InvokeActor(body, body->callback)) {
        if (body->pulse)
            Sound_PlaySong(0x3d9);
        if (Hresvelgr_UpdateTravel(actor) || body->tick > 128) {
            body->finished = 1;
        }
    }
}
