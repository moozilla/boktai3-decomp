#define Hresvelgr_PlayTargetedTravel sub_081A0A88
#define Hresvelgr_GetTravelTarget    sub_0819C5D4
#define Hresvelgr_UpdateTravel       sub_0819CB7C
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
void sub_0819C5D4(HresvelgrActor *, HresvelgrPosition *);
s32 ColliderActor_SetTargetPosition(HresvelgrActor *, HresvelgrPosition *);
void Hresvelgr_SetAnimation(HresvelgrActor *, void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
int sub_0819CB7C(HresvelgrActor *);
void sub_081A0A88(HresvelgrActor *p)
{
    HresvelgrPosition target;
    if (p->changed) {
        p->changed = 0;
        sub_0819C5D4(p, &target);
        ColliderActor_SetTargetPosition(p, &target);
        Hresvelgr_SetAnimation(p, p->animation, p->animationData, 0, 1, 0);
    }
    {
        u32 mask = 0x200040;
        p->flags |= mask;
    }
    if (!(u8)Callback_InvokeActor(p, p->callback)) {
        if (p->pulse)
            Sound_PlaySong(0x3d9);
        if (sub_0819CB7C(p) || p->tick > 128)
            p->finished = 1;
    }
}
