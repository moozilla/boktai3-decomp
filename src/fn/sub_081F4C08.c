#include "global.h"
struct RatatoskAnimation {
    u32 unknown[2];
    u32 flags;
    u8 padC[0x20 - 12];
};
struct Ratatosk {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xaf - 0xa0];
    u8 complete;
    u8 padB0;
    u8 entering;
    u8 padB2;
    u8 rotate;
    u8 padB4[0x104 - 0xb4];
    void *callback;
    u8 pad108[0x32c - 0x108];
    u8 animData[0x20];
    struct RatatoskAnimation anim;
    u8 pad36C[2];
    u16 angle;
    u8 pad370[0x380 - 0x370];
    u8 fadeA, fadeB;
    u8 pad382[0x40c - 0x382];
    u8 shadow[0xed8 - 0x40c];
    u16 fade;
};
s32 Ratatosk_GetSelectedPointIndex(void);
void ScaledShadow_Disable(void *);
void ScaledShadow_Enable(void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void SlotController_SetSlotFlag2(s32);
s32 Callback_InvokeActor(void *, void *);
void Ratatosk_UpdateFadeIn(struct Ratatosk *p, s32 timer)
{
    if (p->entering) {
        struct RatatoskAnimation *a;
        p->entering = 0;
        ScaledShadow_Disable(p->shadow);
        a = &p->anim;
        SceneActors_StartUnmirroredAnimation(p, a, p->animData, 0x19, 1, 4);
        a->flags |= 2;
        p->fade = 0;
        Sound_PlaySong(0x573);
    }
    p->fade += 2;
    p->fadeA = p->fade;
    p->fadeB = p->fade;
    if (p->rotate)
        p->angle += 0x80;
    {
        u32 mask = 0x8000c;
        p->flags |= mask;
    }
    SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
    if (timer > 299 || p->fade > 63) {
        p->fade = 64;
        {
            struct RatatoskAnimation *a = &p->anim;
            a->flags &= ~2;
        }
        ScaledShadow_Enable(p->shadow);
        p->complete = 1;
    }
    Callback_InvokeActor(p, p->callback);
}
