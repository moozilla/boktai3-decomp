// MATCHING_CONCESSION: register-pinning
#include "global.h"

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
void Hresvelgr_PlayNearestPointTravel(HresvelgrActor *p)
{
    HresvelgrPosition target, nearest;
    HresvelgrActor *actor = p;
    register u32 zero asm("r6");
    asm("" : "+r"(actor));
    {
        u32 mask = 0x200040;
        p->flags |= mask;
    }
    if (p->changed) {
        zero = 0;
        asm("" : "+r"(zero));
        p->changed = zero;
        Hresvelgr_GetTravelTarget(p, &target);
        if (Path_SnapToNearestPoint(&nearest, &target, 0) < 0)
            goto done;
        ColliderActor_SetTargetPosition(p, &nearest);
        Hresvelgr_SetAnimation(p, p->animation, p->animationData, 0, 1, zero);
    }
    if (!(u8)Callback_InvokeActor(p, p->callback)) {
        if (p->pulse)
            Sound_PlaySong(0x3d9);
        if (Hresvelgr_UpdateTravel(actor) || p->tick > 128) {
        done:
            p->finished = 1;
        }
    }
}
