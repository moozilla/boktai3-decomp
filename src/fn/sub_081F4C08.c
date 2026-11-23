#include "global.h"
#include "ratatosk.h"
struct RatatoskAnimation {
    u32 unknown[2];
    u32 flags;
    u8 padC[0x20 - 12];
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
    if (p->actionChanged) {
        struct RatatoskAnimation *a;
        p->actionChanged = 0;
        ScaledShadow_Disable(p->shadow);
        a = &p->animationData;
        SceneActors_StartUnmirroredAnimation(p, a, p->animation, 0x19, 1, 4);
        a->flags |= 2;
        p->fade = 0;
        Sound_PlaySong(0x573);
    }
    p->fade += 2;
    p->spriteScaleX = p->fade;
    p->spriteScaleY = p->fade;
    if (p->locked)
        p->anchor.y += 0x80;
    {
        u32 mask = 0x8000c;
        p->flags |= mask;
    }
    SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
    if (timer > 299 || p->fade > 63) {
        p->fade = 64;
        {
            struct RatatoskAnimation *a = &p->animationData;
            a->flags &= ~2;
        }
        ScaledShadow_Enable(p->shadow);
        p->actionFinished = 1;
    }
    Callback_InvokeActor(p, p->actionTryUpdate);
}
