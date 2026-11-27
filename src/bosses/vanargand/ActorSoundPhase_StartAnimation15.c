#define ActorSoundPhase_SetMode Vanargand_SetPendingCue
#include "global.h"
struct ActorSoundPhase {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xb1 - 0xa0];
    u8 pendingAnimation;
    u8 padB2[0x18c - 0xb2];
    u8 animation[0x60];
    u8 drawable[0x5b8 - 0x1ec];
    u16 mode;
    u16 phase;
};
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Anim_SetFrameDurationScale(void *, s32);
void ActorSoundPhase_StartAnimation15(struct ActorSoundPhase *p)
{
    u32 flag = 4;
    p->flags |= flag;
    Vanargand_SetPendingCue(p, 3);
    if (p->pendingAnimation) {
        void *animation;
        u32 one, two;
        p->pendingAnimation = 0;
        one = 1;
        two = 2;
        p->mode = one;
        p->phase = two;
        animation = p->animation;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 15, two, 0);
        Anim_SetFrameDurationScale(animation, 0x80);
    }
}
