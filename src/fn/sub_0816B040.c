#define SceneSequence_UpdateRamp64 sub_0816B040
#define SceneSequence_SetPhase Hud_SetUpdateCallback
#include "global.h"
struct SceneSequence {
    u8 unknown0[0xC4];
    u8 animationData[0xA4C-0xC4];
    u16 phaseTimer;
    u8 unknownA4E[0x3BE0-0xA4E];
    u8 animation[0x120];
    u8 secondaryAnimation[1];
};
void Hud_InitSelectionBounds(u32, u32);
s32 Anim_Step(void *, void *);
void Hud_SetUpdateCallback(void *, void (*)(void));
void sub_0816B0AC(void);
void sub_0816B040(struct SceneSequence *s)
{
    void *data, *initial;
    if (s->phaseTimer <= 15)
        Hud_InitSelectionBounds(s->phaseTimer * 4, 0);
    else
        Hud_InitSelectionBounds(64, 0);
    initial = s->animation;
    data = s->animationData;
    Anim_Step(initial, data);
    Anim_Step(s->secondaryAnimation, data);
    if (++s->phaseTimer > 15)
        Hud_SetUpdateCallback(s, sub_0816B0AC);
}
