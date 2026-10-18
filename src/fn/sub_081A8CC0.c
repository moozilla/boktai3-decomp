#define ActorSoundPhase_StartFlaggedAnimation sub_081A8CC0
#define ActorSoundPhase_SetMode Vanargand_SetPendingCue
#include "global.h"
struct ActorSoundPhase {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xb1-0xa0];
    u8 pendingAnimation;
    u8 padB2[0x104-0xb2];
    void *callback;
    u8 pad108[0x18c-0x108];
    u8 animation[0x60];
    u8 drawable[0x56c-0x1ec];
    void *attachment;
};
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void sub_081B6244(void *, s32, s32);
s32 Callback_InvokeActor(void *, void *);
void sub_081A8CC0(struct ActorSoundPhase *p)
{
    u32 flag = 0x40;
    p->flags |= flag;
    Vanargand_SetPendingCue(p, 0);
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        SceneActors_StartUnmirroredAnimation(p, p->animation, p->drawable, 2, 1, 4);
        sub_081B6244(p->attachment, 3, 2);
    }
    Callback_InvokeActor(p, p->callback);
}
