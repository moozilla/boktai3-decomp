#include "global.h"
void Anim_Step(void *, void *);
void SceneHud_FadeOutPage(void);
void Hud_SetUpdateCallback(void *, void (*)(void));
void Hud_UpdateFirstPostScriptWait(u8 *s)
{
    Anim_Step(s + 0x3BE0, s + 0xC4);
    (*(u16 *)(s + 0xA4C))++;
    if (*(u16 *)(s + 0xA4C) > 0x95)
        Hud_SetUpdateCallback(s, SceneHud_FadeOutPage);
}
