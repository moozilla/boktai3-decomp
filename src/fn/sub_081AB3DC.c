#define ActorSoundPhase_SetMode Vanargand_SetPendingCue
#include "global.h"
struct ActorSoundPhase {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xb1 - 0xa0];
    u8 pendingAnimation;
    u8 padB2[0x18c - 0xb2];
    u8 animation[0x60];
    u8 drawable[0x5b0 - 0x1ec];
    u16 timer;
};
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void ActorSoundPhase_StartTimedAnimation(struct ActorSoundPhase *p)
{
    void *animation = p->animation;
    u32 mask = 5;
    p->flags = (p->flags | mask) | 0x40;
    Vanargand_SetPendingCue(p, 3);
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        SceneActors_StartUnmirroredAnimation(p, animation, p->drawable, 4, 3, 4);
        p->timer = 8;
    }
}
