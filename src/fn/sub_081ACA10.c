#define ActorSoundPhase_StartAnimation4 sub_081ACA10
#define ActorSoundPhase_SetMode Vanargand_SetPendingCue
#include "global.h"
struct ActorSoundPhase {
    u8 pad0[0x9c];
    u32 flags;
    u8 padA0[0xb1-0xa0];
    u8 pendingAnimation;
    u8 padB2[0x18c-0xb2];
    u8 animation[0x60];
    u8 drawable[0x5b0-0x1ec];
    u16 timer;
    u8 pad5b2[6];
    u16 mode;
    u16 phase;
};
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
void Sound_PlaySong(u32);
void sub_081ACA10(struct ActorSoundPhase *p)
{
    u32 flag = 4;
    p->flags |= flag;
    Vanargand_SetPendingCue(p, 3);
    if (p->pendingAnimation) {
        p->pendingAnimation = 0;
        p->mode = flag;
        p->phase = flag;
        SceneActors_StartUnmirroredAnimation(p, p->animation, p->drawable, 4, 1, 0);
        Sound_PlaySong(0x586);
    }
    p->timer = 2;
}
