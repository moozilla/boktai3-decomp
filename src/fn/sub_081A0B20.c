#include "global.h"
/* Tentative Hresvelgr layout and behavior names, inferred from static consumers. */
typedef struct HresvelgrPosition {
    s16 x, y, z, pad;
} HresvelgrPosition;
typedef struct HresvelgrActor {
    u8 pad0[0x90];
    u32 tick;
    u8 pad94[8];
    u32 flags;
    u8 padA0[15];
    u8 finished;
    u8 padB0;
    u8 changed;
    u8 pulse;
    u8 padB3[0x51];
    void *callback;
    u8 pad108[0x6c];
    u8 animationData[0x20];
    u8 animation[0x20];
} HresvelgrActor;
s32 Callback_InvokeActor(void *, void *);
void Hresvelgr_GetTravelTarget(HresvelgrActor *, HresvelgrPosition *);
s32 ColliderActor_SetTargetPosition(HresvelgrActor *, HresvelgrPosition *);
void Hresvelgr_SetAnimation(HresvelgrActor *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
int Hresvelgr_UpdateTravel(HresvelgrActor *);
int Path_SnapToNearestPoint(HresvelgrPosition *, HresvelgrPosition *, int);
void Hresvelgr_PlayNearestPointTravel(HresvelgrActor *body)
{
    HresvelgrActor *actor = body;
    HresvelgrPosition target, nearest;
    {
        u32 mask = 0x200040;
        body->flags |= mask;
    }
    if (body->changed) {
        body->changed = 0;
        Hresvelgr_GetTravelTarget(body, &target);
        if (Path_SnapToNearestPoint(&nearest, &target, 0) < 0) {
            body->finished = 1;
            return;
        }
        ColliderActor_SetTargetPosition(body, &nearest);
        Hresvelgr_SetAnimation(body, body->animation, body->animationData, 0, 1, 0);
    }
    if (!(u8)Callback_InvokeActor(body, body->callback)) {
        if (body->pulse)
            Sound_PlaySong(0x3d9);
        if (Hresvelgr_UpdateTravel(actor) || body->tick > 128) {
            body->finished = 1;
        }
    }
}
