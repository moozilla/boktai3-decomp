#define SceneSequence_SetPhase Hud_SetUpdateCallback
#include "global.h"
struct SceneSequence {
    u8 unknown0[0xC4];
    u8 animationData[0x3BE0-0xC4];
    u8 animation[8];
    u32 animationFlags;
    u8 unknown3BEC[0x14];
    u16 animationX;
    u16 animationY;
};
void PauseMenu_HideAllSprites(void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Sound_PlaySong(u32);
s32 MessageWindow_ShowMessage(s32);
void Hud_SetUpdateCallback(void *, void (*)(void));
void SceneSequence_SlideAnimationLeft(void);
void SceneSequence_BeginAnimation6(struct SceneSequence *s)
{
    u32 zero;
    u16 *p;
    PauseMenu_HideAllSprites(s);
    s->animationFlags &= ~1;
    p = &s->animationX;
    zero = 0;
    { u32 v = 0xFFE4; *p = v; }
    s->animationY = 0x20;
    Anim_SetKeyframes(s->animation, s->animationData, 6, 2, zero);
    Sound_PlaySong(0x2C1);
    MessageWindow_ShowMessage(3);
    Hud_SetUpdateCallback(s, SceneSequence_SlideAnimationLeft);
}
