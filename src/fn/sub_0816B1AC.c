#define SceneSequence_SlideAnimationLeft sub_0816B1AC
#define SceneSequence_SetPhase Hud_SetUpdateCallback
#include "global.h"
struct SceneSequence {
    u8 unknown0[0xC4];
    u8 animationData[0xA4C-0xC4];
    u16 phaseTimer;
    u8 unknownA4E[0x3BE0-0xA4E];
    u8 animation[0x20];
    s16 animationX;
};
s32 Anim_Step(void *, void *);
void Hud_SetUpdateCallback(void *, void (*)(void));
void sub_08176B00(void);
void sub_0816B1AC(struct SceneSequence *s)
{
    Anim_Step(s->animation, s->animationData);
    if (s->animationX > -88)
        s->animationX -= 5;
    if (++s->phaseTimer > 149)
        Hud_SetUpdateCallback(s, sub_08176B00);
}
