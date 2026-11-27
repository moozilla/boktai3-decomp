#include "global.h"
#include "bosses/ratatosk/ratatosk.h"

/* Ratatosk field and state interpretations are provisional (low confidence). */
struct ActorAnimationChannel {
    u8 pad[8];
    u32 flags;
};
s32 Ratatosk_GetSelectedPointIndex(void);
void ScaledShadow_Disable(void *);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
s32 Callback_InvokeActor(void *, void *);
void SlotController_SetSlotFlag2(s32);
void Ratatosk_UpdateShrinkingAnimation(struct Ratatosk *p, s32 frame)
{
    if (p->actionChanged) {
        p->actionChanged = 0;
        ScaledShadow_Disable(p->shadow);
        {
            struct ActorAnimationChannel *animation = &p->animationData;
            SceneActors_StartUnmirroredAnimation(p, animation, p->animation, 25, 1, 4);
            animation->flags |= 2;
        }
        p->fade = 64;
        Sound_PlaySong(0x572);
    }
    p->fade -= 2;
    p->spriteScaleX = p->fade;
    p->spriteScaleY = p->fade;
    if (p->locked)
        p->anchor.y += 128;
    Ratatosk_SetFlags(p, 0x8000c);
    SlotController_SetSlotFlag2(Ratatosk_GetSelectedPointIndex());
    if (frame > 299 || p->fade <= 1) {
        p->fade = 1;
        Ratatosk_SetFlags(p, 16);
        p->actionFinished = 1;
    }
    Callback_InvokeActor(p, p->actionTryUpdate);
}
